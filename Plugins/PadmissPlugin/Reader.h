#pragma once

#include <string>

struct libusb_device_handle;

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
