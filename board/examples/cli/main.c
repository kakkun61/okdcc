#include <okdcc/logic.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 1024
#define SIGNAL_BUFFER_SIZE 256
#define LOG_BUFFER_SIZE 1024

#define LOG(...)                                             \
  {                                                          \
    fprintf(stderr, "LOG  : %s (%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                            \
    fprintf(stderr, "\n");                                   \
  }

int debug_log(char const *const file, int const line, char const *func, char const *format, ...) {
  fprintf(stderr, "DEBUG: %s (%d) %s: ", file, line, func);
  va_list args;
  va_start(args, format);
  int const result = vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  return result;
}

void error_log(char const *const file, int const line, char const *func, char const *format, ...) {
  fprintf(stderr, "ERROR: %s (%d) %s:", file, line, func);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

char *readTimeMicroSec(char *line, dcc_TimeMicroSec *const timeMicroSec) {
  int readCharsCount;
  if (1 != sscanf(line, "%lu%n", timeMicroSec, &readCharsCount)) return NULL;
  return line + readCharsCount;
}

int main(void) {
  dcc_debug_log = debug_log;
  dcc_error_log = error_log;
  char line[LINE_SIZE];
  dcc_TimeMicroSec signalBufferValues[SIGNAL_BUFFER_SIZE];
  char logBuffer[LOG_BUFFER_SIZE] = { 0 };
  struct dcc_Decoder decoder = dcc_initializeDecoder(signalBufferValues, SIGNAL_BUFFER_SIZE);
  while (0 < printf("signals: ") && NULL != fgets(line, LINE_SIZE, stdin) && strlen(line) > 1) {
    {
      // ここは、マイコンに実装する場合は割り込みで実行される部分
      LOG("interrupted");
      dcc_TimeMicroSec timeMicroSec;
      char *head = line;
      while (NULL != (head = readTimeMicroSec(head, &timeMicroSec))) {
        // 割り込まれるたびに `dcc_writeSignalBuffer` を呼び出す
        if (dcc_Failure == dcc_writeSignalBuffer(&decoder.signalBuffer, timeMicroSec)) {
          LOG("write signal buffer error");
        }
      }
    }
    {
      // ここは、マイコンに実装する場合はメインループで実行される部分
      LOG("main loop");
      dcc_TimeMicroSec signal;
      while (dcc_Success == dcc_readSignalBuffer(&decoder.signalBuffer, &signal)) {
        struct dcc_Packet packet;
        enum dcc_StreamParserResult result = dcc_decode(&decoder, signal, &packet);
        switch (result) {
          case dcc_StreamParserResult_Failure:
            LOG("decode error");
            continue;
          case dcc_StreamParserResult_Continue:
            LOG("decode continue");
            continue;
          case dcc_StreamParserResult_Success:
            dcc_showPacket(logBuffer, LOG_BUFFER_SIZE, packet);
            LOG("packet: %s", logBuffer);
            continue;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
