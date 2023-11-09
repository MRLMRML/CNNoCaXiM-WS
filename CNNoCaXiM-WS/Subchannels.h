#pragma once
#include "Parameters.h"
#include <vector>

struct ReadAddressChannel
{
	int ARID{}; // Master is the source
	int ARADDR{}; // Master is the source
	bool ARVALID{ false }; // Master is the source
	bool ARREADY{ true }; // Slave is the source
};

struct ReadDataChannel
{
	int RID{}; // Slave is the source
	std::vector<DATA_PRECISION> RDATA{}; // Slave is the source
	bool RRESP{ true }; // Slave is the source; 0(0b00) OKAY
	bool RVALID{ false }; // Slave is the source
	bool RREADY{ true }; // Master is the source
};

struct WriteAddressChannel
{
	int AWID{}; // Master is the source
	int AWADDR{}; // Master is the source
	bool AWVALID{ false }; // Master is the source
	bool AWREADY{ true }; // Slave is the source
};

struct WriteDataChannel
{
	std::vector<DATA_PRECISION> WDATA{}; // Master is the source
	bool WVALID{ false }; // Master is the source
	bool WREADY{ true }; // Slave is the source
};

struct WriteResponseChannel
{
	int BID{}; // Slave is the source
	//bool BRESP{ true }; // Slave is the source; 0(0b00) OKAY
	bool BVALID{ false }; // Slave is the source
	bool BREADY{ true }; // Master is the source
};
