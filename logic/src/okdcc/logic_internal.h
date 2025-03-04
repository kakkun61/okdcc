#ifndef DCC_LOGIC_INTERNAL_H
#define DCC_LOGIC_INTERNAL_H

#include "logic.h"

enum dcc_Result dcc_decodeSignal(dcc_TimeMicroSec const period1, dcc_TimeMicroSec const period2, dcc_Bit *const bit);

enum dcc_Result dcc_validatePacket(uint8_t const *const bytes, size_t bytesSize, uint8_t const checksum);

#endif
