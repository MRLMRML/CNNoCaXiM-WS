#include "PE.h"

void PE::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		switch (m_peState)
		{
		case PEState::W:
			receiveReadWeightResponse();
			break;
		case PEState::I:
			receiveReadInputResponse();
			break;
		}
	}
}

void PE::receiveReadWeightResponse()
{
	if (m_masterInterface.readDataChannel.RREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_PE_WI - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			m_weightData = m_masterInterface.readDataChannel.RDATA;
			m_masterInterface.readDataChannel.RREADY = true;
			m_peState = PEState::I;
			logDebug(" PE: read response for weight data received ");

			m_localClock->tickTriggerClock(1);
			m_localClock->tickExecutionClock(1);
			m_localClock->toggleWaitingForExecution();
		}
	}
}

void PE::receiveReadInputResponse()
{
	if (m_masterInterface.readDataChannel.RREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			//m_localClock->tickExecutionClock(EXECUTION_TIME_PE_II - 1);
			m_localClock->tickExecutionClock(m_weightData.size() - 1 - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			m_inputData = m_masterInterface.readDataChannel.RDATA;
			m_RID = m_masterInterface.readDataChannel.RID;
			m_masterInterface.readDataChannel.RREADY = true;
			m_peState = PEState::I;
			logDebug(" PE: read response for input data received ");
			processData();
			sendWriteOutputRequest();

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

void PE::sendWriteOutputRequest()
{
	m_masterInterface.writeAddressChannel.AWVALID = true;
	m_masterInterface.writeAddressChannel.AWID = m_RID;
	m_masterInterface.writeAddressChannel.AWADDR = m_RID;

	m_masterInterface.writeDataChannel.WVALID = true;
	m_masterInterface.writeDataChannel.WDATA = m_outputData;

	logDebug(" PE: write request for output data sent ");
}
