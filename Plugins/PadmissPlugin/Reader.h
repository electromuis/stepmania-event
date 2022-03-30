#pragma once

#include <stdio.h>
#include "libusb.h"

#include <mutex>

class PMReader
{
public:
	PMReader();
	~PMReader();
	bool IsConnected();

	std::string Read();

protected:
	int charCount = 0;
	char buffer[16] = "";
	libusb_device_handle* device = nullptr;
};
