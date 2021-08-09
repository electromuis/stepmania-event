#include "Adp.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>

#include "Model/Firmware.h"
#include "Model/Device.h"
#include "Model/Log.h"

namespace adp {

BoardType ParseBoardType(const std::string& str)
{
	if (str == "fsrminipad") { return BOARD_FSRMINIPAD; }
	if (str == "teensy2") { return BOARD_TEENSY2; }
	if (str == "leonardo") { return BOARD_LEONARDO; }
	else { return BOARD_UNKNOWN; }
}

wstring BoardTypeToString(BoardType boardType)
{
	if (boardType == BOARD_FSRMINIPAD) {
		return L"FSR Mini pad";
	}

	if (boardType == BOARD_TEENSY2) {
		return L"Teensy 2";
	}

	if (boardType == BOARD_LEONARDO) {
		return L"Arduino leonardo/pro micro";
	}

	return L"Unknown";
}

}; // namespace adp.