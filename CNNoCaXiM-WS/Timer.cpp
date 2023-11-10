#include "Timer.h"

void Timer::recordStartTime()
{
#if defined (TIMER)
	std::ofstream writeStartTime(g_recordFolderPath + g_timingRecordPath, std::ios::app); // append
	writeStartTime << m_localClock->s_globalClock + 1 << "," << std::endl;
	writeStartTime.close();
#endif
}

void Timer::recordFinishTime()
{
#if defined (TIMER)
	std::ofstream writeFinishTime(g_recordFolderPath + g_timingRecordPath, std::ios::app); // append
	writeFinishTime << m_localClock->s_globalClock + 2 << "," << std::endl; // + 2 due to the execution clock feature
	writeFinishTime.close();
#endif
}

void Timer::recordPacketTimeInitializeStart(const int seqID)
{
#if defined (TIMER_DETAILED)
	std::ofstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::app);
	writePacketTime << seqID << "," << m_localClock->s_globalClock + 1 << "," << std::endl;
	writePacketTime.close();
#endif
}

void Timer::recordPacketTimeInitializeFinish(const int seqID)
{
#if defined (TIMER_DETAILED)
	std::ofstream writePacketTime(g_recordFolderPath + g_timingRecordPath, std::ios::app);
	writePacketTime << seqID << "," << m_localClock->s_globalClock + 2 << "," << std::endl; // + 2 due to the execution clock feature
	writePacketTime.close();
#endif
}

void Timer::recordPacketTimeAppendStart(const int seqID)
{
#if defined (TIMER_DETAILED)
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
#endif
}

void Timer::recordPacketTimeAppendFinish(const int seqID)
{
#if defined (TIMER_DETAILED)
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
#endif
}
