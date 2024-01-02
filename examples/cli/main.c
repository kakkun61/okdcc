#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcc.h"

size_t const lineSize = 1024;
size_t const signalBufferSize = 256;
size_t const signalsSize = 256;
size_t const bitsSize = 32 * 16;
size_t const logBufferSize = 1024;
size_t const bytesSize = 128;

#define LOG(...)                                      \
  {                                                   \
    fprintf(stderr, "%s (%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                     \
    fprintf(stderr, "\n");                            \
  }

int debug_log(char const *format, ...) {
  fprintf(stderr, "%s (%d): ", __FILE__, __LINE__);
  va_list args;
  va_start(args, format);
  int const result = vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  return result;
}

char *readTimeMicroSec(char *line, dcc_TimeMicroSec *const timeMicroSec) {
  int readCharsCount;
  if (1 != sscanf(line, "%lu%n", timeMicroSec, &readCharsCount)) return NULL;
  return line + readCharsCount;
}

int main(void) {
  dcc_debug_log = debug_log;
  char line[lineSize];
  enum dcc_Result result;
  dcc_TimeMicroSec signalBufferValues[signalBufferSize];
  struct dcc_SignalBuffer signalBuffer = dcc_initializeSignalBuffer(signalBufferValues, signalBufferSize);
  dcc_TimeMicroSec signals[signalsSize] = { 0 };
  size_t leftSignalsSize = 0;
  dcc_Bits32 bits[bitsSize / 32];
  size_t leftBitsSize = 0;
  uint8_t bytes[bytesSize];
  size_t leftBytesSize = 0;
  char logBuffer[logBufferSize] = { 0 };
  while (0 < printf("signals: ") && NULL != fgets(line, lineSize, stdin) && strlen(line) > 1) {
    {
      // ここは、マイコンに実装する場合は割り込みで実行される部分
      LOG("interrupted");
      dcc_TimeMicroSec timeMicroSec;
      char *head = line;
      while (NULL != (head = readTimeMicroSec(head, &timeMicroSec))) {
        // 割り込まれるたびに `dcc_writeSignalBuffer` を呼び出す
        if (dcc_Failure == dcc_writeSignalBuffer(&signalBuffer, timeMicroSec)) {
          LOG("write signal buffer error");
        }
        dcc_showSignalBuffer(logBuffer, logBufferSize, signalBuffer);
        LOG("signal buffer: %s", logBuffer);
      }
    }
    {
      // ここは、マイコンに実装する場合はメインループで実行される部分
      LOG("main loop");
      dcc_showSignalBuffer(logBuffer, logBufferSize, signalBuffer);
      LOG("signal buffer: %s", logBuffer);
      size_t readSize;
      dcc_readsSignalBuffer(&signalBuffer, signals + leftSignalsSize, signalsSize - leftSignalsSize, &readSize);
      LOG("read size: %lu", readSize);
      for (size_t i = leftSignalsSize; i < leftSignalsSize + readSize; i++) {
        LOG("copied signal: %lu", signals[i]);
      }
      // 電圧変化がビットの初めなのか、ビットの中間なのかを判断する
      size_t decodedSingalsSize;
      size_t writtenBitsSize;
      if (dcc_Failure == dcc_decodeSignals(signals, leftSignalsSize + readSize, &decodedSingalsSize, bits, leftBitsSize,
                                           bitsSize, &writtenBitsSize)) {
        LOG("read signals size: %lu", decodedSingalsSize);
        LOG("written bits size: %lu", writtenBitsSize);
        // デコードでエラーになった
        LOG("decode 1st trial error");
        if (dcc_Failure == dcc_decodeSignals(signals, leftSignalsSize + readSize, &decodedSingalsSize, bits,
                                             leftBitsSize + 1, bitsSize, &writtenBitsSize)) {
          LOG("read signals size: %lu", decodedSingalsSize);
          LOG("written bits size: %lu", writtenBitsSize);
          LOG("decode 2nd trial error");
        }
      }
      LOG("read signals size: %lu", decodedSingalsSize);
      LOG("written bits size: %lu", writtenBitsSize);
      size_t const newLeftSignalsSize = leftSignalsSize + readSize - decodedSingalsSize;
      if (0 < newLeftSignalsSize)
        memmove(signals, signals + decodedSingalsSize, newLeftSignalsSize * sizeof(dcc_TimeMicroSec));
      leftSignalsSize = newLeftSignalsSize;
      leftBitsSize = leftBitsSize + writtenBitsSize;
      for (size_t i = 0; i < leftBitsSize; i++) {
        LOG("decoded bit: %d: %d", i, dcc_getBit(bits, i));
      }
      size_t next;
      if (dcc_Failure == dcc_consumeThroughPreamble(bits, 0, leftBitsSize, &next)) {
        LOG("consume through preamble error");
        continue;
      }
      LOG("consume through preamble success");
      LOG("next: %lu", next);
      size_t writtenBytesSize;
      if (dcc_Failure == dcc_consumePacket(bits, next, leftBitsSize, bytes, bytesSize, &writtenBytesSize, &next)) {
        LOG("consume packet error");
        continue;
      }
      dcc_showBytes(logBuffer, logBufferSize, bytes, writtenBytesSize);
      LOG("packet: %s", logBuffer);
      bool valid = dcc_validatePacket(bytes, writtenBytesSize - 1, bytes[writtenBytesSize - 1]);
      LOG("valid: %s", valid ? "true" : "false");
      struct dcc_Packet packet;
      if (dcc_Failure == dcc_parsePacket(bytes, writtenBytesSize, &packet)) {
        LOG("parse packet error");
        continue;
      }
      dcc_showPacket(logBuffer, logBufferSize, packet);
      LOG("packet: %s", logBuffer);
      leftBitsSize = leftBitsSize - next;
      LOG("left bits size: %d", leftBitsSize);
    }
  }
  return EXIT_SUCCESS;
}
