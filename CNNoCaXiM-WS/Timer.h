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
	void recordInputReadTime();
	void recordPacketTimeInitializeStart(const int seqID);
	void recordPacketTimeInitializeFinish(const int seqID);
	void recordPacketTimeAppendStart(const int seqID);
	void recordPacketTimeAppendFinish(const int seqID);
	void recordOutputWrittenTime();

	std::unique_ptr<Clock> m_localClock{ std::make_unique<Clock>() };

private:

};