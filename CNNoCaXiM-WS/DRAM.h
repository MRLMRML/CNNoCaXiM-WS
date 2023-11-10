#pragma once
#include "DataStructures.h"
#include "SlaveInterface.h"
#include "Log.h"
#include "Clock.h"
#include "Timer.h"
#include <fstream>
#include <sstream>
#include <cstdio>

extern std::string g_dataFolderPath;
extern std::string g_weightDataPath;
extern std::string g_inputDataPath;
extern std::string g_outputDataPath;

class DRAM
{
public:
	DRAM() = default;
	void runOneStep();

	// With slave interface
	// DRAM(inputs) read by PE
	void receiveReadRequest();
	void sendReadResponse();

	// DRAM(outputs) written by PE
	void receiveWriteRequest();
	void sendWriteResponse();

public:
	DRAMState m_dramState{};
	SlaveInterface m_slaveInterface{ };
	std::shared_ptr<Clock> m_localClock{ nullptr };
	std::unique_ptr<Timer> m_timer{ std::make_unique<Timer>() };

private:
};
