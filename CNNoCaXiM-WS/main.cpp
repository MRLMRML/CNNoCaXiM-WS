#include "Network.h"
#include "DRAMNode.h"
#include "PENode.h"
//#include "FakeNoC.h"

// global variables
std::string g_dataFolderPath{ "C:\\Users\\Hubiao\\source\\repos\\CNNoCaXiM-WS\\CNNoCaXiM-WS\\Data\\" };
std::string g_weightDataPath{ "WeightData.csv" };
std::string g_inputDataPath{ "InputData.csv" };
std::string g_outputDataPath{ "OutputData.csv" };

std::string g_recordFolderPath{ "C:\\Users\\Hubiao\\source\\repos\\CNNoCaXiM-WS\\CNNoCaXiM-WS\\Record\\" };
std::string g_timingRecordPath{ "TimingRecord.csv" };

int main()
{
	// timing
	Clock globalClock{};

	// create network
	Network* network{ new Network{} };
	network->setUpConnections();
	network->printRouterIDs();

	// create PE nodes
	PENode* peNode{ new PENode{1, relu} };

	// create DRAM node
	DRAMNode* dramNode{ new DRAMNode{0, 1} };

	// mount nodes on the network
	network->mountNode({ 0, 0 }, dramNode);
	network->mountNode({ 0, 1 }, peNode);

	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

	//start simulation
	for (int i{}; i < 1000; ++i)
	{
		network->updateEnable();
		dramNode->updateEnable();
		peNode->updateEnable();

		dramNode->runOneStep();
		peNode->runOneStep();
		network->runOneStep();
		globalClock.tickGlobalClock();
	}

	// sanitation
	delete peNode;
	delete dramNode;
	delete network;
}