#pragma once

#include <string>

struct libusb_device_handle;
struct libusb_context;
struct libusb_device;

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
	libusb_device_handle* device_handle = nullptr;
	libusb_device* device = nullptr;
	libusb_context* ctx = nullptr;
};
