#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef DCC_H
#define DCC_H

typedef unsigned long dcc_TimeMicroSec;

typedef uint32_t dcc_Bits32;

#define DCC_BITS32_C(n) UINT32_C(n)

typedef bool dcc_Bit;

#define DCC_BIT_C(n) (n)

enum dcc_Result {
  dcc_Failure = 0,
  dcc_Success = 1,
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
  dcc_Speed speed : 5;
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
};

struct dcc_AdvancedAddressingSetPacket {
  // 未実装
};

struct dcc_DecoderAcknowledgementRequestPacket {
  dcc_Address address;
};

struct dcc_ConsistControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  dcc_ConsistAddress consistAddress : 7;
};

struct dcc_SpeedStep128ControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  bool emergencyStop : 1;
  dcc_Speed speed : 7;
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

dcc_Bit dcc_getBit(dcc_Bits32 const *const bits, size_t const index);

void dcc_setBit(dcc_Bits32 *const bits, size_t const index, dcc_Bit const bit);

struct dcc_SignalBuffer dcc_initializeSignalBuffer(dcc_TimeMicroSec *array, size_t const size);

enum dcc_Result dcc_writeSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec const signal);

void dcc_writesSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec const *const signals,
                            size_t const signalsSize, size_t *const writtenSize);

enum dcc_Result dcc_readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal);

void dcc_readsSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signals,
                           size_t const signalsSize, size_t *const readSize);

enum dcc_Result dcc_decodeSignal(dcc_TimeMicroSec const period1, dcc_TimeMicroSec const period2, dcc_Bit *const bit);

// 線路電圧の変化の時刻列をビット列に変換する。
// `signals` は時刻列を表わす配列へのポインターである。
// `signalsSize` は `signals` の要素数である。
// `readSignalsSize` は読み込まれた `signals` の要素数である
// `bits` は変換したビットを格納する配列へのポインターである。
// `head` は `bits` の先頭のビットのインデックスである（`head` ビットには書き込まれない）。
// `bitsSize` は `bits` のビット数である。
// `writtenBitsSize` は書き込まれた `bits` のビット数である。
// 返り値はエラーなくデコードできたかどうかである。エラーがあった場合はそのビットをスキップしてデコードされる。
enum dcc_Result dcc_decodeSignals(dcc_TimeMicroSec const *const signals, size_t const signalsSize,
                                  size_t *const decodedSingalsSize, dcc_Bits32 *const bits, size_t const head,
                                  size_t const bitsSize, size_t *const writtenBitsSize);

enum dcc_Result dcc_consumeThroughPreamble(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                           size_t *const next);

enum dcc_Result dcc_consumePacketStartBit(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                          size_t *const next);

extern enum dcc_Result (*dcc_consumeAddress)(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                             dcc_Address *const address, size_t *const next);

enum dcc_Result dcc_consumeByte(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                uint8_t *const data, size_t *const next);

enum dcc_Result dcc_consumePacketEndBit(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                        size_t *const next);

enum dcc_Result dcc_consumePacket(dcc_Bits32 const *const bits, size_t const head, size_t const bitsSize,
                                  uint8_t *const bytes, size_t bytesSize, size_t *const writtenPacketSize,
                                  size_t *const next);

bool dcc_validatePacket(uint8_t const *const bytes, size_t bytesSize, uint8_t const checksum);

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

enum dcc_Result parseSpeedStep128ControlPacket(uint8_t const *const bytes, size_t const bytesSize,
                                               struct dcc_SpeedStep128ControlPacket *const packet);

enum dcc_Result dcc_parsePacket(uint8_t const *const bytes, size_t const bytesSize, struct dcc_Packet *const packet);

void shiftBits(dcc_Bits32 *const bits, size_t const bitsSize, int const shift);

int dcc_showSignalBuffer(char *buffer, size_t bufferSize, struct dcc_SignalBuffer const signalBuffer);

int dcc_showBytes(char *buffer, size_t bufferSize, uint8_t const *const bytes, size_t const bytesSize);

int dcc_showDirection(char *buffer, size_t bufferSize, enum dcc_Direction const direction);

int dcc_showSpeedAndDirectionPacket(char *buffer, size_t bufferSize, struct dcc_SpeedAndDirectionPacket const packet);

int dcc_showPacket(char *buffer, size_t bufferSize, struct dcc_Packet const packet);

extern int (*dcc_debug_log)(char const *format, ...);

#define DCC_DEBUG_LOG(...)                         \
  {                                                \
    if (dcc_debug_log) dcc_debug_log(__VA_ARGS__); \
  }

#endif
