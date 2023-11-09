#pragma once
#include "DataStructures.h"
#include "ActivationFunctions.h"
#include "MasterInterface.h"
#include "Log.h"
#include "Clock.h"
#include <numeric> // std::inner_product
#include <stdexcept> // std::runtime_error

class PE
{
public:
	PE() = default;
	PE(DATA_PRECISION(*activationFunction)(DATA_PRECISION&)) : m_activationFunction{ activationFunction } {};

	void runOneStep();

	void receiveReadWeightResponse();
	void receiveReadInputResponse();

	void processData();

	void sendWriteOutputRequest();

public:
	PEState m_peState{};
	MasterInterface m_masterInterface{ };
	std::vector<DATA_PRECISION> m_inputData{};
	std::vector<DATA_PRECISION> m_weightData{};
	std::vector<DATA_PRECISION> m_outputData{};
	int m_RID{ -1 }; // hold the RID of input packet and use it in output packet
	std::shared_ptr<Clock> m_localClock{ nullptr };

private:
	DATA_PRECISION(*m_activationFunction)(DATA_PRECISION&) { relu }; // activation function used for this PE/group of neurons
};