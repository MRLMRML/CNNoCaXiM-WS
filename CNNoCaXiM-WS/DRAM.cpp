#include "DRAM.h"

void DRAM::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		switch (m_dramState)
		{
		case DRAMState::W:
			receiveReadRequest();
			break;
		case DRAMState::I:
			receiveReadRequest();
			break;
		case DRAMState::O:
			receiveWriteRequest();
			break;
		}
	}
}

void DRAM::receiveReadRequest()
{
	if (m_slaveInterface.readAddressChannel.ARREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			if (m_dramState == DRAMState::W)
				m_localClock->tickExecutionClock(EXECUTION_TIME_DRAM_WI - 1);
			else if (m_dramState == DRAMState::I)
				m_localClock->tickExecutionClock(EXECUTION_TIME_DRAM_IO - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			std::vector<DATA_PRECISION> t_RDATA{};
			if (m_slaveInterface.readAddressChannel.ARADDR >= WEIGHT_DATA_START_LOCATION
				&& m_slaveInterface.readAddressChannel.ARADDR < WEIGHT_DATA_START_LOCATION + WEIGHT_DATA_LINE_COUNT)
			{
				std::ifstream readWeightData(g_dataFolderPath + g_weightDataPath, std::ios::in);
				std::string line{};
				std::istringstream lineInString{};
				std::string data{};
				for (int i{}; i < m_slaveInterface.readAddressChannel.ARADDR - WEIGHT_DATA_START_LOCATION + 1; ++i)
					std::getline(readWeightData, line);
				lineInString.str(line);
				while (std::getline(lineInString, data, ','))
					t_RDATA.push_back(std::stoi(data));
				readWeightData.close();
				m_slaveInterface.readDataChannel.RDATA = t_RDATA;
				m_slaveInterface.readAddressChannel.ARREADY = true;
				logDebug(" DRAM: read request for weight data received ");
			}
			else if (m_slaveInterface.readAddressChannel.ARADDR >= INPUT_DATA_START_LOCATION
				&& m_slaveInterface.readAddressChannel.ARADDR < INPUT_DATA_START_LOCATION + INPUT_DATA_LINE_COUNT)
			{
				std::ifstream readInputData(g_dataFolderPath + g_inputDataPath, std::ios::in);
				std::string line{};
				std::istringstream lineInString{};
				std::string data{};
				for (int i{}; i < m_slaveInterface.readAddressChannel.ARADDR - INPUT_DATA_START_LOCATION + 1; ++i)
					std::getline(readInputData, line);
				lineInString.str(line);
				while (std::getline(lineInString, data, ','))
					t_RDATA.push_back(std::stoi(data));
				readInputData.close();
				m_slaveInterface.readDataChannel.RDATA = t_RDATA;
				m_slaveInterface.readAddressChannel.ARREADY = true;
				logDebug(" DRAM: read request for input data received ");
				m_timer->recordInputReadTime();
			}
			else
				throw std::out_of_range{ " DRAM: read request out of range " };

			sendReadResponse();
		}
	}
}

void DRAM::sendReadResponse()
{
	m_slaveInterface.readDataChannel.RVALID = true;
	m_slaveInterface.readDataChannel.RID = m_slaveInterface.readAddressChannel.ARID;
	logDebug(" DRAM: read response sent ");
	if (m_dramState == DRAMState::W)
	{
		m_dramState = DRAMState::I;

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
	else if (m_dramState == DRAMState::I)
	{
		m_dramState = DRAMState::O;

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void DRAM::receiveWriteRequest()
{
	if (m_slaveInterface.writeAddressChannel.AWREADY == false && m_slaveInterface.writeDataChannel.WREADY == false)
	{
		if (!m_localClock->isWaitingForExecution())
		{
			m_localClock->tickExecutionClock(EXECUTION_TIME_DRAM_OI - 1);
			m_localClock->toggleWaitingForExecution();
		}

		if (m_localClock->executeLocalEvent())
		{
			if (m_slaveInterface.writeAddressChannel.AWADDR >= OUTPUT_DATA_START_LOCATION)
			{
				std::ofstream writeOutputData(g_dataFolderPath + g_outputDataPath, std::ios::app); // append
				for (auto& data : m_slaveInterface.writeDataChannel.WDATA)
					writeOutputData << std::to_string(data) << ",";
				writeOutputData << std::endl;
				writeOutputData.close();
				m_slaveInterface.writeAddressChannel.AWREADY = true;
				m_slaveInterface.writeDataChannel.WREADY = true;
				logDebug(" DRAM: write request received ");
				m_timer->recordOutputWrittenTime();
			}
			else
				throw std::out_of_range{ " DRAM: write request out of range " };
		
			sendWriteResponse();
		}
	}
}

void DRAM::sendWriteResponse()
{
	m_slaveInterface.writeResponseChannel.BVALID = true;
	m_slaveInterface.writeResponseChannel.BID = m_slaveInterface.writeAddressChannel.AWID;
	logDebug(" DRAM: write response sent ");
	m_dramState = DRAMState::I;

	m_localClock->tickTriggerClock(1);
	m_localClock->tickExecutionClock(1);
	m_localClock->toggleWaitingForExecution();
}
