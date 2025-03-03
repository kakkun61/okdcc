#ifndef DCC_LOGIC_INTERNAL_H
#define DCC_LOGIC_INTERNAL_H

#include "logic.h"

#define DCC_BIT_STREAM_PARSER_BYTES_CAPACITY 8

/// \~english
/// \brief A structure that records the time of voltage changes.
/// \~japanese
/// \brief 電圧変化の時刻を記録する構造体。
struct dcc_SignalBuffer {
  dcc_TimeMicroSec *const buffer;
  size_t const size;
  size_t written;
  size_t writeIndex;
  size_t readIndex;
};

/// \~english
/// \brief A structure that holds the state of the parser that parses the time of voltage changes and gets the bit.
/// \~japanese
/// \brief 電圧変化の時刻の列をパースしビットを取得するパーサーの状態を保持する構造体。
struct dcc_SignalStreamParser {
  dcc_TimeMicroSec signals[2];
  size_t signalsSize;
};

enum dcc_BitStreamParserState {
  dcc_BitStreamParserState_InPreamble,
  dcc_BitStreamParserState_InByte,
  dcc_BitStreamParserState_AfterByte,
};

/// \~english
/// \brief A structure that holds the state of the parser that parses the bit sequence and gets the byte of the data part.
/// \~japanese
/// \brief ビット列をパースしデータ部分のバイトを取得するパーサーの状態を保持する構造体。
struct dcc_BitStreamParser {
  enum dcc_BitStreamParserState state;
  union {
    struct {
      size_t oneBitsCount;
    } inPreamble;
    struct {
      uint8_t byte;
      size_t bitCount;
    } inByte;
  };
  uint8_t bytes[DCC_BIT_STREAM_PARSER_BYTES_CAPACITY];
  size_t bytesSize;
};

/// \~english
/// \brief A structure that holds the state of the decoder.
/// \~japanese
/// \brief デコーダーの状態を保持する構造体。
struct dcc_Decoder {
  struct dcc_SignalBuffer signalBuffer;
  struct dcc_SignalStreamParser signalStreamParser;
  struct dcc_BitStreamParser bitStreamParser;
};

enum dcc_Result dcc_decodeSignal(dcc_TimeMicroSec const period1, dcc_TimeMicroSec const period2, dcc_Bit *const bit);

enum dcc_Result dcc_validatePacket(uint8_t const *const bytes, size_t bytesSize, uint8_t const checksum);

#endif
