#pragma once
#include "DataStructures.h"
#include "Clock.h"
#include <fstream>
#include <sstream>
#include <cstdio>

extern std::string g_recordFolderPath;
extern std::string g_timingRecordPath;

class Timer
{
public:
	void recordStartTime();
	void recordFinishTime();
	void recordPacketTimeInitializeStart(const int seqID);
	void recordPacketTimeInitializeFinish(const int seqID);
	void recordPacketTimeAppendStart(const int seqID);
	void recordPacketTimeAppendFinish(const int seqID);

	std::unique_ptr<Clock> m_localClock{ std::make_unique<Clock>() };

private:

};