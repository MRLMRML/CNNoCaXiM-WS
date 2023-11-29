#include "Controller.h"

void Controller::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		switch (m_controllerState)
		{
		case ControllerState::N:
			sendReadWeightRequest();
			break;
		case ControllerState::W:
			receiveReadWeightResponse();
			break;
		case ControllerState::K:
			assemblePacket();
			break;
		case ControllerState::I:
			receiveReadInputResponse();
			break;
		case ControllerState::O:
			assemblePacket();
			break;
		case ControllerState::B:
			receiveWriteOutputResponse();
			break;
		}
	}

	sendFlit(); // every cycle; it may stall when network is busy
}

void Controller::viewPacket(const Packet& packet)
{
#if defined (DEBUG)
	std::cout << packet << std::endl;
#endif
}

void Controller::viewFlit(const Flit& flit)
{
#if defined (DEBUG)
	std::cout << flit << std::endl;
#endif
}

void Controller::sendReadWeightRequest()
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_NW - 1);
		m_localClock->toggleWaitingForExecution();
	}

	if (m_localClock->executeLocalEvent())
	{
		m_masterInterface.readAddressChannel.ARVALID = true;
		m_masterInterface.readAddressChannel.ARID = -1; // special xID for weights
		m_masterInterface.readAddressChannel.ARADDR = WEIGHT_DATA_START_LOCATION + m_weightDataIndex;
		++m_weightDataIndex;
		m_controllerState = ControllerState::W;

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void Controller::receiveReadWeightResponse()
{
	if (m_masterInterface.readDataChannel.RREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_WK - 1);
			m_localClock->toggleWaitingForExecution();

			m_timer->recordStartTime();
		}

		if (m_localClock->executeLocalEvent())
		{
			receiveWriteWeightRequest();
		}
	}
}

void Controller::receiveWriteWeightRequest()
{
	Packet writeRequest{};
	writeRequest.destination = m_PEID;
	writeRequest.xID = m_masterInterface.readDataChannel.RID; // should be -1
	writeRequest.RWQB = PacketType::WriteRequest;
	writeRequest.MID = m_PEID;
	writeRequest.SID = m_NID;
	writeRequest.SEQID = m_masterInterface.readDataChannel.RID; // should be -1
	writeRequest.xDATA = m_masterInterface.readDataChannel.RDATA;

	sendPacket(writeRequest);

	m_timer->recordPacketTimeInitializeFinish(writeRequest.SEQID);

	m_masterInterface.readDataChannel.RREADY = true;
	m_controllerState = ControllerState::K;

	m_localClock->tickTriggerClock(1);
	m_localClock->tickExecutionClock(1);
	m_localClock->toggleWaitingForExecution();
}

void Controller::sendReadInputRequest()
{
	m_masterInterface.readAddressChannel.ARVALID = true;
	m_masterInterface.readAddressChannel.ARID = m_inputDataIndex;
	m_masterInterface.readAddressChannel.ARADDR = INPUT_DATA_START_LOCATION + m_inputDataIndex;
	++m_inputDataIndex;
}

void Controller::reformInputData(const std::vector<DATA_PRECISION>& inputData)
{
	for (int oy{}; oy < OUTPUT_DIMENSION; ++oy)
	{
		for (int ox{}; ox < OUTPUT_DIMENSION; ++ox)
		{
			std::vector<DATA_PRECISION> reformedInputData{};
			for (int iy{}; iy < WEIGHT_DIMENSION; ++iy)
			{
				for (int ix{}; ix < WEIGHT_DIMENSION; ++ix)
				{
					reformedInputData.push_back(inputData.at(ox + oy * INPUT_DIMENSION + ix + iy * INPUT_DIMENSION));
				}
			}
			m_reformedInputData.push_back(reformedInputData);
		}
	}
}

void Controller::receiveReadInputResponse()
{
	if (m_masterInterface.readDataChannel.RREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_IO - 1);
			m_localClock->toggleWaitingForExecution();

			m_timer->recordStartTime();
		}

		if (m_localClock->executeLocalEvent())
		{
			reformInputData(m_masterInterface.readDataChannel.RDATA);

			Packet readResponse{};
			readResponse.destination = m_PEID;
			readResponse.xID = m_masterInterface.readDataChannel.RID;
			readResponse.RWQB = PacketType::ReadResponse;
			readResponse.MID = m_PEID;
			readResponse.SID = m_NID;
			readResponse.SEQID = m_reformedInputDataIndex; // keep the reformed input data sequenced
			readResponse.xDATA = m_reformedInputData.at(m_reformedInputDataIndex);

			sendPacket(readResponse);
			
			m_timer->recordPacketTimeInitializeFinish(readResponse.SEQID);

			++m_reformedInputDataIndex;
			
			m_masterInterface.readDataChannel.RREADY = true;
			m_controllerState = ControllerState::O;

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void Controller::sendPacket(const Packet& packet)
{
	// only triggers when there is a packet to send
	dismantlePacket(packet);
	logDebug(" NI: packet sent ");
	viewPacket(packet);
}

void Controller::dismantlePacket(const Packet& packet)
{
	//double flitCount{ ceil(PACKET_SIZE / FLIT_SIZE) }; // number of flits in total
	//double flitCount{ ceil(sizeof(packet) / FLIT_SIZE) }; // number of flits in total
	double flitCount{ static_cast<double>(packet.xDATA.size()) };

	if (flitCount == 0 || flitCount == 1) // H/T flit
	{
		Flit headTailFlit{ PortType::Unselected, -1, FlitType::HeadTailFlit, packet };
		m_sourceQueue.push_back(headTailFlit);
		viewFlit(headTailFlit);
		return;
	}

	if (flitCount == 2) // no BodyFlit
	{
		Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, packet.destination, packet.xID,
			packet.RWQB, packet.MID, packet.SID, packet.SEQID };
		m_sourceQueue.push_back(headFlit);
		viewFlit(headFlit);

		Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, packet.xID, packet.MID, packet.SEQID,
			packet.AxADDR, packet.xDATA };
		m_sourceQueue.push_back(tailFlit);
		viewFlit(tailFlit);
		return;
	}

	// HeadFlit, BodyFlit x (flitCount - 2), TailFlit
	// HeadFlit
	Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, packet.destination, packet.xID,
			packet.RWQB, packet.MID, packet.SID, packet.SEQID };
	m_sourceQueue.push_back(headFlit);
	viewFlit(headFlit);

	// BodyFlit
	for (int i{}; i < (flitCount - 2); ++i)
	{
		Flit bodyFlit{ PortType::Unselected, -1, FlitType::BodyFlit, i + 1, packet.xID, packet.MID, packet.SEQID };
		m_sourceQueue.push_back(bodyFlit);
		viewFlit(bodyFlit);
	}

	// TailFlit
	Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, packet.xID, packet.MID, packet.SEQID,
			packet.AxADDR, packet.xDATA };
	m_sourceQueue.push_back(tailFlit);
	viewFlit(tailFlit);
}

void Controller::sendFlit()
{
	if (!m_sourceQueue.empty()
		&& m_port.m_outFlitRegister.size() < REGISTER_DEPTH)
	{
		m_port.m_outFlitRegister.push_back(m_sourceQueue.front());
		if (m_sourceQueue.front().flitType == FlitType::HeadFlit ||
			m_sourceQueue.front().flitType == FlitType::HeadTailFlit)
			m_timer->recordPacketTimeAppendFinish(m_sourceQueue.front().SEQID);
		m_sourceQueue.pop_front();
	}
}

bool Controller::receiveFlit()
{
	if (!m_port.m_inFlitRegister.empty())
	{
		Flit flit{ m_port.m_inFlitRegister.front() };
		m_port.m_inFlitRegister.pop_front();
		m_flitReorderBuffer.push_back(flit);
		return true;
	}
	else
		return false;
}

void Controller::assemblePacket()
{
	if (receiveFlit())
	{
		if (m_flitReorderBuffer.back().flitType == FlitType::HeadTailFlit)
		{
			Packet packet{ m_flitReorderBuffer.back().destination, m_flitReorderBuffer.back().xID,
				m_flitReorderBuffer.back().RWQB, m_flitReorderBuffer.back().MID, m_flitReorderBuffer.back().SID,
				m_flitReorderBuffer.back().SEQID, m_flitReorderBuffer.back().AxADDR, m_flitReorderBuffer.back().xDATA };
			m_flitReorderBuffer.pop_back();
			receivePacket(packet);
			return;
		}

		if (m_flitReorderBuffer.back().flitType == FlitType::TailFlit)
		{
			std::vector<Flit> t_flitReorderBuffer{ m_flitReorderBuffer };
			for (auto& flit : t_flitReorderBuffer) // find the flits that have the same IDs and erase them from flit reorder buffer
			{
				if (flit.xID == m_flitReorderBuffer.back().xID &&
					flit.MID == m_flitReorderBuffer.back().MID &&
					flit.SEQID == m_flitReorderBuffer.back().SEQID)
				{
					if (flit.flitType == FlitType::HeadFlit) // find the head flit and get the data
					{
						Packet packet{ flit.destination, flit.xID,
						flit.RWQB, flit.MID, flit.SID,
						flit.SEQID, m_flitReorderBuffer.back().AxADDR, m_flitReorderBuffer.back().xDATA };
						receivePacket(packet);
					}
					std::erase(m_flitReorderBuffer, flit); // C++20
					//m_flitReorderBuffer.erase(std::remove(m_flitReorderBuffer.begin(), m_flitReorderBuffer.end(), flit), m_flitReorderBuffer.end());
				}
			}
			return;
		}
	}
}

void Controller::receivePacket(const Packet& packet)
{
	logDebug(" NI: packet received ");
	viewPacket(packet);

	//if (packet.RWQB == PacketType::ReadRequest)
	//{
	//	sendReadRequest(packet);
	//	return;
	//}
	//if (packet.RWQB == PacketType::ReadResponse)
	//{
	//	sendReadResponse(packet);
	//	return;
	//}
	if (packet.RWQB == PacketType::WriteRequest)
	{
		sendWriteOutputRequest(packet);
		return;
	}
	if (packet.RWQB == PacketType::WriteResponse)
	{
		sendWriteWeightResponse(packet);
		return;
	}
}

void Controller::sendWriteWeightResponse(const Packet& packet)
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_KI - 1);
		m_localClock->toggleWaitingForExecution();

		m_timer->recordPacketTimeAppendStart(packet.SEQID);
	}

	if (m_localClock->executeLocalEvent())
	{
		if (packet.xID == -1)
		{
			m_timer->recordFinishTime();

			m_controllerState = ControllerState::I;

			sendReadInputRequest();

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
		else
			throw std::runtime_error{ " Controller: xID should be -1 for weights " };
	}
}

void Controller::sendWriteOutputRequest(const Packet& packet)
{
	m_outputData.insert(m_outputData.end(), packet.xDATA.begin(), packet.xDATA.end());
	m_timer->recordPacketTimeAppendStart(packet.SEQID);
	if (m_outputData.size() == m_reformedInputData.size())
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_OB - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			m_masterInterface.writeAddressChannel.AWVALID = true;
			m_masterInterface.writeAddressChannel.AWID = packet.xID;
			m_masterInterface.writeAddressChannel.AWADDR = OUTPUT_DATA_START_LOCATION + packet.AxADDR;

			m_masterInterface.writeDataChannel.WVALID = true;
			m_masterInterface.writeDataChannel.WDATA = m_outputData;

			m_timer->recordFinishTime();

			m_controllerState = ControllerState::B;

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
	else
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_OO - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			if (m_reformedInputDataIndex < m_reformedInputData.size())
			{
				Packet readResponse{};
				readResponse.destination = m_PEID;
				readResponse.xID = m_masterInterface.readDataChannel.RID;
				readResponse.RWQB = PacketType::ReadResponse;
				readResponse.MID = m_PEID;
				readResponse.SID = m_NID;
				readResponse.SEQID = m_reformedInputDataIndex; // keep the reformed input data sequenced
				readResponse.xDATA = m_reformedInputData.at(m_reformedInputDataIndex);

				sendPacket(readResponse);

				m_timer->recordPacketTimeInitializeFinish(readResponse.SEQID);

				++m_reformedInputDataIndex;

				m_controllerState = ControllerState::O;

				m_localClock->tickTriggerClock(1);
				m_localClock->tickExecutionClock(1);
				m_localClock->toggleWaitingForExecution();
			}
		}
	}
}

void Controller::receiveWriteOutputResponse()
{
	if (m_masterInterface.writeResponseChannel.BREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_CONTROLLER_BI - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			m_reformedInputData.clear();
			m_outputData.clear();
			m_reformedInputDataIndex = 0;

			m_masterInterface.writeResponseChannel.BREADY = true;
			m_controllerState = ControllerState::I;

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}