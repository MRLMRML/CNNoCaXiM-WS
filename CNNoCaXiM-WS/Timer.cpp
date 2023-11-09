#include "Timer.h"

void Timer::recordInputReadTime()
{
	std::ofstream writeInputReadTime(g_recordFolderPath + g_timingRecordPath, std::ios::app); // append
	writeInputReadTime << m_localClock->s_globalClock + 1 << "," << std::endl;
	writeInputReadTime.close();
}

void Timer::recordOutputWrittenTime()
{
	std::ofstream writeOutputWrittenTime(g_recordFolderPath + g_timingRecordPath, std::ios::app); // append
	writeOutputWrittenTime << m_localClock->s_globalClock + 1 << "," << std::endl;
	writeOutputWrittenTime.close();
}

void Timer::recordPacketTimeInitializeStart(const int seqID)
{
	std::ofstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::app);
	writePacketTime << seqID << "," << m_localClock->s_globalClock + 1 << "," << std::endl;
	writePacketTime.close();
}

void Timer::recordPacketTimeInitializeFinish(const int seqID)
{
	std::ofstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::app);
	writePacketTime << seqID << "," << m_localClock->s_globalClock + 2 << "," << std::endl; // + 2 due to the execution clock feature
	writePacketTime.close();
}

void Timer::recordPacketTimeAppendStart(const int seqID)
{
	std::ifstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::in);
	std::string t_packetTime{ g_recordFolderPath + "t_timingRecord.csv" };
	std::ofstream t_writePacketTime(t_packetTime); // tmp file
	std::string line{};
	std::string seqid{};
	std::istringstream lineInString{};
	std::getline(writePacketTime, line); // read the first line
	t_writePacketTime << line << std::endl; // write the first line into the tmp file
	while (std::getline(writePacketTime, line))
	{
		lineInString.str(line);
		std::getline(lineInString, seqid, ',');
		if (seqid == std::to_string(seqID))
			t_writePacketTime << line << m_localClock->s_globalClock + 1 << "," << std::endl;
		else
			t_writePacketTime << line << std::endl;
	}

	writePacketTime.close();
	t_writePacketTime.close();
	std::remove((g_recordFolderPath + g_timingRecordPath).c_str()); // delete the old file
	std::rename(t_packetTime.c_str(), (g_recordFolderPath + g_timingRecordPath).c_str()); // rename the tmp file to the old one
}

void Timer::recordPacketTimeAppendFinish(const int seqID)
{
	std::ifstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::in);
	std::string t_packetTime{ g_recordFolderPath + "t_timingRecord.csv" };
	std::ofstream t_writePacketTime(t_packetTime); // tmp file
	std::string line{};
	std::string seqid{};
	std::istringstream lineInString{};
	std::getline(writePacketTime, line); // read the first line
	t_writePacketTime << line << std::endl; // write the first line into the tmp file
	while (std::getline(writePacketTime, line))
	{
		lineInString.str(line);
		std::getline(lineInString, seqid, ',');
		if (seqid == std::to_string(seqID))
			t_writePacketTime << line << m_localClock->s_globalClock + 2 << "," << std::endl; // + 2 due to the execution clock feature
		else
			t_writePacketTime << line << std::endl;
	}

	writePacketTime.close();
	t_writePacketTime.close();
	std::remove((g_recordFolderPath + g_timingRecordPath).c_str()); // delete the old file
	std::rename(t_packetTime.c_str(), (g_recordFolderPath + g_timingRecordPath).c_str()); // rename the tmp file to the old one
}
