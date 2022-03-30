#include "Reader.h"

using namespace std;

PMReader::PMReader()
{

}

bool PMReader::IsConnected()
{
	if (!device)
	{
		int r;

		r = libusb_init(NULL);
		if (r < 0)
			return false;

		libusb_device** list;
		ssize_t cnt = libusb_get_device_list(NULL, &list);

		for (r = 0; r < cnt; r++) {
			libusb_device* device = list[r];
		}

		libusb_free_device_list(list, 1);

		device = libusb_open_device_with_vid_pid(NULL, (uint16_t)0x08ff, (uint16_t)0x0009);
		if (!device)
			return false;

		r = libusb_claim_interface(device, 0);
		if (r < 0)
			return false;
	}

	return true;
}

PMReader::~PMReader()
{
	libusb_release_interface(device, 0);
	libusb_close(device);

	libusb_exit(NULL);
}

string PMReader::Read()
{
	if (!IsConnected())
		return "";

	unsigned char data[200];
	int actual_length;
	int r;

	r = libusb_interrupt_transfer(device, 0x81, data, 200, &actual_length, 1500);

	if (r == 0 || actual_length > 0)
	{
		for (int i = 0; i < actual_length; i += 8 * 2)
		{
			unsigned char key1[8];
			unsigned char key2[8];

			for (int d = 0; d < 8; d++)
			{
				key1[d] = data[i + d];
				key2[d] = data[8 + i + d];
			}

			unsigned char charByte = key1[2];
			if (charByte == 39)
				buffer[charCount] = '0';
			else if (charByte == 40)
			{
				string ret = buffer;
				memset(buffer, 0, sizeof(buffer));
				charCount = 0;

				if (ret.length() == 0)
				{
					bool f = true;
				}

				return ret;
			}	
			else
				buffer[charCount] = '1' + charByte - 0x1e;

			charCount++;

			if (charCount > 15)
			{
				string ret = buffer;
				memset(buffer, 0, sizeof(buffer));
				charCount = 0;

				return ret;
			}
		}
	}

	return "";
}
