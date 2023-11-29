#include "NI.h"

void NI::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		switch (m_niState)
		{
		case NIState::W:
			assemblePacket();
			break;
		case NIState::K:
			receiveWriteResponse();
			break;
		case NIState::I:
			assemblePacket();
			break;
		case NIState::O:
			receiveWriteRequest();
			break;
		}
	}

	sendFlit(); // every cycle; it may stall when network is busy
}

void NI::viewPacket(const Packet& packet)
{
#if defined (DEBUG)
	std::cout << packet << std::endl;
#endif
}

void NI::viewFlit(const Flit& flit)
{
#if defined (DEBUG)
	std::cout << flit << std::endl;
#endif
}

bool NI::receiveFlit()
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

void NI::assemblePacket()
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

void NI::receivePacket(const Packet& packet)
{
	logDebug(" NI: packet received ");
	viewPacket(packet);

	//if (packet.RWQB == PacketType::ReadRequest)
	//{
	//	sendReadRequest(packet);
	//	return;
	//}
	if (packet.RWQB == PacketType::ReadResponse)
	{
		sendReadResponse(packet);
		return;
	}
	if (packet.RWQB == PacketType::WriteRequest)
	{
		sendWriteRequest(packet);
		return;
	}
	//if (packet.RWQB == PacketType::WriteResponse)
	//{
	//	sendWriteOutputResponse(packet);
	//	return;
	//}
}

void NI::sendWriteRequest(const Packet& packet)
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(EXECUTION_TIME_NI_WK - 1);
		m_localClock->toggleWaitingForExecution();

		m_timer->recordPacketTimeAppendStart(packet.SEQID);
	}

	if (m_localClock->executeLocalEvent())
	{
		m_masterInterface.writeAddressChannel.AWVALID = true;
		m_masterInterface.writeAddressChannel.AWID = packet.SEQID; // should be -1,AWID is SEQID instead of xID, for timing purpose

		m_masterInterface.writeDataChannel.WVALID = true;
		m_masterInterface.writeDataChannel.WDATA = packet.xDATA;

		m_xID = packet.xID; // should be -1
		m_DRAMID = packet.SID; // DRAMID is SID in packet

		m_niState = NIState::K;

		m_timer->recordPacketTimeAppendFinish(packet.SEQID);

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void NI::sendReadResponse(const Packet& packet)
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(EXECUTION_TIME_NI_IO - 1);
		m_localClock->toggleWaitingForExecution();

		m_timer->recordPacketTimeAppendStart(packet.SEQID);
	}

	if (m_localClock->executeLocalEvent())
	{
		m_slaveInterface.readDataChannel.RVALID = true;
		m_slaveInterface.readDataChannel.RID = packet.SEQID; // RID is SEQID instead of xID, for timing purpose
		m_slaveInterface.readDataChannel.RDATA = packet.xDATA;

		m_xID = packet.xID;
		m_DRAMID = packet.SID; // DRAMID is SID in packet

		m_niState = NIState::O;

		m_timer->recordPacketTimeAppendFinish(packet.SEQID);
		
		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void NI::receiveWriteResponse()
{
	if (m_masterInterface.writeResponseChannel.BREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_NI_KI - 1);
			m_localClock->toggleWaitingForExecution();

			m_timer->recordPacketTimeAppendStart(m_masterInterface.writeResponseChannel.BID);
		}

		if (m_localClock->executeLocalEvent())
		{
			Packet writeResponse{};
			writeResponse.destination = m_DRAMID;
			writeResponse.xID = m_xID;
			writeResponse.RWQB = PacketType::WriteResponse;
			writeResponse.MID = m_DRAMID;
			writeResponse.SID = m_NID;
			writeResponse.SEQID = m_masterInterface.writeResponseChannel.BID; // just for timing purpose

			sendPacket(writeResponse);

			m_timer->recordPacketTimeAppendFinish(writeResponse.SEQID);

			m_masterInterface.writeResponseChannel.BREADY = true;
			m_niState = NIState::I;

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void NI::receiveWriteRequest()
{
	if (m_slaveInterface.writeAddressChannel.AWREADY == false && m_slaveInterface.writeDataChannel.WREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_NI_OI - 1);
			m_localClock->toggleWaitingForExecution();

			m_timer->recordPacketTimeAppendStart(m_slaveInterface.writeAddressChannel.AWID);
		}

		if (m_localClock->executeLocalEvent())
		{
			Packet writeRequest{};
			writeRequest.destination = m_DRAMID;
			writeRequest.xID = m_xID;
			writeRequest.RWQB = PacketType::WriteRequest;
			writeRequest.MID = m_NID;
			writeRequest.SID = m_DRAMID;
			writeRequest.SEQID = m_slaveInterface.writeAddressChannel.AWID;
			writeRequest.AxADDR = m_xID; // AxADDR is the xID of the packet
			writeRequest.xDATA = m_slaveInterface.writeDataChannel.WDATA;

			sendPacket(writeRequest);

			m_timer->recordPacketTimeAppendFinish(writeRequest.SEQID);

			m_slaveInterface.writeAddressChannel.AWREADY = true;
			m_slaveInterface.writeDataChannel.WREADY = true;
			m_niState = NIState::I;

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void NI::sendPacket(const Packet& packet)
{
	dismantlePacket(packet);
	logDebug(" NI: packet sent ");
	viewPacket(packet);
}

void NI::dismantlePacket(const Packet& packet)
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

void NI::sendFlit()
{
	if (!m_sourceQueue.empty()
		&& m_port.m_outFlitRegister.size() < REGISTER_DEPTH)
	{
		m_port.m_outFlitRegister.push_back(m_sourceQueue.front());
		m_sourceQueue.pop_front();
	}
}

