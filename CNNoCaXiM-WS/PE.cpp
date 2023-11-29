#include "PE.h"

void PE::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		switch (m_peState)
		{
		case PEState::W:
			receiveWriteRequest();
			break;
		case PEState::I:
			receiveReadResponse();
			break;
		}
	}
}

void PE::receiveWriteRequest()
{
	if (m_slaveInterface.writeAddressChannel.AWREADY == false &&
		m_slaveInterface.writeDataChannel.WREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_PE_WI - 1);
			m_localClock->toggleWaitingForExecution();

			m_timer->recordPacketTimeAppendStart(m_slaveInterface.writeAddressChannel.AWID); // this is SEQID of the packet
		}

		if (m_localClock->executeLocalEvent())
		{
			m_weightData = m_slaveInterface.writeDataChannel.WDATA;
			m_slaveInterface.writeAddressChannel.AWREADY = true;
			m_slaveInterface.writeDataChannel.WREADY = true;
			logDebug(" PE: write request for weight data received ");

			sendWriteResponse();

			m_peState = PEState::I;

			m_timer->recordPacketTimeAppendFinish(m_slaveInterface.writeAddressChannel.AWID); // this RID is SEQID of the packet, while the real RID is stored in the NI

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void PE::sendWriteResponse()
{
	m_slaveInterface.writeResponseChannel.BVALID = true;
	m_slaveInterface.writeResponseChannel.BID = m_slaveInterface.writeAddressChannel.AWID; // should be -1
	logDebug(" PE: write response for weight data sent ");
}

void PE::receiveReadResponse()
{
	if (m_masterInterface.readDataChannel.RREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			//m_localClock->tickExecutionClock(EXECUTION_TIME_PE_II - 1);
			m_localClock->tickExecutionClock(m_weightData.size() - 1); // activation function also takes one cycle
			m_localClock->toggleWaitingForExecution();

			m_timer->recordPacketTimeAppendStart(m_masterInterface.readDataChannel.RID); // this is SEQID of the packet
		}

		if (m_localClock->executeLocalEvent())
		{
			m_inputData = m_masterInterface.readDataChannel.RDATA;
			m_masterInterface.readDataChannel.RREADY = true;
			m_peState = PEState::I;
			logDebug(" PE: read response for input data received ");
			processData();
			sendWriteRequest();

			m_timer->recordPacketTimeAppendFinish(m_masterInterface.readDataChannel.RID); // this RID is SEQID of the packet, while the real RID is stored in the NI

			m_outputData.clear();

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void PE::processData()
{
	DATA_PRECISION output{ std::inner_product(m_inputData.begin(), m_inputData.end(), m_weightData.begin(), m_weightData.back()) };
	m_outputData.push_back(m_activationFunction(output));
}

void PE::sendWriteRequest()
{
	m_masterInterface.writeAddressChannel.AWVALID = true;
	m_masterInterface.writeAddressChannel.AWID = m_masterInterface.readDataChannel.RID; // this is SEQID of the packet
	m_masterInterface.writeAddressChannel.AWADDR = m_masterInterface.readDataChannel.RID; // does not matter

	m_masterInterface.writeDataChannel.WVALID = true;
	m_masterInterface.writeDataChannel.WDATA = m_outputData;

	logDebug(" PE: write request for output data sent ");
}
