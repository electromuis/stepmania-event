#pragma once

#include "stdint.h"
#include <string>

using namespace std;

namespace adp {

enum BoardType {
	BOARD_UNKNOWN,
	BOARD_FSRMINIPAD,
	BOARD_TEENSY2,
	BOARD_LEONARDO
};

enum BoardType ParseBoardType(const std::string& str);
wstring BoardTypeToString(BoardType boardType);

}; // namespace adp.
