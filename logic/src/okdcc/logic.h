#ifndef DCC_H
#define DCC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define BYTES_CAPACITY 8

typedef unsigned long dcc_TimeMicroSec;

typedef bool dcc_Bit;

enum dcc_Result {
  dcc_Failure = 0,
  dcc_Success = 1,
};

enum dcc_StreamParserResult {
  dcc_StreamParserResult_Failure = 0,
  dcc_StreamParserResult_Continue = 1,
  dcc_StreamParserResult_Success = 2,
};

struct dcc_SignalBuffer {
  dcc_TimeMicroSec *const head;
  dcc_TimeMicroSec *const last;
  dcc_TimeMicroSec *writeAt;
  dcc_TimeMicroSec *readAt;
};

typedef uint16_t dcc_Address;

// 下位7ビットしか使用しない
typedef uint8_t dcc_ConsistAddress;

enum dcc_Direction {
  dcc_Backward = 0,
  dcc_Forward = 1,
};

// 下位5ビットしか使用しない
typedef uint8_t dcc_Speed;

struct dcc_SpeedAndDirectionPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  dcc_Speed speed;
};

struct dcc_DecoderResetPacket {
  dcc_Address address;
};

struct dcc_HardResetPacket {
  dcc_Address address;
};

struct dcc_FactoryTestInstructionPacket {
  dcc_Address address;
  uint8_t data[2];
  size_t dataSize;
};

struct dcc_DecoderFlagsSetPacket {
  // 未実装
  void *unimplemented;
};

struct dcc_AdvancedAddressingSetPacket {
  // 未実装
  void *unimplemented;
};

struct dcc_DecoderAcknowledgementRequestPacket {
  dcc_Address address;
};

struct dcc_ConsistControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  dcc_ConsistAddress consistAddress;
};

struct dcc_SpeedStep128ControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  bool emergencyStop;
  dcc_Speed speed;
};

enum dcc_PacketTag {
  dcc_SpeedAndDirectionPacketTag,
  dcc_AllDecoderResetPacketTag,
  dcc_AllDecoderIdlePacketTag,
  dcc_DecoderResetPacketTag,
  dcc_HardResetPacketTag,
  dcc_FactoryTestInstructionPacketTag,
  dcc_DecoderFlagsSetPacketTag,
  dcc_AdvancedAddressingSetPacketTag,
  dcc_DecoderAcknowledgementRequestPacketTag,
  dcc_ConsistControlPacketTag,
  dcc_SpeedStep128ControlPacketTag,
};

struct dcc_Packet {
  enum dcc_PacketTag tag;
  union {
    struct dcc_SpeedAndDirectionPacket speedAndDirectionPacket;
    struct dcc_DecoderResetPacket decoderResetPacket;
    struct dcc_HardResetPacket hardResetPacket;
    struct dcc_FactoryTestInstructionPacket factoryTestInstructionPacket;
    struct dcc_DecoderFlagsSetPacket decoderFlagsSetPacket;
    struct dcc_AdvancedAddressingSetPacket advancedAddressingSetPacket;
    struct dcc_DecoderAcknowledgementRequestPacket decoderAcknowledgementRequestPacket;
    struct dcc_ConsistControlPacket consistControlPacket;
    struct dcc_SpeedStep128ControlPacket speedStep128ControlPacket;
  };
};

struct dcc_SignalStreamParser {
  dcc_TimeMicroSec signals[2];
  size_t signalsSize;
};

enum dcc_BitStreamParserState {
  dcc_BitStreamParserState_InPreamble,
  dcc_BitStreamParserState_InByte,
  dcc_BitStreamParserState_AfterByte,
};

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
  uint8_t bytes[BYTES_CAPACITY];
  size_t bytesSize;
};

struct dcc_Decoder {
  dcc_TimeMicroSec previousSignal;
  struct dcc_SignalBuffer signalBuffer;
  struct dcc_SignalStreamParser signalStreamParser;
  struct dcc_BitStreamParser bitStreamParser;
};

// `1` の半ビットの転送継続時間の最小値
extern dcc_TimeMicroSec const dcc_minOneHalfBitSentPeriod;

// `1` の半ビットの転送継続時間の最大値
extern dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriod;

// 受信した `1` の半ビットの許容時間の最小値
extern dcc_TimeMicroSec const dcc_minOneHalfBitReceivedPeriod;

// 受信した `1` の半ビットの許容時間の最大値
extern dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriod;

// `0` の半ビットの転送継続時間の最小値
extern dcc_TimeMicroSec const dcc_minZeroHalfBitSentPeriod;

// `0` の半ビットの転送継続時間の最大値
extern dcc_TimeMicroSec const dcc_maxZeroHalfBitSentPeriod;

// 引き伸ばされた `0` ビットの総和継続時間の最大値
extern dcc_TimeMicroSec const dcc_maxStretchedZeroBitPeriod;

// 受信した `0` の半ビットの許容時間の最小値
extern dcc_TimeMicroSec const dcc_minZeroHalfBitReceivedPeriod;

// 受信した `0` の半ビットの許容時間の最大値
extern dcc_TimeMicroSec const dcc_maxZeroHalfBitReceivedPeriod;

// 転送した `1` の半ビットの時間の差の最大値
extern dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriodDiff;

// 受信した `1` の半ビットの時間の差の最大値
extern dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriodDiff;

struct dcc_SignalBuffer dcc_initializeSignalBuffer(dcc_TimeMicroSec *array, size_t const size);

enum dcc_Result dcc_writeSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec const signal);

enum dcc_Result dcc_readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal);

enum dcc_Result dcc_decodeSignal(dcc_TimeMicroSec const period1, dcc_TimeMicroSec const period2, dcc_Bit *const bit);

struct dcc_SignalStreamParser dcc_initializeSignalStreamParser(void);

enum dcc_StreamParserResult dcc_feedSignal(struct dcc_SignalStreamParser *const parser, dcc_TimeMicroSec const signal,
                                           dcc_Bit *const bit);

struct dcc_BitStreamParser dcc_initializeBitStreamParser(void);

enum dcc_StreamParserResult dcc_feedBit(struct dcc_BitStreamParser *const parser, dcc_Bit const bit, uint8_t bytes[],
                                        size_t *const bytesSize);

enum dcc_Result dcc_validatePacket(uint8_t const *const bytes, size_t bytesSize, uint8_t const checksum);

enum dcc_Result dcc_parseSpeedAndDirectionPacket(uint8_t const *const bytes, size_t const bytesSize,
                                                 struct dcc_SpeedAndDirectionPacket *const packet);

enum dcc_Result dcc_parseAllDecoderResetPacket(uint8_t const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseAllDecoderIdlePacket(uint8_t const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseDecoderResetPacket(uint8_t const *const bytes, size_t const bytesSize,
                                            struct dcc_DecoderResetPacket *const packet);

enum dcc_Result dcc_parseHardResetPacket(uint8_t const *const bytes, size_t const bytesSize,
                                         struct dcc_HardResetPacket *const packet);

enum dcc_Result dcc_parseDecoderAcknowledgementRequestPacket(
  uint8_t const *const bytes, size_t const bytesSize, struct dcc_DecoderAcknowledgementRequestPacket *const packet);

enum dcc_Result dcc_parseFactoryTestInstructionPacket(uint8_t const *const bytes, size_t const bytesSize,
                                                      struct dcc_FactoryTestInstructionPacket *const packet);

enum dcc_Result dcc_parseConsistControlPacket(uint8_t const *const bytes, size_t const bytesSize,
                                              struct dcc_ConsistControlPacket *const packet);

enum dcc_Result dcc_parseSpeedStep128ControlPacket(uint8_t const *const bytes, size_t const bytesSize,
                                                   struct dcc_SpeedStep128ControlPacket *const packet);

enum dcc_Result dcc_parsePacket(uint8_t const *const bytes, size_t const bytesSize, struct dcc_Packet *const packet);

struct dcc_Decoder dcc_initializeDecoder(dcc_TimeMicroSec *signalBufferValues, size_t const singalBufferSize);

enum dcc_StreamParserResult dcc_decode(struct dcc_Decoder *const decoder, dcc_TimeMicroSec const signal,
                                       struct dcc_Packet *const packet);

int dcc_showSignalBuffer(char *buffer, size_t bufferSize, struct dcc_SignalBuffer const signalBuffer);

int dcc_showBytes(char *buffer, size_t bufferSize, uint8_t const *const bytes, size_t const bytesSize);

int dcc_showDirection(char *buffer, size_t bufferSize, enum dcc_Direction const direction);

int dcc_showSpeedAndDirectionPacket(char *buffer, size_t bufferSize, struct dcc_SpeedAndDirectionPacket const packet);

int dcc_showPacket(char *buffer, size_t bufferSize, struct dcc_Packet const packet);

// エラー時に呼び出される関数を登録する。ログを出力し、終了や再起動もすべし。
extern void (*dcc_error_log)(char const *const file, int const line, char const *format, ...);

#define DCC_ERROR_LOG(...)                                                     \
  {                                                                            \
    if (dcc_error_log != NULL) dcc_error_log(__FILE__, __LINE__, __VA_ARGS__); \
    exit(EXIT_FAILURE);                                                        \
  }

extern int (*dcc_debug_log)(char const *const file, int const line, char const *format, ...);

#define DCC_DEBUG_LOG(...) (dcc_debug_log == NULL ? 0 : dcc_debug_log(__FILE__, __LINE__, __VA_ARGS__))

#define DCC_UNREACHABLE(...) DCC_ERROR_LOG("unreachable: "__VA_ARGS__)

#define DCC_ASSERT(e)                                    \
  {                                                      \
    if (!(e)) DCC_ERROR_LOG("assertion failed: %s", #e); \
  }

#endif
