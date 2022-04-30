#include "Reader.h"
#include <cstring>
#include "libusb-1.0/libusb.h"
#include <iostream>

using namespace std;

#define CHECK_ERROR_RET(statement)                          \
    {                                                       \
        int e = statement;                                  \
        if(e != 0) {                                        \
            cout << "USB Err (" << to_string(__LINE__ ) << "): " << to_string(e) << endl;    \
            return false;                                   \
        }                                                   \
    }

PMReader::PMReader()
{
    int e = libusb_init(&ctx);
    if(e < 0) {
        cout << "USB Err: " << to_string(e) << endl;
        return;
    }
    else {
        libusb_set_debug(ctx, 3);
    }
}

bool PMReader::IsConnected()
{
	if (!device)
	{
        libusb_device **devs;
        ssize_t cnt = libusb_get_device_list(ctx, &devs);

        if(cnt == 0)
            return false;

        for(size_t i=0; i<cnt; ++i)
        {
            libusb_device *c_device = devs[i];
            libusb_device_descriptor c_desc;

            {
                int e = libusb_get_device_descriptor(c_device, &c_desc);
                if(e != 0) {
                    cout << "USB Err: " << to_string(e) << endl;
                    break;
                }
            }

            printf("Vendor:Device = %04x:%04x\n", c_desc.idVendor, c_desc.idProduct);

            if(c_desc.idVendor == 0x08ff && c_desc.idProduct == 0x0009)
            {
                device = c_device;
                break;
            }
        }

        //libusb_free_device_list(devs, 1);

		if (!device)
			return false;

        CHECK_ERROR_RET(libusb_open(device, &device_handle))

#ifndef _WIN32
        CHECK_ERROR_RET(libusb_set_auto_detach_kernel_driver(device_handle, 1))
#endif

		CHECK_ERROR_RET(libusb_claim_interface(device_handle, 0))
	}

	return true;
}

PMReader::~PMReader()
{
	libusb_release_interface(device_handle, 0);
	libusb_close(device_handle);
	libusb_exit(ctx);
}

string PMReader::Read()
{
	if (!IsConnected())
		return "";

	unsigned char data[200] = "";
	int actual_length;
	int r;

	r = libusb_interrupt_transfer(device_handle, 0x81, data, 200, &actual_length, 1500);

	if(r < 0 && r != LIBUSB_ERROR_TIMEOUT)
	{
        cout << "USB Err (" << to_string(__LINE__ ) << "): " << to_string(r) << endl;

        if(r == LIBUSB_ERROR_NO_DEVICE)
        {
            // Release device variable, which will make it try to reconnect
            libusb_release_interface(device_handle, 0);
            libusb_close(device_handle);
            device = nullptr;
            device_handle = nullptr;
        }

        return "";
	}


	if (actual_length > 0)
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
