#pragma once
#include "DataStructures.h"
#include "ActivationFunctions.h"
#include "MasterInterface.h"
#include "SlaveInterface.h"
#include "Log.h"
#include "Clock.h"
#include "Timer.h"
#include <numeric> // std::inner_product
#include <stdexcept> // std::runtime_error

class PE
{
public:
	PE() = default;
	PE(DATA_PRECISION(*activationFunction)(DATA_PRECISION&)) : m_activationFunction{ activationFunction } {};

	void runOneStep();

	void receiveWriteRequest();
	void sendWriteResponse();

	void receiveReadResponse();
	void processData();
	void sendWriteRequest();

public:
	PEState m_peState{};
	MasterInterface m_masterInterface{ };
	SlaveInterface m_slaveInterface{ };
	std::vector<DATA_PRECISION> m_inputData{};
	std::vector<DATA_PRECISION> m_weightData{};
	std::vector<DATA_PRECISION> m_outputData{};
	std::shared_ptr<Clock> m_localClock{ nullptr };
	std::unique_ptr<Timer> m_timer{ std::make_unique<Timer>() };

private:
	DATA_PRECISION(*m_activationFunction)(DATA_PRECISION&) { relu }; // activation function used for this PE/group of neurons
};