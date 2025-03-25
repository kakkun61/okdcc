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
    exit(EXIT_FAILURE);                                                                  \
  } while (0)

#define DCC_DEBUG_LOG(...) (dcc_debug_log == NULL ? 0 : dcc_debug_log(__FILE__, __LINE__, __func__, __VA_ARGS__))

#define DCC_UNREACHABLE(...) DCC_ERROR_LOG("unreachable: "__VA_ARGS__)

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
  return (struct dcc_SignalBuffer){ .buffer = buffer, .size = size, .written = 0, .writeIndex = 0, .readIndex = 0 };
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
  return (struct dcc_SignalStreamParser){ { 0 }, 0 };
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
      DCC_UNREACHABLE("signalsSize: %d", parser->signalsSize);
  }
}

struct dcc_BitStreamParser dcc_initializeBitStreamParser(void) {
  return (struct dcc_BitStreamParser){
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

enum dcc_Result dcc_validatePacket(dcc_Byte const *const bytes, size_t bytesSize, dcc_Byte const checksum) {
  dcc_Byte sum = 0;
  for (size_t i = 0; i < bytesSize; i++) sum ^= bytes[i];
  if (sum == checksum) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parsePacket(dcc_Byte const *const bytes, size_t const bytesSize, struct dcc_Packet *const packet) {
  if (dcc_Success == dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(
                       bytes,
                       bytesSize,
                       false,  // TODO CV の実装ができたら書き換える
                       &packet->packet.speedAndDirectionPacketForLocomotiveDecoders)) {
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
      dcc_parseResetPacketForMultiFunctionDecoders(bytes,
                                                   bytesSize,
                                                   &packet->packet.decoderResetPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_ResetPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseHardResetPacketForMultiFunctionDecoders(bytes,
                                                       bytesSize,
                                                       &packet->packet.hardResetPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_HardResetPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseDecoderAcknowledgementRequestPacket(
                       bytes,
                       bytesSize,
                       &packet->packet.decoderAcknowledgementRequestPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseFactoryTestInstructionPacketForMultiFunctionDecoders(
                       bytes,
                       bytesSize,
                       &packet->packet.factoryTestInstructionPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success ==
      dcc_parseConsistControlPacket(bytes, bytesSize, &packet->packet.consistControlPacketForMultiFunctionDecoders)) {
    packet->tag = dcc_ConsistControlPacketForMultiFunctionDecodersTag;
    return dcc_Success;
  }
  if (dcc_Success == dcc_parseSpeedStep128ControlPacket(bytes, bytesSize, &packet->packet.speedStep128ControlPacket)) {
    packet->tag = dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag;
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
  if (bytesSize < 3) return dcc_Failure;
  if ((bytes[1] & 0xC0) != 0x40) return dcc_Failure;  // check packet identifier
  if (bytes[0] & 0x80) return dcc_Failure;
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
  if (bytesSize < 3) return dcc_Failure;
  if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseIdlePacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize) {
  if (bytesSize < 3) return dcc_Failure;
  if (bytes[0] == 0xFF && bytes[1] == 0 && bytes[2] == 0xFF) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseBroadcastStopPacketForAllDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_BroadcastStopPacketForAllDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  if (bytes[0] != 0 || (bytes[1] & 0xCE) != 0x40) return dcc_Failure;
  packet->direction = bytes[1] & 0x20 ? dcc_Forward : dcc_Backward;
  packet->directionMayBeIgnored = (bool)bytes[1] & 0x10;
  packet->kind = (enum dcc_BroadcastStopKind) (bytes[1] & 1);
  return dcc_Success;
}

static enum dcc_Result parseAddressForExtendedPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                     dcc_AddressForExtendedPacket *const address,
                                                     size_t *const addressSize) {
  if (bytesSize < 1) return dcc_Failure;
  if ((bytes[0] & 0xC0) == 0xC0 && bytes[0] != 0xFF) {
    if (bytesSize < 2) return dcc_Failure;
    *address = (dcc_AddressForExtendedPacket) ((bytes[0] & 0x3F) << 8 | bytes[1]);
    *addressSize = 2;
    return dcc_Success;
  }
  *address = bytes[0];
  *addressSize = 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_ResetPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 1) return dcc_Failure;
  if (bytes[addressSize] == 0) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseHardResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_HardResetPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 1) return dcc_Failure;
  if (bytes[addressSize] == 1) return dcc_Success;
  return dcc_Failure;
}

enum dcc_Result dcc_parseFactoryTestInstructionPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if ((bytes[addressSize] & 0xFE) != 2) return dcc_Failure;
  packet->set = bytes[addressSize] & 1;
  if (addressSize + 1 == bytesSize) {
    packet->dataExists = false;
    return dcc_Success;
  }
  packet->dataExists = true;
  packet->data = bytes[addressSize + 1];
  return dcc_Success;
}

enum dcc_Result dcc_parseDecoderFlagsSetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SetDecoderFlagsPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if ((bytes[addressSize] & 0xFE) != 6) return dcc_Failure;
  packet->set = bytes[addressSize] & 1;
  packet->subaddress = bytes[addressSize + 1] & 7;
  uint_least8_t const instruction = bytes[addressSize + 1] >> 4;
  if (instruction != dcc_Disable111Instructions && instruction != dcc_DisableDecoderAcknowledgementRequestInstruction &&
      instruction != dcc_ActivateBiDirectionalCommunications && instruction != dcc_SetBiDirectionalCommunications &&
      instruction != dcc_Set111Instruction && instruction != dcc_Accept111Instructions) {
    return dcc_Failure;
  }
  packet->instruction = (enum dcc_DecoderFlagsInstruction) instruction;
  return dcc_Success;
}

enum dcc_Result dcc_parseSetAdvancedAddressingPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SetExtendedAddressingPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 2) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 1) return dcc_Failure;
  if ((bytes[addressSize] & 0xFE) != 0xA) return dcc_Failure;
  packet->set = bytes[addressSize] & 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseDecoderAcknowledgementRequestPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 1) return dcc_Failure;
  if ((bytes[addressSize] & 0xFE) != 0xE) return dcc_Failure;
  packet->set = bytes[addressSize] & 1;
  return dcc_Success;
}

enum dcc_Result dcc_parseConsistControlPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                              struct dcc_ConsistControlPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 2) return dcc_Failure;
  if ((bytes[addressSize] & 0xF0) != 0x10) return dcc_Failure;
  switch (bytes[addressSize] & 0x0F) {
    case 2:
      packet->direction = dcc_Forward;
      break;
    case 3:
      packet->direction = dcc_Backward;
      break;
    default:
      return dcc_Failure;
  }
  packet->consistAddress = (dcc_ConsistAddress) (bytes[addressSize + 1] & 0x7F);
  return dcc_Success;
}

enum dcc_Result dcc_parseSpeedStep128ControlPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders *const packet) {
  if (bytesSize < 3) return dcc_Failure;
  size_t addressSize;
  if (dcc_Failure == parseAddressForExtendedPacket(bytes, bytesSize, &packet->address, &addressSize)) {
    return dcc_Failure;
  }
  if (bytesSize < addressSize + 2) return dcc_Failure;
  if ((bytes[addressSize] & 0xFF) != 0x3F) return dcc_Failure;
  if ((bytes[addressSize + 1] & 0x80) == 0) packet->direction = dcc_Backward;
  else packet->direction = dcc_Forward;
  dcc_Speed7Bit const speed = (dcc_Speed7Bit) (bytes[addressSize + 1] & 0x7F);
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

struct dcc_Decoder dcc_initializeDecoder(dcc_TimeMicroSec *signalBufferValues, size_t const signalBufferSize) {
  return (struct dcc_Decoder){ .signalBuffer = dcc_initializeSignalBuffer(signalBufferValues, signalBufferSize),
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

int dcc_showDirection(char *buffer, size_t const bufferSize, enum dcc_Direction const direction) {
  switch (direction) {
    case dcc_Forward:
      return snprintf(buffer, bufferSize, "Forward");
    case dcc_Backward:
      return snprintf(buffer, bufferSize, "Backward");
    default:
      return snprintf(buffer, bufferSize, "Unknown");
  }
}

int dcc_showBroadcastStopKind(char *buffer, size_t const bufferSize, enum dcc_BroadcastStopKind const kind) {
  switch (kind) {
    case dcc_BroadcastStopKind_Stop:
      return snprintf(buffer, bufferSize, "Stop");
    case dcc_BroadcastStopKind_Shutdown:
      return snprintf(buffer, bufferSize, "Shutdown");
    default:
      return snprintf(buffer, bufferSize, "Unknown");
  }
}

int dcc_showSpeedAndDirectionPacketForLocomotiveDecoders(
  char *buffer, size_t const bufferSize, struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders const packet) {
  int writtenSize = 0;
  writtenSize +=
    snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "{\"address\":%d,\"direction\":", packet.address);
  writtenSize += dcc_showDirection(buffer + writtenSize, bufferSize - (size_t) writtenSize, packet.direction);
  writtenSize +=
    snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"flControl\":%d", packet.flControl);
  if (packet.flControl) {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed4Bit\":%d", packet.speed4Bit);
    writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"fl\":%d", packet.fl);
  } else {
    writtenSize +=
      snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"speed5Bit\":%d", packet.speed5Bit);
    writtenSize += snprintf(buffer + writtenSize,
                            bufferSize - (size_t) writtenSize,
                            ",\"directionMayBeIgnored\":%d",
                            packet.directionMayBeIgnored);
  }
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          ",\"emergencyStop\":%s}",
                          packet.emergencyStop ? "true" : "false");
  return writtenSize;
}

int dcc_showBroadcastStopPacketForAllDecoders(
  char *buffer, size_t const bufferSize, struct dcc_BroadcastStopPacketForAllDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "{\"kind\":");
  writtenSize += dcc_showBroadcastStopKind(buffer + writtenSize, bufferSize - (size_t) writtenSize, packet.kind);
  writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"directionMayBeIgnored\":%s,\"direction\":",
                          packet.directionMayBeIgnored ? "true" : "false");
  writtenSize += dcc_showDirection(buffer + writtenSize, bufferSize - (size_t) writtenSize, packet.direction);
  writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
  return writtenSize;
}

int dcc_showResetPacketForMultiFunctionDecoders(char *buffer, size_t const bufferSize,
                               struct dcc_ResetPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showHardResetPacket(char *buffer, size_t const bufferSize,
                            struct dcc_HardResetPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showFactoryTestInstructionPacket(char *buffer, size_t const bufferSize,
                                         struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          "{\"address\":%d,\"set\":%d,\"dataExists\":%d",
                          packet.address,
                          packet.set,
                          packet.dataExists);
  if (packet.dataExists)
    writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"data\":%#X", packet.data);
  writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
  return writtenSize;
}

int dcc_showDecoderAcknowledgementRequestPacket(
  char *buffer, size_t const bufferSize,
  struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders const packet) {
  return snprintf(buffer, bufferSize, "{\"address\":%d}", packet.address);
}

int dcc_showConsistControlPacket(char *buffer, size_t const bufferSize,
                                 struct dcc_ConsistControlPacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize +=
    snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "{\"address\":%d,\"direction\":", packet.address);
  writtenSize += dcc_showDirection(buffer + writtenSize, bufferSize - (size_t) writtenSize, packet.direction);
  writtenSize +=
    snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, ",\"consistAddress\":%d}", packet.consistAddress);
  return writtenSize;
}

int dcc_showSpeedStep128ControlPacket(char *buffer, size_t const bufferSize,
                                      struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders const packet) {
  int writtenSize = 0;
  writtenSize +=
    snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "{\"address\":%d,\"direction\":", packet.address);
  writtenSize += dcc_showDirection(buffer + writtenSize, bufferSize - (size_t) writtenSize, packet.direction);
  writtenSize += snprintf(buffer + writtenSize,
                          bufferSize - (size_t) writtenSize,
                          ",\"emergencyStop\":%s,\"speed\":%d}",
                          packet.emergencyStop ? "true" : "false",
                          packet.speed);
  return writtenSize;
}

int dcc_showPacket(char *buffer, size_t const bufferSize, struct dcc_Packet const packet) {
  int writtenSize = 0;
  switch (packet.tag) {
    case dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag\",\"packet\":");
      writtenSize += dcc_showSpeedAndDirectionPacketForLocomotiveDecoders(
        buffer + writtenSize,
        bufferSize - (size_t) writtenSize,
        packet.packet.speedAndDirectionPacketForLocomotiveDecoders);
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
                                                               packet.packet.broadcastStopPacketForAllDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_ResetPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_ResetPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showResetPacketForMultiFunctionDecoders(buffer + writtenSize,
                                                bufferSize - (size_t) writtenSize,
                                                packet.packet.decoderResetPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_HardResetPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_HardResetPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showHardResetPacket(buffer + writtenSize,
                                             bufferSize - (size_t) writtenSize,
                                             packet.packet.hardResetPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize +=
        dcc_showFactoryTestInstructionPacket(buffer + writtenSize,
                                             bufferSize - (size_t) writtenSize,
                                             packet.packet.factoryTestInstructionPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag:
      // 未実装
      return snprintf(buffer, bufferSize, "{\"tag\":\"dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag\"}");
    case dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag:
      // 未実装
      return snprintf(buffer, bufferSize, "{\"tag\":\"dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag\"}");
    case dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag:
      writtenSize +=
        snprintf(buffer + writtenSize,
                 bufferSize - (size_t) writtenSize,
                 "{\"tag\":\"dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showDecoderAcknowledgementRequestPacket(
        buffer + writtenSize,
        bufferSize - (size_t) writtenSize,
        packet.packet.decoderAcknowledgementRequestPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_ConsistControlPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_ConsistControlPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showConsistControlPacket(buffer + writtenSize,
                                                  bufferSize - (size_t) writtenSize,
                                                  packet.packet.consistControlPacketForMultiFunctionDecoders);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    case dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag:
      writtenSize += snprintf(buffer + writtenSize,
                              bufferSize - (size_t) writtenSize,
                              "{\"tag\":\"dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag\",\"packet\":");
      writtenSize += dcc_showSpeedStep128ControlPacket(buffer + writtenSize,
                                                       bufferSize - (size_t) writtenSize,
                                                       packet.packet.speedStep128ControlPacket);
      writtenSize += snprintf(buffer + writtenSize, bufferSize - (size_t) writtenSize, "}");
      return writtenSize;
    default:
      return snprintf(buffer, bufferSize, "{\"tag\":\"Not implemented or unknown\"}");
  }
}
