// spell-checker:words uldiff

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "logic_internal.h"

#define DCC_ERROR_LOG(...)                                                               \
  do {                                                                                   \
    if (dcc_error_log != NULL) dcc_error_log(__FILE__, __LINE__, __func__, __VA_ARGS__); \
    fprintf(stderr, "error: %s:%d:%s: ", __FILE__, __LINE__, __func__);                  \
    fprintf(stderr, __VA_ARGS__);                                                        \
    fprintf(stderr, "\n");                                                               \
    exit(EXIT_FAILURE);                                                                  \
  } while (0)

#define DCC_DEBUG_LOG(...) (dcc_debug_log == NULL ? 0 : dcc_debug_log(__FILE__, __LINE__, __func__, __VA_ARGS__))

#define DCC_UNREACHABLE(...) DCC_ERROR_LOG("unreachable: "__VA_ARGS__)

#define DCC_UNIMPLEMENTED() DCC_ERROR_LOG("unimplemented")

#ifdef DCC_ASSERT
#undef DCC_ASSERT
#define DCC_ASSERT(e) assert(e)
#endif

void (*dcc_error_log)(char const *const file, int const line, char const *func, char const *format, ...) = NULL;

int (*dcc_debug_log)(char const *const file, int const line, char const *func, char const *format, ...) = NULL;

// `1` の半ビットの転送継続時間の最小値
dcc_TimeMicroSec const dcc_minOneHalfBitSentPeriod = 55UL;

// `1` の半ビットの転送継続時間の最大値
dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriod = 61UL;

// 受信した `1` の半ビットの許容時間の最小値
dcc_TimeMicroSec const dcc_minOneHalfBitReceivedPeriod = 52UL;

// 受信した `1` の半ビットの許容時間の最大値
dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriod = 64UL;

// `0` の半ビットの転送継続時間の最小値
dcc_TimeMicroSec const dcc_minZeroHalfBitSentPeriod = 95UL;

// `0` の半ビットの転送継続時間の最大値
dcc_TimeMicroSec const dcc_maxZeroHalfBitSentPeriod = 9900UL;

// 引き伸ばされた `0` ビットの総和継続時間の最大値
dcc_TimeMicroSec const dcc_maxStretchedZeroBitPeriod = 12000UL;

// 受信した `0` の半ビットの許容時間の最小値
dcc_TimeMicroSec const dcc_minZeroHalfBitReceivedPeriod = 90UL;

// 受信した `0` の半ビットの許容時間の最大値
dcc_TimeMicroSec const dcc_maxZeroHalfBitReceivedPeriod = 10000UL;

// 転送した `1` の半ビットの時間の差の最大値
dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriodDiff = 3UL;

// 受信した `1` の半ビットの時間の差の最大値
dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriodDiff = 6UL;

#define LOG_BUFFER_SIZE 1024

static unsigned long uldiff(unsigned long const a, unsigned long const b) { return a > b ? a - b : b - a; }

struct dcc_SignalBuffer dcc_initializeSignalBuffer(dcc_TimeMicroSec *buffer, size_t const size) {
  return (struct dcc_SignalBuffer) { .buffer = buffer, .size = size, .written = 0, .writeIndex = 0, .readIndex = 0 };
}

enum dcc_Result dcc_writeSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec const signal) {
  DCC_DEBUG_LOG("dcc_writeSignalBuffer(buffer: %p, signal: %lu)", buffer, signal);
  if (buffer->written == buffer->size) return dcc_Failure;
  buffer->buffer[buffer->writeIndex] = signal;
  buffer->written++;
  buffer->writeIndex = (buffer->writeIndex + 1) % buffer->size;
  return dcc_Success;
}

enum dcc_Result dcc_readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal) {
  DCC_DEBUG_LOG("dcc_readSignalBuffer(buffer: %p, signal: %p)", buffer, signal);
  if (buffer->written == 0) return dcc_Failure;
  *signal = buffer->buffer[buffer->readIndex];
  buffer->written--;
  buffer->readIndex = (buffer->readIndex + 1) % buffer->size;
  return dcc_Success;
}

struct dcc_SignalStreamParser dcc_initializeSignalStreamParser(void) {
  return (struct dcc_SignalStreamParser) { { 0 }, 0 };
}

enum dcc_StreamParserResult dcc_feedSignal(struct dcc_SignalStreamParser *const parser, dcc_TimeMicroSec const signal,
                                           dcc_Bit *const bit) {
  DCC_DEBUG_LOG("dcc_feedSignal(parser: %p, signal: %lu, bit: %p)", parser, signal, bit);
  switch (parser->signalsSize) {
    case 0:
    case 1:
      parser->signals[parser->signalsSize] = signal;
      parser->signalsSize++;
      return dcc_StreamParserResult_Continue;
    case 2: {
      // TODO signal がオーバーフローした場合の処理が必要
      enum dcc_Result const result =
        dcc_decodeSignal(parser->signals[1] - parser->signals[0], signal - parser->signals[1], bit);
      switch (result) {
        case dcc_Failure:
          parser->signals[0] = parser->signals[1];
          parser->signals[1] = signal;
          return dcc_StreamParserResult_Failure;
        case dcc_Success:
          parser->signals[0] = signal;
          parser->signalsSize = 1;
          return dcc_StreamParserResult_Success;
        default:
          DCC_UNREACHABLE("result: %d", result);
      }
    }
    default:
      DCC_UNREACHABLE("signalsSize: %zd", parser->signalsSize);
  }
}

struct dcc_BitStreamParser dcc_initializeBitStreamParser(void) {
  return (struct dcc_BitStreamParser) {
    .state = dcc_BitStreamParserState_InPreamble,
    .inPreamble = { .oneBitsCount = 0 },
    .bytes = { 0 },
    .bytesSize = 0,
  };
}

enum dcc_StreamParserResult dcc_feedBit(struct dcc_BitStreamParser *const parser, dcc_Bit const bit,
                                        dcc_Byte *const bytes, size_t *const bytesSize) {
  DCC_DEBUG_LOG("dcc_feedBit(parser: %p, bit: %d, bytes: %p, bytesSize: %p)", parser, bit, bytes, bytesSize);
  switch (parser->state) {
    case dcc_BitStreamParserState_InPreamble:
      if (bit) {
        parser->inPreamble.oneBitsCount++;
        return dcc_StreamParserResult_Continue;
      }
      if (parser->inPreamble.oneBitsCount <= 12) {
        DCC_DEBUG_LOG("too short preamble: one bits count: %d", parser->inPreamble.oneBitsCount);
        *parser = dcc_initializeBitStreamParser();
        return dcc_StreamParserResult_Failure;
      }
      parser->state = dcc_BitStreamParserState_InByte;
      parser->inByte.byte = 0;
      parser->inByte.bitCount = 0;
      return dcc_StreamParserResult_Continue;
    case dcc_BitStreamParserState_InByte:
      parser->inByte.byte |= (bit << (7 - parser->inByte.bitCount));
      parser->inByte.bitCount++;
      if (parser->inByte.bitCount < 8) return dcc_StreamParserResult_Continue;
      parser->bytes[parser->bytesSize] = parser->inByte.byte;
      parser->bytesSize++;
      parser->state = dcc_BitStreamParserState_AfterByte;
      return dcc_StreamParserResult_Continue;
    case dcc_BitStreamParserState_AfterByte:
      if (bit) {
        memcpy(bytes, parser->bytes, parser->bytesSize);
        *bytesSize = parser->bytesSize;
        parser->state = dcc_BitStreamParserState_InPreamble;
        parser->inPreamble.oneBitsCount = 0;
        parser->bytesSize = 0;
        return dcc_StreamParserResult_Success;
      }
      parser->state = dcc_BitStreamParserState_InByte;
      parser->inByte.byte = 0;
      parser->inByte.bitCount = 0;
      return dcc_StreamParserResult_Continue;
    default:
      DCC_UNREACHABLE("state: %d", parser->state);
  }
}

enum dcc_Result dcc_decodeSignal(dcc_TimeMicroSec const period1, dcc_TimeMicroSec const period2, dcc_Bit *const bit) {
  if (period1 >= dcc_minOneHalfBitReceivedPeriod && period1 <= dcc_maxOneHalfBitReceivedPeriod &&
      uldiff(period1, period2) <= dcc_maxOneHalfBitReceivedPeriodDiff) {
    *bit = 1;
    return dcc_Success;
  }
  if (period1 >= dcc_minZeroHalfBitReceivedPeriod && period1 <= dcc_maxZeroHalfBitReceivedPeriod &&
      period2 >= dcc_minZeroHalfBitReceivedPeriod && period2 <= dcc_maxZeroHalfBitReceivedPeriod &&
      period1 + period2 <= dcc_maxStretchedZeroBitPeriod) {
    *bit = 0;
    return dcc_Success;
  }
  return dcc_Failure;
}

enum dcc_Result dcc_validatePacket(dcc_Byte const *const bytes, size_t const bytesSize, dcc_Byte const checksum) {
  dcc_Byte sum = 0;
  for (size_t i = 0; i < bytesSize; i++) sum ^= bytes[i];
  if (sum == checksum) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parsePacket(dcc_Byte const *const bytes, size_t const bytesSize, struct dcc_Packet *const packet) {
  // 速度・方向パケット内で FL を制御するかどうか（CV 29 のビット1が `0` のとき `true`）
  bool const flControl = false;  // TODO CV の実装ができたら書き換える
  if (dcc_Success ==
      dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(bytes,
                                                            bytesSize,
                                                            flControl,
                                                            &packet->speedAndDirectionPacketForLocomotiveDecoders)) {
    packet->tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseResetPacketForAllDecoders(bytes, bytesSize)) {
    packet->tag = dcc_ResetPacketForAllDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseIdlePacketForAllDecoders(bytes, bytesSize)) {
    packet->tag = dcc_IdlePacketForAllDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseBroadcastStopPacketForAllDecoders(bytes, bytesSize, &packet->broadcastStopPacketForAllDecoders)) {
    packet->tag = dcc_BroadcastStopPacketForAllDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseResetPacketForMultiFunctionDecoders(bytes, bytesSize, &packet->resetPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_ResetPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseHardResetPacketForMultiFunctionDecoders(bytes,
                                                       bytesSize,
                                                       &packet->hardResetPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_HardResetPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseFactoryTestInstructionPacketForMultiFunctionDecoders(
                       bytes,
                       bytesSize,
                       &packet->factoryTestInstructionPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseSetDecoderFlagsPacketForMultiFunctionDecoders(bytes,
                                                             bytesSize,
                                                             &packet->setDecoderFlagsPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseSetExtendedAddressingPacketForMultiFunctionDecoders(
                       bytes,
                       bytesSize,
                       &packet->setExtendedAddressingPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseDecoderAcknowledgementRequestPacket(
                       bytes,
                       bytesSize,
                       &packet->decoderAcknowledgementRequestPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseConsistControlPacket(bytes, bytesSize, &packet->consistControlPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_ConsistControlPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseSpeedStep128ControlPacket(bytes, bytesSize, &packet->speedStep128ControlPacket)) {
    packet->tag = dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseRestrictedSpeedStepPacketForMultiFunctionDecoders(
                       bytes,
                       bytesSize,
                       flControl,
                       &packet->restrictedSpeedStepPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseAnalogFunctionPacketForMultiFunctionDecoders(bytes,
                                                            bytesSize,
                                                            &packet->analogFunctionPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_AnalogFunctionPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseSpeedAndDirectionPacketForMultiFunctionDecoders(
                       bytes,
                       bytesSize,
                       flControl,
                       &packet->speedAndDirectionPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseFunctionGroup1PacketForMultiFunctionDecoders(bytes,
                                                            bytesSize,
                                                            !flControl,
                                                            &packet->functionGroup1PacketForMultiFunctionDecoders)) {
    packet->tag = dcc_FunctionGroup1PacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseFunctionGroup2PacketForMultiFunctionDecoders(bytes,
                                                            bytesSize,
                                                            &packet->functionGroup2PacketForMultiFunctionDecoders)) {
    packet->tag = dcc_FunctionGroup2PacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseBinaryStateControlLongFormPacket(bytes, bytesSize, &packet->binaryStateControlLongFormPacket)) {
    packet->tag = dcc_BinaryStateControlLongFormPacketTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseBinaryStateControlShortFormPacket(bytes, bytesSize, &packet->binaryStateControlShortFormPacket)) {
    packet->tag = dcc_BinaryStateControlShortFormPacketTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseFunctionControlF13F20Packet(bytes, bytesSize, &packet->functionControlF13F20Packet)) {
    packet->tag = dcc_FunctionControlF13F20PacketTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseFunctionControlF21F28Packet(bytes, bytesSize, &packet->functionControlF21F28Packet)) {
    packet->tag = dcc_FunctionControlF21F28PacketTag;
    return dcc_Success;
  }
  return dcc_Failure;
}

void parseSpeed4Bit(dcc_Byte const byte, dcc_Speed4Bit *const speed, bool *const emergencyStop) {
  *emergencyStop = false;
  dcc_Speed4Bit const speed_ = (dcc_Speed4Bit) (byte & 0x1F);
  switch (speed_) {
    case 0:
      *speed = 0;
      break;
    case 1:
      *speed = 0;
      *emergencyStop = true;
      break;
    default:
      *speed = speed_ - 1;
      break;
  }
}

void parseSpeed5Bit(dcc_Byte const byte, dcc_Speed5Bit *const speed, bool *const emergencyStop,
                    bool *const directionMayBeIgnored) {
  *emergencyStop = false;
  *directionMayBeIgnored = false;
  dcc_Speed5Bit speed_ = (dcc_Speed5Bit) (((byte & 0xF) << 1) | ((byte & 0x10) >> 4));
  switch (speed_) {
    case 0:
      *speed = 0;
      break;
    case 1:
      *speed = 0;
      *directionMayBeIgnored = true;
      break;
    case 2:
      *speed = 0;
      *emergencyStop = true;
      break;
    case 3:
      *speed = 0;
      *emergencyStop = true;
      *directionMayBeIgnored = true;
      break;
    default:
      *speed = speed_ - 3;
      break;
  }
}

enum dcc_Result dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, bool flControl,
  struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders *const packet) {
  if (bytesSize != 3) return dcc_Failure;
  if ((bytes[1] & 0xC0) != 0x40) return dcc_Failure;  // check packet identifier
  if (bytes[0] & 0x80) return dcc_Failure;
  if (bytes[0] == 0) return dcc_Failure;
  packet->address = (dcc_AddressForBaselinePacket) (bytes[0] & 0x7F);
  packet->direction = bytes[1] & 0x20 ? dcc_Forward : dcc_Backward;
  packet->flControl = flControl;
  packet->emergencyStop = false;
  if (flControl) {
    parseSpeed4Bit(bytes[1], &packet->speed4Bit, &packet->emergencyStop);
    packet->fl = (bytes[1] & 0x10) >> 4;
  } else {
    parseSpeed5Bit(bytes[1], &packet->speed5Bit, &packet->emergencyStop, &packet->directionMayBeIgnored);
  }
  return dcc_Success;
}

enum dcc_Result dcc_parseResetPacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize) {
  if (bytesSize != 3) return dcc_Failure;
  if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseIdlePacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize) {
  if (bytesSize != 3) return dcc_Failure;
  if (bytes[0] == 0xFF && bytes[1] == 0 && bytes[2] == 0xFF) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseBroadcastStopPacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize,
                                                           struct dcc_BroadcastStopPacketForAllDecoders *const packet) {
  if (bytesSize != 3) return dcc_Failure;
  if (bytes[0] != 0 || (bytes[1] & 0xCE) != 0x40) return dcc_Failure;
  packet->direction = bytes[1] & 0x20 ? dcc_Forward : dcc_Backward;
  packet->directionMayBeIgnored = (bytes[1] & 0x10) != 0;
  packet->kind = (enum dcc_BroadcastStopKind)(bytes[1] & 1);
  return dcc_Success;
}

static enum dcc_Result parseAddressForExtendedPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                     dcc_AddressForExtendedPacket *const address,
                                                     size_t *const addressSize) {
  if (bytesSize < 1) return dcc_Failure;
  // `0`（ブロードキャスト）と `1` ～ `127`
  if (bytes[0] <= 0x7F) {
    *address = bytes[0];
    *addressSize = 1;
    return dcc_Success;
  }
  // `192` ～ `231`
  if (0xC0 <= bytes[0] && bytes[0] <= 0xE7) {
    if (bytesSize < 2) return dcc_Failure;
    *address = (dcc_AddressForExtendedPacket) ((bytes[0] & 0x3F) << 8 | bytes[1]);
    *addressSize = 2;
    return dcc_Success;
  }
  // アクセサリーデコーダー、予約、アイドル
  return dcc_Failure;
}

// 多機能デコーダー用パケットのアドレスをパースし、命令部分の位置とバイト数を得る。
// 命令は1つだけ含まれるものとして扱う。
static enum dcc_Result parseMultiFunctionDecoderPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                       dcc_AddressForExtendedPacket *const address,
                                                       dcc_Byte const **const instruction,
                                                       size_t *const instructionSize) {
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, address, &addressSize)) return dcc_Failure;
  // 最後の1バイトは誤り検出バイト
  if (bytesSize < addressSize + 2) return dcc_Failure;
  *instruction = bytes + addressSize;
  *instructionSize = bytesSize - addressSize - 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, struct dcc_ResetPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if (instruction[0] == 0) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseHardResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_HardResetPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if (instruction[0] == 1) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseFactoryTestInstructionPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if ((instruction[0] & 0xFE) != 2) return dcc_Failure;
  packet->set = instruction[0] & 1;
  switch (instructionSize) {
    case 1:
      packet->dataExists = false;
      return dcc_Success;
    case 2:
      packet->dataExists = true;
      packet->data = instruction[1];
      return dcc_Success;
    default:
      return dcc_Failure;
  }
}

enum dcc_Result dcc_parseSetDecoderFlagsPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SetDecoderFlagsPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if ((instruction[0] & 0xFE) != 6) return dcc_Failure;
  if (instruction[1] & 0x08) return dcc_Failure;
  packet->set = instruction[0] & 1;
  packet->subaddress = instruction[1] & 7;
  uint_least8_t const flagsInstruction = instruction[1] >> 4;
  if (flagsInstruction != dcc_Disable111Instructions &&
      flagsInstruction != dcc_DisableDecoderAcknowledgementRequestInstruction &&
      flagsInstruction != dcc_ActivateBiDirectionalCommunications &&
      flagsInstruction != dcc_SetBiDirectionalCommunications && flagsInstruction != dcc_Set111Instruction &&
      flagsInstruction != dcc_Accept111Instructions) {
    return dcc_Failure;
  }
  packet->instruction = (enum dcc_DecoderFlagsInstruction) flagsInstruction;
  return dcc_Success;
}

enum dcc_Result dcc_parseSetExtendedAddressingPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SetExtendedAddressingPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if ((instruction[0] & 0xFE) != 0xA) return dcc_Failure;
  packet->set = instruction[0] & 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseDecoderAcknowledgementRequestPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if ((instruction[0] & 0xFE) != 0xE) return dcc_Failure;
  packet->set = instruction[0] & 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseConsistControlPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                              struct dcc_ConsistControlPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if ((instruction[0] & 0xF0) != 0x10) return dcc_Failure;
  switch (instruction[0] & 0x0F) {
    case 2:
      packet->direction = dcc_Forward;
      break;
    case 3:
      packet->direction = dcc_Backward;
      break;
    default:
      return dcc_Failure;
  }
  packet->consistAddress = (dcc_ConsistAddress) (instruction[1] & 0x7F);
  return dcc_Success;
}

enum dcc_Result dcc_parseSpeedStep128ControlPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if (instruction[0] != 0x3F) return dcc_Failure;
  if ((instruction[1] & 0x80) == 0) packet->direction = dcc_Backward;
  else packet->direction = dcc_Forward;
  dcc_Speed7Bit const speed = (dcc_Speed7Bit) (instruction[1] & 0x7F);
  switch (speed) {
    case 0:
      packet->emergencyStop = false;
      packet->speed = 0;
      break;
    case 1:
      packet->emergencyStop = true;
      packet->speed = 0;
      break;
    default:
      packet->emergencyStop = false;
      packet->speed = (dcc_Speed7Bit) (speed - 1);
      break;
  }
  return dcc_Success;
}

enum dcc_Result dcc_parseRestrictedSpeedStepPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, bool flControl,
  struct dcc_RestrictedSpeedStepPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if (instruction[0] != 0x3E) return dcc_Failure;
  packet->enabled = (instruction[1] & 0x80) == 0;
  packet->flControl = flControl;
  bool emergencyStop;
  bool directionMayBeIgnored;
  if (flControl) parseSpeed4Bit(instruction[1], &packet->speed4Bit, &emergencyStop);
  else parseSpeed5Bit(instruction[1], &packet->speed5Bit, &emergencyStop, &directionMayBeIgnored);
  return dcc_Success;
}

enum dcc_Result dcc_parseAnalogFunctionPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_AnalogFunctionPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 3) return dcc_Failure;
  if (instruction[0] != 0x3D) return dcc_Failure;
  packet->output = instruction[1];
  packet->data = instruction[2];
  return dcc_Success;
}

enum dcc_Result dcc_parseSpeedAndDirectionPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, bool flControl,
  struct dcc_SpeedAndDirectionPacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if ((instruction[0] & 0xC0) != 0x40) return dcc_Failure;
  packet->direction = instruction[0] & 0x20 ? dcc_Forward : dcc_Backward;
  packet->flControl = flControl;
  if (flControl) {
    parseSpeed4Bit(instruction[0], &packet->speed4Bit, &packet->emergencyStop);
    packet->fl = (instruction[0] & 0x10) >> 4;
    packet->directionMayBeIgnored = false;
  } else {
    parseSpeed5Bit(instruction[0], &packet->speed5Bit, &packet->emergencyStop, &packet->directionMayBeIgnored);
  }
  return dcc_Success;
}

enum dcc_Result dcc_parseFunctionGroup1PacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, bool flControl,
  struct dcc_FunctionGroup1PacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if ((instruction[0] & 0xE0) != 0x80) return dcc_Failure;
  packet->flControl = flControl;
  packet->fl = (instruction[0] & 0x10) != 0;
  packet->f1 = (instruction[0] & 0x01) != 0;
  packet->f2 = (instruction[0] & 0x02) != 0;
  packet->f3 = (instruction[0] & 0x04) != 0;
  packet->f4 = (instruction[0] & 0x08) != 0;
  return dcc_Success;
}

enum dcc_Result dcc_parseFunctionGroup2PacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_FunctionGroup2PacketForMultiFunctionDecoders *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 1) return dcc_Failure;
  if ((instruction[0] & 0xE0) != 0xA0) return dcc_Failure;
  packet->group = instruction[0] & 0x10 ? dcc_FunctionGroup2Group_F5_F8 : dcc_FunctionGroup2Group_F9_F12;
  packet->functions.f5 = (instruction[0] & 0x01) != 0;
  packet->functions.f6 = (instruction[0] & 0x02) != 0;
  packet->functions.f7 = (instruction[0] & 0x04) != 0;
  packet->functions.f8 = (instruction[0] & 0x08) != 0;
  return dcc_Success;
}

enum dcc_Result dcc_parseBinaryStateControlLongFormPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                          struct dcc_BinaryStateControlLongFormPacket *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 3) return dcc_Failure;
  if (instruction[0] != 0xC0) return dcc_Failure;
  packet->state = (instruction[1] & 0x80) != 0;
  packet->stateAddress = (dcc_BinaryStateAddressLongForm) (instruction[2] << 7 | (instruction[1] & 0x7F));
  return dcc_Success;
}

enum dcc_Result dcc_parseBinaryStateControlShortFormPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                           struct dcc_BinaryStateControlShortFormPacket *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if (instruction[0] != 0xDD) return dcc_Failure;
  packet->state = (instruction[1] & 0x80) != 0;
  packet->stateAddress = (dcc_BinaryStateAddressShortForm) (instruction[1] & 0x7F);
  return dcc_Success;
}

enum dcc_Result dcc_parseFunctionControlF13F20Packet(dcc_Byte const *const bytes, size_t const bytesSize,
                                                     struct dcc_FunctionControlF13F20Packet *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if (instruction[0] != 0xDE) return dcc_Failure;
  packet->f13 = (instruction[1] & 0x01) != 0;
  packet->f14 = (instruction[1] & 0x02) != 0;
  packet->f15 = (instruction[1] & 0x04) != 0;
  packet->f16 = (instruction[1] & 0x08) != 0;
  packet->f17 = (instruction[1] & 0x10) != 0;
  packet->f18 = (instruction[1] & 0x20) != 0;
  packet->f19 = (instruction[1] & 0x40) != 0;
  packet->f20 = (instruction[1] & 0x80) != 0;
  return dcc_Success;
}

enum dcc_Result dcc_parseFunctionControlF21F28Packet(dcc_Byte const *const bytes, size_t const bytesSize,
                                                     struct dcc_FunctionControlF21F28Packet *const packet) {
  dcc_Byte const *instruction;
  size_t instructionSize;
  if (dcc_Failure ==
      parseMultiFunctionDecoderPacket(bytes, bytesSize, &packet->address, &instruction, &instructionSize))
    return dcc_Failure;
  if (instructionSize != 2) return dcc_Failure;
  if (instruction[0] != 0xDF) return dcc_Failure;
  packet->f21 = (instruction[1] & 0x01) != 0;
  packet->f22 = (instruction[1] & 0x02) != 0;
  packet->f23 = (instruction[1] & 0x04) != 0;
  packet->f24 = (instruction[1] & 0x08) != 0;
  packet->f25 = (instruction[1] & 0x10) != 0;
  packet->f26 = (instruction[1] & 0x20) != 0;
  packet->f27 = (instruction[1] & 0x40) != 0;
  packet->f28 = (instruction[1] & 0x80) != 0;
  return dcc_Success;
}

struct dcc_Decoder dcc_initializeDecoder(dcc_TimeMicroSec *signalBufferValues, size_t const signalBufferSize) {
  return (struct dcc_Decoder) { .signalBuffer = dcc_initializeSignalBuffer(signalBufferValues, signalBufferSize),
                                .signalStreamParser = dcc_initializeSignalStreamParser(),
                                .bitStreamParser = dcc_initializeBitStreamParser() };
}

enum dcc_StreamParserResult dcc_decode(struct dcc_Decoder *const decoder, dcc_TimeMicroSec const signal,
                                       struct dcc_Packet *const packet) {
  DCC_DEBUG_LOG("dcc_decode(decoder: %p, signal: %lu, packet: %p)", decoder, signal, packet);
  dcc_Bit bit;
  {
    enum dcc_StreamParserResult const result = dcc_feedSignal(&decoder->signalStreamParser, signal, &bit);
    switch (result) {
      case dcc_StreamParserResult_Failure:
        // 直近の3つの信号の組ではビットにならなかった
        // そのまま次の信号を待つ
        return dcc_StreamParserResult_Continue;
      case dcc_StreamParserResult_Continue:
        return dcc_StreamParserResult_Continue;
      case dcc_StreamParserResult_Success:
        break;
      default:
        DCC_UNREACHABLE("result: %d", result);
    }
  }
  dcc_Byte bytes[DCC_BIT_STREAM_PARSER_BYTES_CAPACITY];
  size_t bytesSize;
  {
    enum dcc_StreamParserResult const result = dcc_feedBit(&decoder->bitStreamParser, bit, bytes, &bytesSize);
    switch (result) {
      case dcc_StreamParserResult_Failure:
        DCC_DEBUG_LOG("dcc_feedBit failed");
        return dcc_StreamParserResult_Failure;
      case dcc_StreamParserResult_Continue:
        return dcc_StreamParserResult_Continue;
      case dcc_StreamParserResult_Success:
        break;
      default:
        DCC_UNREACHABLE("result: %d", result);
    }
  }
  if (dcc_Failure == dcc_validatePacket(bytes, bytesSize - 1, bytes[bytesSize - 1])) {
    DCC_DEBUG_LOG("dcc_validatePacket failed");
    return dcc_StreamParserResult_Failure;
  }
  {
    enum dcc_Result const result = dcc_parsePacket(bytes, bytesSize, packet);
    switch (result) {
      case dcc_Failure:
        DCC_DEBUG_LOG("dcc_parsePacket failed");
        return dcc_StreamParserResult_Failure;
      case dcc_Success:
        return dcc_StreamParserResult_Success;
      default:
        DCC_UNREACHABLE("result: %d", result);
    }
  }
}

#define SHOW_BOOL(value) ((value) ? "true" : "false")

int dcc_showSignalBuffer(char *buffer, size_t const bufferSize, struct dcc_SignalBuffer const signalBuffer) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"buffer\":%p,\"size\":%zu,\"written\":%zu,\"writeIndex\":%zu,\"readIndex\":%zu}",
                  signalBuffer.buffer,
                  signalBuffer.size,
                  signalBuffer.written,
                  signalBuffer.writeIndex,
                  signalBuffer.readIndex);
}

int dcc_showBytes(char *buffer, size_t const bufferSize, dcc_Byte const *const packet, size_t const packetSize) {
  int writtenSize = 0;
  for (size_t i = 0; i < packetSize; i++) {
    if (i != 0) writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, " ");
    writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "%02X", packet[i]);
  }
  return writtenSize;
}

#define SHOW_DIRECTION(value) \
  ((value) == dcc_Forward ? "\"Forward\"" : (value) == dcc_Backward ? "\"Backward\"" : "\"Unknown\"")

#define SHOW_BROADCAST_STOP_KIND(value)                         \
  ((value) == dcc_BroadcastStopKind_Stop       ? "\"Stop\""     \
   : (value) == dcc_BroadcastStopKind_Shutdown ? "\"Shutdown\"" \
                                               : "\"Unknown\"")

#define SHOW_DECODER_FLAGS_INSTRUCTION(value)                                                       \
  ((value) == dcc_Disable111Instructions ? "\"Disable111Instructions\""                             \
   : (value) == dcc_DisableDecoderAcknowledgementRequestInstruction                                 \
     ? "\"DisableDecoderAcknowledgementRequestInstruction\""                                        \
   : (value) == dcc_ActivateBiDirectionalCommunications ? "\"ActivateBiDirectionalCommunications\"" \
   : (value) == dcc_SetBiDirectionalCommunications      ? "\"SetBiDirectionalCommunications\""      \
   : (value) == dcc_Set111Instruction                   ? "\"Set111Instruction\""                   \
   : (value) == dcc_Accept111Instructions               ? "\"Accept111Instructions\""               \
                                                        : "\"Unknown\"")

int dcc_showSpeedAndDirectionPacketForLocomotiveDecoders(
  char *buffer, size_t const bufferSize, struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          "{\"address\":%d,\"direction\":%s,\"flControl\":%s",
                          packet.address,
                          SHOW_DIRECTION(packet.direction),
                          SHOW_BOOL(packet.flControl));
  if (packet.flControl) {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed4Bit\":%d", packet.speed4Bit);
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"fl\":%s", SHOW_BOOL(packet.fl));
  } else {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed5Bit\":%d", packet.speed5Bit);
    writtenSize += snprintf(buffer + writtenSize,
                            bufferSize - (size_t) writtenSize,
                            ",\"directionMayBeIgnored\":%s",
                            SHOW_BOOL(packet.directionMayBeIgnored));
  }
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          ",\"emergencyStop\":%s}",
                          SHOW_BOOL(packet.emergencyStop));
  return writtenSize;
}

int dcc_showBroadcastStopPacketForAllDecoders(char *buffer, size_t const bufferSize,
                                              struct dcc_BroadcastStopPacketForAllDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"kind\":%s,\"directionMayBeIgnored\":%s,\"direction\":%s}",
                  SHOW_BROADCAST_STOP_KIND(packet.kind),
                  SHOW_BOOL(packet.directionMayBeIgnored),
                  SHOW_DIRECTION(packet.direction));
}

int dcc_showResetPacketForMultiFunctionDecoders(char *buffer, size_t const bufferSize,
                                                struct dcc_ResetPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showHardResetPacketForMultiFunctionDecoders(char *buffer, size_t const bufferSize,
                                                    struct dcc_HardResetPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showFactoryTestInstructionPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          "{\"address\":%d,\"set\":%s,\"dataExists\":%s",
                          packet.address,
                          SHOW_BOOL(packet.set),
                          SHOW_BOOL(packet.dataExists));
  if (packet.dataExists)
    writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"data\":\"%#x\"", packet.data);
  writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
  return writtenSize;
}

int dcc_showSetDecoderFlagsPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_SetDecoderFlagsPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"set\":%s,\"subaddress\":%d,\"instruction\":%s}",
                  packet.address,
                  SHOW_BOOL(packet.set),
                  packet.subaddress,
                  SHOW_DECODER_FLAGS_INSTRUCTION(packet.instruction));
}

int dcc_showDecoderAcknowledgementRequestPacket(
  char *buffer, size_t const bufferSize,
  struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showConsistControlPacket(char *buffer, size_t const bufferSize,
                                 struct dcc_ConsistControlPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"direction\":%s,\"consistAddress\":%d}",
                  packet.address,
                  SHOW_DIRECTION(packet.direction),
                  packet.consistAddress);
}

int dcc_showSpeedStep128ControlPacket(char *buffer, size_t const bufferSize,
                                      struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"direction\":%s,\"emergencyStop\":%s,\"speed\":%d}",
                  packet.address,
                  SHOW_DIRECTION(packet.direction),
                  SHOW_BOOL(packet.emergencyStop),
                  packet.speed);
}

int dcc_showSetExtendedAddressingPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_SetExtendedAddressingPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d,\"set\":%s}", packet.address, SHOW_BOOL(packet.set));
}

int dcc_showRestrictedSpeedStepPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_RestrictedSpeedStepPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"enabled\":%s,\"flControl\":%s,\"%s\":%d}",
                  packet.address,
                  SHOW_BOOL(packet.enabled),
                  SHOW_BOOL(packet.flControl),
                  packet.flControl ? "speed4Bit" : "speed5Bit",
                  packet.flControl ? packet.speed4Bit : packet.speed5Bit);
}

int dcc_showAnalogFunctionPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_AnalogFunctionPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"output\":%d,\"data\":%d}",
                  packet.address,
                  packet.output,
                  packet.data);
}

int dcc_showSpeedAndDirectionPacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_SpeedAndDirectionPacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          "{\"address\":%d,\"direction\":%s,\"flControl\":%s",
                          packet.address,
                          SHOW_DIRECTION(packet.direction),
                          SHOW_BOOL(packet.flControl));
  if (packet.flControl) {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed4Bit\":%d", packet.speed4Bit);
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"fl\":%s", SHOW_BOOL(packet.fl));
  } else {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed5Bit\":%d", packet.speed5Bit);
  }
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          ",\"directionMayBeIgnored\":%s,\"emergencyStop\":%s}",
                          SHOW_BOOL(packet.directionMayBeIgnored),
                          SHOW_BOOL(packet.emergencyStop));
  return writtenSize;
}

int dcc_showFunctionGroup1PacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_FunctionGroup1PacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          "{\"address\":%d,\"flControl\":%s",
                          packet.address,
                          SHOW_BOOL(packet.flControl));
  if (packet.flControl)
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"fl\":%s", SHOW_BOOL(packet.fl));
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          ",\"f1\":%s,\"f2\":%s,\"f3\":%s,\"f4\":%s}",
                          SHOW_BOOL(packet.f1),
                          SHOW_BOOL(packet.f2),
                          SHOW_BOOL(packet.f3),
                          SHOW_BOOL(packet.f4));
  return writtenSize;
}

int dcc_showFunctionGroup2PacketForMultiFunctionDecoders(
  char *buffer, size_t const bufferSize, struct dcc_FunctionGroup2PacketForMultiFunctionDecoders const packet) {
  if (packet.group == dcc_FunctionGroup2Group_F5_F8)
    return snprintf(buffer,
                    bufferSize,
                    "{\"address\":%d,\"group\":\"F5_F8\",\"f5\":%s,\"f6\":%s,\"f7\":%s,\"f8\":%s}",
                    packet.address,
                    SHOW_BOOL(packet.functions.f5),
                    SHOW_BOOL(packet.functions.f6),
                    SHOW_BOOL(packet.functions.f7),
                    SHOW_BOOL(packet.functions.f8));
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"group\":\"F9_F12\",\"f9\":%s,\"f10\":%s,\"f11\":%s,\"f12\":%s}",
                  packet.address,
                  SHOW_BOOL(packet.functions.f9),
                  SHOW_BOOL(packet.functions.f10),
                  SHOW_BOOL(packet.functions.f11),
                  SHOW_BOOL(packet.functions.f12));
}

int dcc_showBinaryStateControlLongFormPacket(char *buffer, size_t const bufferSize,
                                             struct dcc_BinaryStateControlLongFormPacket const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"stateAddress\":%d,\"state\":%s}",
                  packet.address,
                  packet.stateAddress,
                  SHOW_BOOL(packet.state));
}

int dcc_showBinaryStateControlShortFormPacket(char *buffer, size_t const bufferSize,
                                              struct dcc_BinaryStateControlShortFormPacket const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"stateAddress\":%d,\"state\":%s}",
                  packet.address,
                  packet.stateAddress,
                  SHOW_BOOL(packet.state));
}

int dcc_showFunctionControlF13F20Packet(char *buffer, size_t const bufferSize,
                                        struct dcc_FunctionControlF13F20Packet const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"f13\":%s,\"f14\":%s,\"f15\":%s,\"f16\":%s,\"f17\":%s,\"f18\":%s,\"f19\":%s,"
                  "\"f20\":%s}",
                  packet.address,
                  SHOW_BOOL(packet.f13),
                  SHOW_BOOL(packet.f14),
                  SHOW_BOOL(packet.f15),
                  SHOW_BOOL(packet.f16),
                  SHOW_BOOL(packet.f17),
                  SHOW_BOOL(packet.f18),
                  SHOW_BOOL(packet.f19),
                  SHOW_BOOL(packet.f20));
}

int dcc_showFunctionControlF21F28Packet(char *buffer, size_t const bufferSize,
                                        struct dcc_FunctionControlF21F28Packet const packet) {
  return snprintf(buffer,
                  bufferSize,
                  "{\"address\":%d,\"f21\":%s,\"f22\":%s,\"f23\":%s,\"f24\":%s,\"f25\":%s,\"f26\":%s,\"f27\":%s,"
                  "\"f28\":%s}",
                  packet.address,
                  SHOW_BOOL(packet.f21),
                  SHOW_BOOL(packet.f22),
                  SHOW_BOOL(packet.f23),
                  SHOW_BOOL(packet.f24),
                  SHOW_BOOL(packet.f25),
                  SHOW_BOOL(packet.f26),
                  SHOW_BOOL(packet.f27),
                  SHOW_BOOL(packet.f28));
}

int dcc_showPacket(char *buffer, size_t const bufferSize, struct dcc_Packet const packet) {
  int writtenSize = 0;
  switch (packet.tag) {
    case dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showSpeedAndDirectionPacketForLocomotiveDecoders(buffer + writtenSize,
                                                             bufferSize - (size_t) writtenSize,
                                                             packet.speedAndDirectionPacketForLocomotiveDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_ResetPacketForAllDecodersTag:
      return snprintf(buffer, bufferSize, "{\"tag\":\"dcc_ResetPacketForAllDecodersTag\"}");
    case dcc_IdlePacketForAllDecodersTag:
      return snprintf(buffer, bufferSize, "{\"tag\":\"dcc_IdlePacketForAllDecodersTag\"}");
    case dcc_BroadcastStopPacketForAllDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_BroadcastStopPacketForAllDecodersTag\",\"packet\":");
      writtenSize += dcc_showBroadcastStopPacketForAllDecoders(buffer + writtenSize,
                                                               bufferSize - (size_t) writtenSize,
                                                               packet.broadcastStopPacketForAllDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_ResetPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_ResetPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showResetPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                                 bufferSize - (size_t) writtenSize,
                                                                 packet.resetPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_HardResetPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_HardResetPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showHardResetPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                                     bufferSize - (size_t) writtenSize,
                                                                     packet.hardResetPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showFactoryTestInstructionPacketForMultiFunctionDecoders(
        buffer + writtenSize,
        bufferSize - (size_t) writtenSize,
        packet.factoryTestInstructionPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showSetDecoderFlagsPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                              bufferSize - (size_t) writtenSize,
                                                              packet.setDecoderFlagsPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showSetExtendedAddressingPacketForMultiFunctionDecoders(
        buffer + writtenSize,
        bufferSize - (size_t) writtenSize,
        packet.setExtendedAddressingPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag:
      writtenSize +=
        snprintf(buffer + writtenSize,
                 bufferSize - (size_t) writtenSize,
                 "{\"tag\":\"dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showDecoderAcknowledgementRequestPacket(buffer + writtenSize,
                                                    bufferSize - (size_t) writtenSize,
                                                    packet.decoderAcknowledgementRequestPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_ConsistControlPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_ConsistControlPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showConsistControlPacket(buffer + writtenSize,
                                                  bufferSize - (size_t) writtenSize,
                                                  packet.consistControlPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showSpeedStep128ControlPacket(buffer + writtenSize,
                                                       bufferSize - (size_t) writtenSize,
                                                       packet.speedStep128ControlPacket);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showRestrictedSpeedStepPacketForMultiFunctionDecoders(
        buffer + writtenSize,
        bufferSize - (size_t) writtenSize,
        packet.restrictedSpeedStepPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_AnalogFunctionPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_AnalogFunctionPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showAnalogFunctionPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                             bufferSize - (size_t) writtenSize,
                                                             packet.analogFunctionPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showSpeedAndDirectionPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                                bufferSize - (size_t) writtenSize,
                                                                packet.speedAndDirectionPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FunctionGroup1PacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FunctionGroup1PacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showFunctionGroup1PacketForMultiFunctionDecoders(buffer + writtenSize,
                                                             bufferSize - (size_t) writtenSize,
                                                             packet.functionGroup1PacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FunctionGroup2PacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FunctionGroup2PacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showFunctionGroup2PacketForMultiFunctionDecoders(buffer + writtenSize,
                                                             bufferSize - (size_t) writtenSize,
                                                             packet.functionGroup2PacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_BinaryStateControlLongFormPacketTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_BinaryStateControlLongFormPacketTag\",\"packet\":");
      writtenSize += dcc_showBinaryStateControlLongFormPacket(buffer + writtenSize,
                                                              bufferSize - (size_t) writtenSize,
                                                              packet.binaryStateControlLongFormPacket);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_BinaryStateControlShortFormPacketTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_BinaryStateControlShortFormPacketTag\",\"packet\":");
      writtenSize += dcc_showBinaryStateControlShortFormPacket(buffer + writtenSize,
                                                               bufferSize - (size_t) writtenSize,
                                                               packet.binaryStateControlShortFormPacket);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FunctionControlF13F20PacketTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FunctionControlF13F20PacketTag\",\"packet\":");
      writtenSize += dcc_showFunctionControlF13F20Packet(buffer + writtenSize,
                                                         bufferSize - (size_t) writtenSize,
                                                         packet.functionControlF13F20Packet);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FunctionControlF21F28PacketTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FunctionControlF21F28PacketTag\",\"packet\":");
      writtenSize += dcc_showFunctionControlF21F28Packet(buffer + writtenSize,
                                                         bufferSize - (size_t) writtenSize,
                                                         packet.functionControlF21F28Packet);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    default:
      return snprintf(buffer, bufferSize, "{\"tag\":\"Not implemented or unknown\"}");
  }
}
