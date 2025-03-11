#ifndef DCC_LOGIC_H
#define DCC_LOGIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define DCC_BIT_STREAM_PARSER_BYTES_CAPACITY 8

/// \~english
/// \brief A type that represents the time in microseconds.
///
/// \~japanese
/// \brief マイクロ秒単位の時間を表す型。
typedef unsigned long dcc_TimeMicroSec;

/// \~english
/// \brief A type that represents a bit.
///
/// \~japanese
/// \brief ビットを表す型。
typedef bool dcc_Bit;

/// \~english
/// \brief A type that represents a byte.
///
/// \~japanese
/// \brief バイトを表す型。
typedef uint_least8_t dcc_Byte;

/// \~english
/// \brief A type that represents success or failure.
///
/// \~japanese
/// \brief 成功か失敗を表す型。
enum dcc_Result {
  dcc_Failure = 0,
  dcc_Success = 1,
};

/// \~english
/// \brief A type that represents the result of a stream parser.
///
/// \~japanese
/// \brief ストリームパーサーの結果を表す型。
enum dcc_StreamParserResult {
  dcc_StreamParserResult_Failure = 0,
  /// \~english
  /// \brief Parsing has not failed, but additional input is required to return a result.
  ///
  /// \~japanese
  /// \brief パースは失敗していないが結果を返すには追加の入力が要求されることを示す。
  dcc_StreamParserResult_Continue = 1,
  dcc_StreamParserResult_Success = 2,
};

/// \~english
/// \brief A structure that records the time of voltage changes.
///
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
///
/// \~japanese
/// \brief デコーダーのアドレスを表す型。
typedef uint16_t dcc_Address;

/// \~english
/// \brief A type that represents a consist address.
///
/// Only the lower 7 bits are used.
///
/// \~japanese
/// \brief 重連時のアドレスを表す型。
///
/// 下位7ビットしか使用しない。
typedef uint_least8_t dcc_ConsistAddress;

/// \~english
/// \brief A type that represents the direction of the locomotive.
///
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
///
/// \~japanese
/// \brief 機関車の速度を表す型。
///
/// 下位5ビットしか使用しない。
typedef uint_least8_t dcc_Speed;

/// \~english
/// See `dcc_SpeedAndDirectionPacketTag`.
///
/// \~japanese
/// `dcc_SpeedAndDirectionPacketTag` を参照。
struct dcc_SpeedAndDirectionPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  dcc_Speed speed;
};

// no structure for Digital Decoder Reset Packet For All Decoders

// no structure for Digital Decoder idle Packet For All Decoders

/// \~english
/// See `dcc_AllDecoderBroadcastStopPacketTag`.
///
/// \~japanese
/// `dcc_AllDecoderBroadcastStopPacketTag` を参照。
struct dcc_AllDecoderBroadcastStopPacket {
  /// \~english
  /// Only the lower 1 bit is used. Upon `0`, digital decoders intended to control a locomotive's motor shall bring the locomotive to a stop. Upon `1`, digital decoders intended to control a locomotive's motor shall stop delivering energy to the motor.
  ///
  /// \~japanese
  /// 下位1ビットしか使用しない。`0` の場合、機関車のモーターを制御することを意図したデジタルデコーダーは機関車を停止させなければいけない。`1` の場合、機関車のモーターを制御することを意図したデジタルデコーダーはモーターへのエネルギー供給を停止させなければいけない。
  uint_least8_t kind;
  /// \~english
  /// Only the lower 1 bit is used. If `1`, the `direction` may optionally be ignored for all direction sensitive functions.
  ///
  /// \~japanese
  /// 下位1ビットしか使用しない。`1` の場合、`direction` は方向に影響する機能に対して任意に無視することができる。
  uint_least8_t ignoreDirection;
  enum dcc_Direction direction;
};

/// \~english
/// See `dcc_DecoderResetPacketTag`.
///
/// \~japanese
/// `dcc_DecoderResetPacketTag` を参照。
struct dcc_DecoderResetPacket {
  dcc_Address address;
};

/// \~english
/// See `dcc_HardResetPacketTag`.
///
/// \~japanese
/// `dcc_HardResetPacketTag` を参照。
struct dcc_HardResetPacket {
  dcc_Address address;
};

/// \~english
/// See `dcc_FactoryTestInstructionPacketTag`.
///
/// \~japanese
/// `dcc_FactoryTestInstructionPacketTag` を参照。
struct dcc_FactoryTestInstructionPacket {
  dcc_Address address;
  dcc_Byte data[2];
  size_t dataSize;
};

enum dcc_DecoderFlagsInstruction {
  dcc_Disable111Instructions = 0,
  dcc_DisableDecoderAcknowledgementRequestInstruction = 4,
  dcc_ActivateBiDirectionalCommunications = 5,
  dcc_setBiDirectionalCommunications = 8,
  dcc_Set111Instruction = 9,
  dcc_Accept111Instructions = 0xf
};

/// \~english
/// See `dcc_DecoderFlagsSetPacketTag`.
///
/// \~japanese
/// `dcc_DecoderFlagsSetPacketTag` を参照。
struct dcc_DecoderFlagsSetPacket {
  dcc_Address address;
  /// \~english
  /// Only the lower 3 bits are used.
  ///
  /// \~japanese
  /// 下位3ビットしか使用しない。
  uint_least8_t subaddress;
  enum dcc_DecoderFlagsInstruction instruction;
  /// \~english
  /// If this instruction is sent to the consist address and when `effect` is `0`, this instruction has effect only on the consist address, and when `1`, it has no effect.
  ///
  /// \~japanese
  /// 編成アドレスに送信された場合、`effect` が `0` であればこの命令は編成アドレスにのみ効果があり、`1` であればこの命令は効果がない。
  dcc_Bit effect;
};

/// \~english
/// See `dcc_ConsistControlPacketTag`.
///
/// \~japanese
/// `dcc_ConsistControlPacketTag` を参照。
struct dcc_ConsistControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  /// \~english
  /// An address of the consists, but `0` means disabling of consists. Only the lower 7 bits are used.
  ///
  /// \~japanese
  /// 編成のアドレス、ただし `0` は編成の無効化を意味する。下位7ビットしか使用しない。
  dcc_ConsistAddress consistAddress;
};

/// \~english
/// See `dcc_SpeedStep128ControlPacketTag`.
///
/// \~japanese
/// `dcc_SpeedStep128ControlPacketTag` を参照。
struct dcc_SpeedStep128ControlPacket {
  dcc_Address address;
  enum dcc_Direction direction;
  bool emergencyStop;
  dcc_Speed speed;
};

/// \~english
/// See `dcc_RestrictedSpeedStepPacketTag`.
///
/// \~japanese
/// `dcc_RestrictedSpeedStepPacketTag` を参照。
struct dcc_RestrictedSpeedStepPacket {
  dcc_Address address;
  bool enabled;
  /// \~english
  /// Only the lower 6 bits are used.
  ///
  /// \~japanese
  /// 下位6ビットしか使用しない。
  dcc_Speed speed;
};

/// \~english
/// See `dcc_AnalogFunctionPacketTag`.
///
/// \~japanese
/// `dcc_AnalogFunctionPacketTag` を参照。
struct AnalogFunctionPacket {
  dcc_Address address;
  dcc_Byte data;
};




struct dcc_AdvancedAddressingSetPacket {
  // 未実装
  void *unimplemented;
};

struct dcc_DecoderAcknowledgementRequestPacket {
  dcc_Address address;
};

enum dcc_PacketTag {
  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Speed and Direction Packet For Locomotive Decoders][spec-en-speed-and-direction-packet-for-locomotive-decoders]
  ///
  /// See `dcc_SpeedAndDirectionPacket`.
  ///
  /// [spec-en-speed-and-direction-packet-for-locomotive-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#speed-and-direction-packet-for-locomotive-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 機関車デコーダー用速度・方向パケット][spec-ja-speed-and-direction-packet-for-locomotive-decoders]
  ///
  /// `dcc_SpeedAndDirectionPacket` を参照。
  ///
  /// [spec-ja-speed-and-direction-packet-for-locomotive-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#speed-and-direction-packet-for-locomotive-decoders
  dcc_SpeedAndDirectionPacketTag,

  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Speed and Direction Packet For Locomotive Decoders][spec-en-digital-decoder-reset-packet-for-all-decoders]
  ///
  /// There is no corresponding structure because it has no fields.
  ///
  /// [spec-en-digital-decoder-reset-packet-for-all-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#digital-decoder-reset-packet-for-all-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 全デコーダー用デジタルデコーダーリセットパケット][spec-ja-digital-decoder-reset-packet-for-all-decoders]
  ///
  /// フィールドが存在しないため対応する構造体はない。
  ///
  /// [spec-ja-digital-decoder-reset-packet-for-all-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#digital-decoder-reset-packet-for-all-decoders
  dcc_AllDecoderResetPacketTag,

  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Digital Decoder idle Packet For All Decoders][spec-en-digital-decoder-idle-packet-for-all-decoders]
  ///
  /// There is no corresponding structure because it has no fields.
  ///
  /// [spec-en-digital-decoder-idle-packet-for-all-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#digital-decoder-idle-packet-for-all-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 全デコーダー用デジタルデコーダーアイドルパケット][spec-ja-digital-decoder-idle-packet-for-all-decoders]
  ///
  /// フィールドが存在しないため対応する構造体はない。
  ///
  /// [spec-ja-digital-decoder-idle-packet-for-all-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#digital-decoder-idle-packet-for-all-decoders
  dcc_AllDecoderIdlePacketTag,

  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Digital Decoder Broadcast Stop Packet for All Decoders][spec-en-digital-decoder-broadcast-stop-packets-for-all-decoders]
  ///
  /// See `dcc_AllDecoderBroadcastStopPacket`.
  ///
  /// [spec-en-digital-decoder-broadcast-stop-packets-for-all-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#digital-decoder-broadcast-stop-packets-for-all-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 全デコーダー用デジタルデコーダーブロードキャスト停止パケット][spec-ja-digital-decoder-broadcast-stop-packets-for-all-decoders]
  ///
  /// `dcc_AllDecoderBroadcastStopPacket` を参照。
  ///
  /// [spec-ja-digital-decoder-broadcast-stop-packets-for-all-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#digital-decoder-broadcast-stop-packets-for-all-decoders
  dcc_AllDecoderBroadcastStopPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Digital Decoder Reset][spec-en-digital-decoder-reset]
  ///
  /// There is no corresponding structure because it has no fields.
  ///
  /// [spec-en-digital-decoder-reset]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#digital-decoder-reset
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; デジタルデコーダーリセット][spec-ja-digital-decoder-reset]
  ///
  /// フィールドが存在しないため対応する構造体はない。
  ///
  /// [spec-ja-digital-decoder-reset]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#digital-decoder-reset
  dcc_DecoderResetPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Hard Reset][spec-en-hard-reset]
  ///
  /// There is no corresponding structure because it has no fields.
  ///
  /// [spec-en-hard-reset]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#herd-reset
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; 強制リセット][spec-ja-hard-reset]
  ///
  /// フィールドが存在しないため対応する構造体はない。
  ///
  /// [spec-ja-hard-reset]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#hard-reset
  dcc_HardResetPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Factory Test Instruction][spec-en-factory-test-instruction]
  ///
  /// See `dcc_FactoryTestInstructionPacket`.
  ///
  /// [spec-en-factory-test-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#factory-test-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; 工場試験命令][spec-ja-factory-test-instruction]
  ///
  /// `dcc_FactoryTestInstructionPacket` を参照。
  ///
  /// [spec-ja-factory-test-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#factory-test-instruction
  dcc_FactoryTestInstructionPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Set Decoder Flags][spec-en-set-decoder-flags]
  ///
  /// See `dcc_DecoderFlagsSetPacket`.
  ///
  /// [spec-en-set-decoder-flags]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#set-decoder-flags
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; デコーダーフラグの設定][spec-ja-set-decoder-flags]
  ///
  /// `dcc_DecoderFlagsSetPacket` を参照。
  ///
  /// [spec-ja-set-decoder-flags]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#set-decoder-flags
  dcc_DecoderFlagsSetPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Consist Control][spec-en-consist-control]
  ///
  /// See `dcc_ConsistControlPacket`.
  ///
  /// [spec-en-consist-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#consist-control
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; 編成制御][spec-ja-consist-control]
  ///
  /// `dcc_ConsistControlPacket` を参照。
  ///
  /// [spec-ja-consist-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#consist-control
  dcc_ConsistControlPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; 128 Speed Step Control][spec-en-128-speed-step-control]
  ///
  /// See `dcc_SpeedStep128ControlPacket`.
  ///
  /// [spec-en-128-speed-step-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#128-speed-step-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 128段階速度制御][spec-ja-128-speed-step-control]
  ///
  /// `dcc_SpeedStep128ControlPacket` を参照。
  ///
  /// [spec-ja-128-speed-step-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#128-speed-step-control
  dcc_SpeedStep128ControlPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Restricted Speed Step Instruction][spec-en-restricted-speed-step-instruction]
  ///
  /// See `dcc_RestrictedSpeedStepPacket`.
  ///
  /// [spec-en-restricted-speed-step-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#restricted-speed-step-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 制限された速度制御命令][spec-ja-restricted-speed-step-instruction]
  ///
  /// `dcc_RestrictedSpeedStepPacket` を参照。
  ///
  /// [spec-ja-restricted-speed-step-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#restricted-speed-step-instruction
  dcc_RestrictedSpeedStepPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Analog Function Group][spec-en-analog-function-group]
  ///
  /// See `AnalogFunctionPacket`.
  ///
  /// [spec-en-analog-function-group]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#analog-function-group
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; アナログ機能群][spec-ja-analog-function-group]
  ///
  /// `AnalogFunctionPacket` を参照。
  ///
  /// [spec-ja-analog-function-group]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#analog-function-group
  dcc_AnalogFunctionPacketTag,




  dcc_AdvancedAddressingSetPacketTag,
  dcc_DecoderAcknowledgementRequestPacketTag,
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
      dcc_Byte byte;
      size_t bitCount;
    } inByte;
  };
  dcc_Byte bytes[DCC_BIT_STREAM_PARSER_BYTES_CAPACITY];
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
enum dcc_StreamParserResult dcc_feedBit(struct dcc_BitStreamParser *const parser, dcc_Bit const bit, dcc_Byte *const bytes,
                                        size_t *const bytesSize);

enum dcc_Result dcc_parseSpeedAndDirectionPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                 struct dcc_SpeedAndDirectionPacket *const packet);

enum dcc_Result dcc_parseAllDecoderResetPacket(dcc_Byte const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseAllDecoderIdlePacket(dcc_Byte const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseDecoderResetPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                            struct dcc_DecoderResetPacket *const packet);

enum dcc_Result dcc_parseHardResetPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                         struct dcc_HardResetPacket *const packet);

enum dcc_Result dcc_parseDecoderAcknowledgementRequestPacket(
  dcc_Byte const *const bytes, size_t const bytesSize, struct dcc_DecoderAcknowledgementRequestPacket *const packet);

enum dcc_Result dcc_parseFactoryTestInstructionPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                      struct dcc_FactoryTestInstructionPacket *const packet);

enum dcc_Result dcc_parseConsistControlPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                              struct dcc_ConsistControlPacket *const packet);

enum dcc_Result dcc_parseSpeedStep128ControlPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                                   struct dcc_SpeedStep128ControlPacket *const packet);

enum dcc_Result dcc_parsePacket(dcc_Byte const *const bytes, size_t const bytesSize, struct dcc_Packet *const packet);

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

int dcc_showBytes(char *buffer, size_t bufferSize, dcc_Byte const *const bytes, size_t const bytesSize);

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
