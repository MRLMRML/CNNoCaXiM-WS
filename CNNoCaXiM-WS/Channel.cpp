#include "Channel.h"
#include "Log.h"

void Channel::setUpConnection(MasterInterface& masterInterface,
	SlaveInterface& slaveInterface)
{
	m_masterInterfacePointer = &masterInterface;
	m_slaveInterfacePointer = &slaveInterface;
	logDebug(" Channel: connection set up ");
}

void Channel::terminateConnection()
{
	m_masterInterfacePointer = nullptr;
	m_slaveInterfacePointer = nullptr;
	logDebug(" Channel: connection terminated ");
}

void Channel::resetHandshakes()
{
	m_masterInterfacePointer->readAddressChannel.ARVALID = false;
	m_masterInterfacePointer->readAddressChannel.ARREADY = true;
	m_masterInterfacePointer->readDataChannel.RVALID = false;
	m_masterInterfacePointer->readDataChannel.RREADY = true;
	m_masterInterfacePointer->writeAddressChannel.AWVALID = false;
	m_masterInterfacePointer->writeAddressChannel.AWREADY = true;
	m_masterInterfacePointer->writeDataChannel.WVALID = false;
	m_masterInterfacePointer->writeDataChannel.WREADY = true;
	m_masterInterfacePointer->writeResponseChannel.BVALID = false;
	m_masterInterfacePointer->writeResponseChannel.BREADY = true;

	m_slaveInterfacePointer->readAddressChannel.ARVALID = false;
	m_slaveInterfacePointer->readAddressChannel.ARREADY = true;
	m_slaveInterfacePointer->readDataChannel.RVALID = false;
	m_slaveInterfacePointer->readDataChannel.RREADY = true;
	m_slaveInterfacePointer->writeAddressChannel.AWVALID = false;
	m_slaveInterfacePointer->writeAddressChannel.AWREADY = true;
	m_slaveInterfacePointer->writeDataChannel.WVALID = false;
	m_slaveInterfacePointer->writeDataChannel.WREADY = true;
	m_slaveInterfacePointer->writeResponseChannel.BVALID = false;
	m_slaveInterfacePointer->writeResponseChannel.BREADY = true;
}

void Channel::runOneStep()
{
	if (m_localClock->triggerLocalEvent())
	{
		if (m_masterInterfacePointer->readAddressChannel.ARVALID == true &&
			m_slaveInterfacePointer->readAddressChannel.ARREADY == true)
			transferReadAddress();

		if (m_slaveInterfacePointer->readDataChannel.RVALID == true &&
			m_masterInterfacePointer->readDataChannel.RREADY == true)
			transferReadData();

		if (m_masterInterfacePointer->writeAddressChannel.AWVALID == true &&
			m_slaveInterfacePointer->writeAddressChannel.AWREADY == true)
			transferWriteAddress();

		if (m_masterInterfacePointer->writeDataChannel.WVALID == true &&
			m_slaveInterfacePointer->writeDataChannel.WREADY == true)
			transferWriteData();

		if (m_slaveInterfacePointer->writeResponseChannel.BVALID == true &&
			m_masterInterfacePointer->writeResponseChannel.BREADY == true)
			transferWriteResponse();
	}
}

void Channel::transferReadAddress()
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(1 - 1);
		m_localClock->toggleWaitingForExecution();
	}

	if (m_localClock->executeLocalEvent())
	{
		m_slaveInterfacePointer->readAddressChannel.ARID = m_masterInterfacePointer->readAddressChannel.ARID;
		m_slaveInterfacePointer->readAddressChannel.ARADDR = m_masterInterfacePointer->readAddressChannel.ARADDR;
		m_masterInterfacePointer->readAddressChannel.ARVALID = false;
		m_slaveInterfacePointer->readAddressChannel.ARREADY = false;
		logDebug(" Channel: read address transferred ");

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void Channel::transferReadData()
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(m_slaveInterfacePointer->readDataChannel.RDATA.size() - 1);
		m_localClock->toggleWaitingForExecution();
	}

	if (m_localClock->executeLocalEvent())
	{
		m_masterInterfacePointer->readDataChannel.RID = m_slaveInterfacePointer->readDataChannel.RID;
		m_masterInterfacePointer->readDataChannel.RDATA = m_slaveInterfacePointer->readDataChannel.RDATA;
		m_slaveInterfacePointer->readDataChannel.RVALID = false;
		m_masterInterfacePointer->readDataChannel.RREADY = false;
		logDebug(" Channel: read data transferred ");

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void Channel::transferWriteAddress()
{
	m_slaveInterfacePointer->writeAddressChannel.AWID = m_masterInterfacePointer->writeAddressChannel.AWID;
	m_slaveInterfacePointer->writeAddressChannel.AWADDR = m_masterInterfacePointer->writeAddressChannel.AWADDR;
	m_masterInterfacePointer->writeAddressChannel.AWVALID = false;
	m_slaveInterfacePointer->writeAddressChannel.AWREADY = false;
	logDebug(" Channel: write address transferred ");
}

void Channel::transferWriteData()
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(m_masterInterfacePointer->writeDataChannel.WDATA.size() - 1);
		m_localClock->toggleWaitingForExecution();
	}

	if (m_localClock->executeLocalEvent())
	{
		m_slaveInterfacePointer->writeDataChannel.WDATA = m_masterInterfacePointer->writeDataChannel.WDATA;
		m_masterInterfacePointer->writeDataChannel.WVALID = false;
		m_slaveInterfacePointer->writeDataChannel.WREADY = false;
		logDebug(" Channel: write data transferred ");

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

void Channel::transferWriteResponse()
{
	if (!m_localClock->isWaitingForExecution())
	{
		m_localClock->tickExecutionClock(1 - 1);
		m_localClock->toggleWaitingForExecution();
	}

	if (m_localClock->executeLocalEvent())
	{
		m_masterInterfacePointer->writeResponseChannel.BID = m_slaveInterfacePointer->writeResponseChannel.BID;
		m_slaveInterfacePointer->writeResponseChannel.BVALID = false;
		m_masterInterfacePointer->writeResponseChannel.BREADY = false;
		logDebug(" Channel: write response transferred ");

		m_localClock->tickTriggerClock(1);
		m_localClock->tickExecutionClock(1);
		m_localClock->toggleWaitingForExecution();
	}
}

