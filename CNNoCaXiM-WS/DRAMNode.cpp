#include "DRAMNode.h"

void DRAMNode::runOneStep()
{
	m_controller->runOneStep();
	m_slaveChannel->runOneStep();
	m_DRAM->runOneStep();
	m_localClock->synchronizeExecutionClock();
	m_localClock->synchronizeTriggerClock();
}

void DRAMNode::updateEnable()
{
	m_controller->m_port.updateEnable();
}