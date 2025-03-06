#ifndef DCC_LOGIC_H
#define DCC_LOGIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define DCC_BIT_STREAM_PARSER_BYTES_CAPACITY 8

/// \~english
/// \brief A type that represents the time in microseconds.
/// \~japanese
/// \brief マイクロ秒単位の時間を表す型。
typedef unsigned long dcc_TimeMicroSec;

/// \~english
/// \brief A type that represents a bit.
/// \~japanese
/// \brief ビットを表す型。
typedef bool dcc_Bit;

/// \~english
/// \brief A type that represents success or failure.
/// \~japanese
/// \brief 成功か失敗を表す型。
enum dcc_Result {
  dcc_Failure = 0,
  dcc_Success = 1,
};

/// \~english
/// \brief A type that represents the result of a stream parser.
/// \~japanese
/// \brief ストリームパーサーの結果を表す型。
enum dcc_StreamParserResult {
  dcc_StreamParserResult_Failure = 0,
  /// \~english
  /// \brief Parsing has not failed, but additional input is required to return a result.
  /// \~japanese
  /// \brief パースは失敗していないが結果を返すには追加の入力が要求されることを示す。
  dcc_StreamParserResult_Continue = 1,
  dcc_StreamParserResult_Success = 2,
};

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
/// \brief A type that represents a decoder's address.
/// \~japanese
/// \brief デコーダーのアドレスを表す型。
typedef uint16_t dcc_Address;

/// \~english
/// \brief A type that represents a consist address.
///
/// Only the lower 7 bits are used.
/// \~japanese
/// \brief 重連時のアドレスを表す型。
///
/// 下位7ビットしか使用しない。
typedef uint8_t dcc_ConsistAddress;

/// \~english
/// \brief A type that represents the direction of the locomotive.
/// \~japanese
/// \brief 機関車の進行方向を表す型。
enum dcc_Direction {
  dcc_Backward = 0,
  dcc_Forward = 1,
};

/// \~english
/// \brief A type that represents the speed of the locomotive.
///
/// Only the lower 5 bits are used.
/// \~japanese
/// \brief 機関車の速度を表す型。
///
/// 下位5ビットしか使用しない。
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

/// \~english
/// \brief The minimum value of the duration of a half bit of `1` sent.
/// \~japanese
/// \brief `1` の半ビットの転送継続時間の最小値。
extern dcc_TimeMicroSec const dcc_minOneHalfBitSentPeriod;

/// \~english
/// \brief The maximum value of the duration of a half bit of `1` sent.
/// \~japanese
/// \brief `1` の半ビットの転送継続時間の最大値。
extern dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriod;

/// \~english
/// \brief The minimum value of the acceptable duration of a half bit of `1` received.
/// \~japanese
/// \brief 受信した `1` の半ビットの許容時間の最小値。
extern dcc_TimeMicroSec const dcc_minOneHalfBitReceivedPeriod;

/// \~english
/// \brief The maximum value of the acceptable duration of a half bit of `1` received.
/// \~japanese
/// \brief 受信した `1` の半ビットの許容時間の最大値。
extern dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriod;

/// \~english
/// \brief The minimum value of the duration of a half bit of `0` sent.
/// \~japanese
/// \brief `0` の半ビットの転送継続時間の最小値。
extern dcc_TimeMicroSec const dcc_minZeroHalfBitSentPeriod;

/// \~english
/// \brief The maximum value of the duration of a half bit of `0` sent.
/// \~japanese
/// \brief `0` の半ビットの転送継続時間の最大値。
extern dcc_TimeMicroSec const dcc_maxZeroHalfBitSentPeriod;

/// \~english
/// \brief The maximum value of the total duration of stretched `0` bits.
/// \~japanese
/// \brief 引き伸ばされた `0` ビットの総和継続時間の最大値。
extern dcc_TimeMicroSec const dcc_maxStretchedZeroBitPeriod;

/// \~english
/// \brief The minimum value of the acceptable duration of a half bit of `0` received.
/// \~japanese
/// \brief 受信した `0` の半ビットの許容時間の最小値。
extern dcc_TimeMicroSec const dcc_minZeroHalfBitReceivedPeriod;

/// \~english
/// \brief The maximum value of the acceptable duration of a half bit of `0` received.
/// \~japanese
/// \brief 受信した `0` の半ビットの許容時間の最大値。
extern dcc_TimeMicroSec const dcc_maxZeroHalfBitReceivedPeriod;

/// \~english
/// \brief The maximum value of the difference in the durations of half bits of `1` sent.
/// \~japanese
/// \brief 転送した `1` の半ビットの時間の差の最大値。
extern dcc_TimeMicroSec const dcc_maxOneHalfBitSentPeriodDiff;

/// \~english
/// \brief The maximum value of the difference in the durations of half bits of `1` received.
/// \~japanese
/// \brief 受信した `1` の半ビットの時間の差の最大値。
extern dcc_TimeMicroSec const dcc_maxOneHalfBitReceivedPeriodDiff;

/// \~english
/// \brief To initialize a `SignalBuffer`.
/// \param array A pointer to the array used by the `SignalBuffer`.
/// \param size The number of elements in `array`.
/// \return The initialized `SignalBuffer`.
/// \~japanese
/// \brief `dcc_SignalBuffer` を初期化する。
/// \param array `dcc_SignalBuffer` が使う配列へのポインター。
/// \param size `array` の要素数。
/// \return 初期化された `dcc_SignalBuffer`
struct dcc_SignalBuffer dcc_initializeSignalBuffer(dcc_TimeMicroSec *array, size_t const size);

/// \~english
/// \brief To write the time of a voltage change to a `dcc_SignalBuffer`.
///
/// It is expected to be called within an interrupt handler.
///
/// \param buffer The `dcc_SignalBuffer` to write to.
/// \param signal The time of the voltage change.
/// \return Failure if there is no space left in the buffer, otherwise success.
/// \~japanese
/// \brief 電圧変化の時刻を `dcc_SignalBuffer` に書き込む。
///
/// 割り込みハンドラー内で呼び出すことが想定される。
///
/// \param buffer 書き込み先の `dcc_SignalBuffer`。
/// \param signal 電圧変化の時刻。
/// \return バッファに空きがない場合は失敗、それ以外は成功。
enum dcc_Result dcc_writeSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec const signal);

/// \~english
/// \brief To read the time of a voltage change written to a `dcc_SignalBuffer`.
/// \param buffer The `dcc_SignalBuffer` to read from.
/// \param signal The time of the voltage change read (output). If it fails, the value will not change.
/// \return Failure if the buffer is empty, otherwise success.
/// \~japanese
/// \brief 書き込まれた電圧変化の時刻を読み出す。
/// \param buffer 読み出し先の `dcc_SignalBuffer`。
/// \param signal 読み出された電圧変化の時刻（出力）。失敗した場合は値が変更されない。
/// \return バッファが空の場合は失敗、それ以外は成功。
enum dcc_Result dcc_readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal);

/// \~english
/// \brief To initialize a `dcc_SignalStreamParser`.
/// \return The initialized `dcc_SignalStreamParser`.
/// \~japanese
/// \brief `dcc_SignalStreamParser` を初期化する。
/// \return 初期化された `dcc_SignalStreamParser`。
struct dcc_SignalStreamParser dcc_initializeSignalStreamParser(void);

/// \~english
/// \brief To input the time of a voltage change to a `dcc_SignalStreamParser` and get a bit.
/// \param parser The place to store the state.
/// \param signal The time at which the line voltage changes.
/// \param bit The bit (output). If it is not successful, the value will not change.
/// \return Success or failure of the parsing.
/// \~japanese
/// \brief `dcc_SignalStreamParser` に電圧変化の時刻を入力し、ビットを取得する。
/// \param parser 状態を保持する場所。
/// \param signal 線路電圧の変化した時刻。
/// \param bit ビット（出力）。成功でない場合は値が変更されない。
/// \return パースの成否。
enum dcc_StreamParserResult dcc_feedSignal(struct dcc_SignalStreamParser *const parser, dcc_TimeMicroSec const signal,
                                           dcc_Bit *const bit);

/// \~english
/// \brief To initialize a `dcc_BitStreamParser`.
/// \return The initialized `dcc_BitStreamParser`.
/// \~japanese
/// \brief `dcc_BitStreamParser` を初期化する。
/// \return 初期化された `dcc_BitStreamParser`。
struct dcc_BitStreamParser dcc_initializeBitStreamParser(void);

/// \~english
/// \brief To input a bit to a `dcc_BitStreamParser` and get a byte.
///
/// The state of the `parser` is initialized when the result is `dcc_StreamParserResult_Failure`.
/// \param parser The place to store the state.
/// \param bit The bit.
/// \param bytes The byte (output). If it is not successful, the value will not change.
/// \param bytesSize The size of the byte (output). If it is not successful, the value will not change.
/// \return Success or failure of the parsing.
/// \~japanese
/// \brief `dcc_BitStreamParser` にビットを入力し、バイトを取得する。
///
/// 結果が `dcc_StreamParserResult_Failure` の場合、`parser` の状態は初期化される。
/// \param parser 状態を保持する場所。
/// \param bit ビット。
/// \param bytes バイト（出力）。成功でない場合は値が変更されない。
/// \param bytesSize バイトのサイズ（出力）。成功でない場合は値が変更されない。
/// \return パースの成否。
enum dcc_StreamParserResult dcc_feedBit(struct dcc_BitStreamParser *const parser, dcc_Bit const bit, uint8_t *const bytes,
                                        size_t *const bytesSize);

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

/// \~english
/// \brief To initialize a `dcc_Decoder`.
/// \param signalBufferValues A pointer to the array used by the `dcc_SignalBuffer`.
/// \param signalBufferSize The number of elements in `signalBufferValues`.
/// \return The initialized `dcc_Decoder`.
/// \~japanese
/// \brief `dcc_Decoder` を初期化する。
/// \param signalBufferValues `dcc_SignalBuffer` が使う配列へのポインター。
/// \param signalBufferSize `signalBufferValues` の要素数。
/// \return 初期化された `dcc_Decoder`。
struct dcc_Decoder dcc_initializeDecoder(dcc_TimeMicroSec *signalBufferValues, size_t const signalBufferSize);

/// \~english
/// \brief A function that serves as the main interface when used as a decoder.
/// \param decoder A place to store the state.
/// \param signal The time at which the line voltage changes.
/// \param packet The decoded packet (output).
/// \return Success or failure of the decoding.
/// \~japanese
/// \brief デコーダーとして使用するときのメインのインターフェースとなる関数。
/// \param decoder 状態を保持する場所。
/// \param signal 線路電圧の変化した時刻。
/// \param packet デコードされたパケット（出力）。
/// \return デコードの成否。
enum dcc_StreamParserResult dcc_decode(struct dcc_Decoder *const decoder, dcc_TimeMicroSec const signal,
                                       struct dcc_Packet *const packet);

int dcc_showSignalBuffer(char *buffer, size_t bufferSize, struct dcc_SignalBuffer const signalBuffer);

int dcc_showBytes(char *buffer, size_t bufferSize, uint8_t const *const bytes, size_t const bytesSize);

int dcc_showDirection(char *buffer, size_t bufferSize, enum dcc_Direction const direction);

int dcc_showSpeedAndDirectionPacket(char *buffer, size_t bufferSize, struct dcc_SpeedAndDirectionPacket const packet);

int dcc_showPacket(char *buffer, size_t bufferSize, struct dcc_Packet const packet);

/// \~english
/// \brief A pointer to a function called when an error occurs.
///
/// The function should output logs and exit or restart. `NULL` means that no function is called.
/// \~japanese
/// \brief エラー時に呼び出される関数へのポインター。
///
/// ログを出力し、終了や再起動もすべし。`NULL` にすると関数は呼び出されない。
extern void (*dcc_error_log)(char const *const file, int const line, char const *func, char const *format, ...);

/// \~english
/// \brief A pointer to a function called when debugging.
///
/// `NULL` means that no function is called.
/// \~japanese
/// \brief デバッグ時に呼び出される関数へのポインター。
///
/// `NULL` にすると関数は呼び出されない。
extern int (*dcc_debug_log)(char const *const file, int const line, char const *func, char const *format, ...);

#endif
