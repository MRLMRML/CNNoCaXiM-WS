#pragma once
#include "Subchannels.h"

struct SlaveInterface
{
	ReadAddressChannel readAddressChannel{};
	ReadDataChannel readDataChannel{};
	WriteAddressChannel writeAddressChannel{};
	WriteDataChannel writeDataChannel{};
	WriteResponseChannel writeResponseChannel{};
};
