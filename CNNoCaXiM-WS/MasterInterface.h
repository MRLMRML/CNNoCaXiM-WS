#pragma once
#include "Subchannels.h"

struct MasterInterface
{
	ReadAddressChannel readAddressChannel{};
	ReadDataChannel readDataChannel{};
	WriteAddressChannel writeAddressChannel{};
	WriteDataChannel writeDataChannel{};
	WriteResponseChannel writeResponseChannel{};
};

