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
/// Only the lower 7 bits are used for baseline packets.
///
/// \~japanese
/// \brief デコーダーのアドレスを表す型。
///
/// 基本パケットでは下位7ビットしか使用しない。
typedef uint_least8_t dcc_AddressForBaselinePacket;

/// \~english
/// \brief A type that represents a decoder's address.
///
/// Only the lower 14 bits are used for extended packets.
///
/// \~japanese
/// \brief デコーダーのアドレスを表す型。
///
/// 下位14ビットしか使用しない。
typedef uint_least16_t dcc_AddressForExtendedPacket;

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
/// Only the lower 4 bits are used.
///
/// `0` represents stop, and the others represent the speed steps. `0b1111` is not used.
///
/// \~japanese
/// \brief 機関車の速度を表す型。
///
/// 下位4ビットしか使用しない。
///
/// `0` は停止を、それ以外は速度の段数を表す。`0b1111` は使用しない。
typedef uint_least8_t dcc_Speed4Bit;

/// \~english
/// \brief A type that represents the speed of the locomotive.
///
/// Only the lower 5 bits are used.
///
/// `0` represents stop, and the others represent the speed steps. `0b11100`-`0b11111` are not used.
///
/// \~japanese
/// \brief 機関車の速度を表す型。
///
/// 下位5ビットしか使用しない。
///
/// `0` は停止を、それ以外は速度の段数を表す。`0b11100`～`0b11111` は使用しない。
typedef uint_least8_t dcc_Speed5Bit;

/// \~english
///
/// \brief A type that represents the speed of the locomotive.
///
/// Only the lower 7 bits are used.
///
/// `0` represents stop, and the others represent the speed steps. `0b1111111` is not used.
///
/// \~japanese
/// \brief 機関車の速度を表す型。
///
/// 下位7ビットしか使用しない。
///
/// `0` は停止を、それ以外は速度の段数を表す。`0b1111111` は使用しない。
typedef uint_least8_t dcc_Speed7Bit;

/// \~english
/// See `dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag`.
///
/// \~japanese
/// `dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag` を参照。
struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders {
  dcc_AddressForBaselinePacket address;
  enum dcc_Direction direction;
  /// \~english
  /// \brief Which FL control is active within Speed And Direction Packet or not.
  ///
  /// When this value is `false`, `speed` is `speed5Bit`, and when this value is `true`, `speed` is `speed4Bit`. This value must be the same as “FL location” of [CV 29][spec-en-cv-29].
  ///
  /// [spec-en-cv-29]: http://kakkun61.com/nmra-ja/en/S-9.2.2-configuration-variables-for-dcc.html#cv-29-configurations-supported
  /// \~japanese
  /// \brief 速度・方向パケット内で FL 制御が有効かどうか。
  ///
  /// この値が `false` のとき `speed5Bit` が有効であり、この値が `true` のとき `speed4Bit` と `fl` が有効である。この値は [CV 29][spec-ja-cv-29] の “FL location” と同じでなければならない。
  ///
  /// [spec-ja-cv-29]: http://kakkun61.com/nmra-ja/ja/S-9.2.2-configuration-variables-for-dcc.html#cv-29-configurations-supported
  bool flControl;
  union {
    struct {
      dcc_Speed4Bit speed4Bit;
      bool fl;
    };
    struct {
      dcc_Speed5Bit speed5Bit;
      bool directionMayBeIgnored;
    };
  };
  bool emergencyStop;
};

// no structure for Digital Decoder Reset Packet For All Decoders

// no structure for Digital Decoder idle Packet For All Decoders

enum dcc_BroadcastStopKind {
  dcc_BroadcastStopKind_Stop = 0,
  dcc_BroadcastStopKind_Shutdown = 1,
};

/// \~english
/// See `dcc_BroadcastStopPacketForAllDecodersTag`.
///
/// \~japanese
/// `dcc_BroadcastStopPacketForAllDecodersTag` を参照。
struct dcc_BroadcastStopPacketForAllDecoders {
  enum dcc_BroadcastStopKind kind;
  bool directionMayBeIgnored;
  enum dcc_Direction direction;
};

/// \~english
/// See `dcc_ResetPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_ResetPacketForMultiFunctionDecodersTag` を参照。
struct dcc_ResetPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
};

/// \~english
/// See `dcc_HardResetPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_HardResetPacketForMultiFunctionDecodersTag` を参照。
struct dcc_HardResetPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
};

/// \~english
/// See `dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag` を参照。
struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  bool set;
  bool dataExists;
  dcc_Byte data;
};

enum dcc_DecoderFlagsInstruction {
  dcc_Disable111Instructions = 0,
  dcc_DisableDecoderAcknowledgementRequestInstruction = 4,
  dcc_ActivateBiDirectionalCommunications = 5,
  dcc_SetBiDirectionalCommunications = 8,
  dcc_Set111Instruction = 9,
  dcc_Accept111Instructions = 0xf
};

/// \~english
/// See `dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag` を参照。
struct dcc_SetDecoderFlagsPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  /// \~english
  /// If this instruction is sent to the consist address and when `set` is `0`, this instruction has effect only on the consist address, and when `1`, it has no effect.
  ///
  /// \~japanese
  /// 編成アドレスに送信された場合、`set` が `false` であればこの命令は編成アドレスにのみ効果があり、`true` であればこの命令は効果がない。
  bool set;
  /// \~english
  /// Only the lower 3 bits are used.
  ///
  /// \~japanese
  /// 下位3ビットしか使用しない。
  uint_least8_t subaddress;
  enum dcc_DecoderFlagsInstruction instruction;
};

/// \~english
/// See `dcc_AdvancedAddressingSetPacketTag`.
///
/// \~japanese
/// `dcc_AdvancedAddressingSetPacketTag` を参照。
struct dcc_SetExtendedAddressingPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  bool set;
};

/// \~english
/// See `dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag` を参照。
struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  bool set;
};

/// \~english
/// See `dcc_ConsistControlPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_ConsistControlPacketForMultiFunctionDecodersTag` を参照。
struct dcc_ConsistControlPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  enum dcc_Direction direction;
  /// \~english
  /// An address of the consists, but `0` means disabling of consists. Only the lower 7 bits are used.
  ///
  /// \~japanese
  /// 編成のアドレス、ただし `0` は編成の無効化を意味する。下位7ビットしか使用しない。
  dcc_ConsistAddress consistAddress;
};

/// \~english
/// See `dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag` を参照。
struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  enum dcc_Direction direction;
  dcc_Speed7Bit speed;
  bool emergencyStop;
};

/// \~english
/// See `dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag` を参照。
struct dcc_RestrictedSpeedStepPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  bool enabled;
  bool flControl;
  union {
    dcc_Speed4Bit speed4Bit;
    dcc_Speed5Bit speed5Bit;
  };
};

/// \~english
/// See `dcc_AnalogFunctionPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_AnalogFunctionPacketForMultiFunctionDecodersTag` を参照。
struct dcc_AnalogFunctionPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  dcc_Byte data;
};

/// \~english
/// See `dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag` を参照。
struct dcc_SpeedAndDirectionPacketForMultiFunctionDecoders {
  dcc_AddressForExtendedPacket address;
  enum dcc_Direction direction;
  /// \~english
  ///
  ///
  /// \~japanese
  /// `flControl` と `speed` に関しては `dcc_SpeedAndDirectionPacketForLocomotiveDecoders` と同様である。
  bool flControl;
  union {
    struct {
      dcc_Speed4Bit speed4Bit;
      bool fl;
    };
    dcc_Speed5Bit speed5Bit;
  };
  bool emergencyStop;
  bool directionMayBeIgnored;
};

/// \~english
/// See `dcc_FunctionGroup1PacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_FunctionGroup1PacketForMultiFunctionDecodersTag` を参照。
struct dcc_FunctionGroup1PacketForMultiFunctionDecoders {
  /// \~english
  /// \brief Which FL control is active within Function Group One Instruction Packet or not.
  ///
  /// When `false`, the value of `fl` is unspecified.
  ///
  /// \~japanese
  /// \brief 第1機能群命令パケット内で FL 制御が有効かどうか。
  ///
  /// `false` のとき `fl` の値は未規定である。
  bool flControl;
  bool fl;
  bool f1;
  bool f2;
  bool f3;
  bool f4;
};

enum dcc_FunctionGroup2Group {
  dcc_FunctionGroup2Group_F5_F8 = 1,
  dcc_FunctionGroup2Group_F9_F12 = 0,
};

/// \~english
/// See `dcc_FunctionGroup2PacketForMultiFunctionDecodersTag`.
///
/// \~japanese
/// `dcc_FunctionGroup2PacketForMultiFunctionDecodersTag` を参照。
struct dcc_FunctionGroup2PacketForMultiFunctionDecoders {
  enum dcc_FunctionGroup2Group group;
  union {
    struct {
      bool f5;
      bool f6;
      bool f7;
      bool f8;
    };
    struct {
      bool f9;
      bool f10;
      bool f11;
      bool f12;
    };
  } functions;
};

/// \~english
/// \brief A type that represents an address of a binary state.
///
/// Only the lower 15 bits are used.
///
/// `0` is an address that refers to all binary states. When sending (i.e., the command station), values from `1` to `0b1111111` (`127`) must not be used, and `dcc_BinaryStateAddressShortForm` must be used.
///
/// \~japanese
/// \brief 2値状態のアドレスを表す型。
///
/// 下位15ビットしか使用しない。
///
/// `0` は全ての2値状態を差すアドレスである。送信時は（つまりコマンドステーションは）`1` ～ `0b1111111`（`127`）の値を使用してはならず、`dcc_BinaryStateAddressShortForm` を使用しなければならない。
typedef uint_least16_t dcc_BinaryStateAddressLongForm;

/// \~english
/// \brief A type that represents an address of a binary state.
///
/// Only the lower 7 bits are used.
///
/// \~japanese
/// \brief 2値状態のアドレスを表す型。
///
/// 下位7ビットしか使用しない。
typedef uint_least8_t dcc_BinaryStateAddressShortForm;

/// \~english
/// See `dcc_BinaryStateControlLongFormPacketTag`.
///
/// \~japanese
/// `dcc_BinaryStateControlLongFormPacketTag` を参照。
struct dcc_BinaryStateControlLongFormPacket {
  dcc_AddressForExtendedPacket address;
  dcc_BinaryStateAddressLongForm stateAddress;
  bool state;
};

/// \~english
/// See `dcc_BinaryStateControlShortFormPacketTag`.
///
/// \~japanese
/// `dcc_BinaryStateControlShortFormPacketTag` を参照。
struct dcc_BinaryStateControlShortFormPacket {
  dcc_AddressForExtendedPacket address;
  dcc_BinaryStateAddressShortForm stateAddress;
  bool state;
};

/// \~english
/// See `dcc_FunctionControlF13F20PacketTag`.
///
/// \~japanese
/// `dcc_FunctionControlF13F20PacketTag` を参照。
struct dcc_FunctionControlF13F20Packet {
  dcc_AddressForExtendedPacket address;
  bool f13;
  bool f14;
  bool f15;
  bool f16;
  bool f17;
  bool f18;
  bool f19;
  bool f20;
};

/// \~english
/// See `dcc_FunctionControlF21F28PacketTag`.
///
/// \~japanese
/// `dcc_FunctionControlF21F28PacketTag` を参照。
struct dcc_FunctionControlF21F28Packet {
  dcc_AddressForExtendedPacket address;
  bool f21;
  bool f22;
  bool f23;
  bool f24;
  bool f25;
  bool f26;
  bool f27;
  bool f28;
};

enum dcc_PacketTag {
  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Speed and Direction Packet For Locomotive Decoders][spec-en-speed-and-direction-packet-for-locomotive-decoders]
  ///
  /// See `dcc_SpeedAndDirectionPacketForLocomotiveDecoders`.
  ///
  /// [spec-en-speed-and-direction-packet-for-locomotive-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#speed-and-direction-packet-for-locomotive-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 機関車デコーダー用速度・方向パケット][spec-ja-speed-and-direction-packet-for-locomotive-decoders]
  ///
  /// `dcc_SpeedAndDirectionPacketForLocomotiveDecoders` を参照。
  ///
  /// [spec-ja-speed-and-direction-packet-for-locomotive-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#speed-and-direction-packet-for-locomotive-decoders
  dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,

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
  dcc_ResetPacketForAllDecodersTag,

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
  dcc_IdlePacketForAllDecodersTag,

  /// \~english
  /// \brief [S-9.2 &gt; B: Baseline Packets &gt; Digital Decoder Broadcast Stop Packet for All Decoders][spec-en-digital-decoder-broadcast-stop-packets-for-all-decoders]
  ///
  /// See `dcc_BroadcastStopPacketForAllDecoders`.
  ///
  /// [spec-en-digital-decoder-broadcast-stop-packets-for-all-decoders]: https://kakkun61.com/nmra-ja/en/S-9.2-dcc-communications-standard.html#digital-decoder-broadcast-stop-packets-for-all-decoders
  ///
  /// \~japanese
  /// \brief [S-9.2 &gt; B：基本パケット &gt; 全デコーダー用デジタルデコーダーブロードキャスト停止パケット][spec-ja-digital-decoder-broadcast-stop-packets-for-all-decoders]
  ///
  /// `dcc_BroadcastStopPacketForAllDecoders` を参照。
  ///
  /// [spec-ja-digital-decoder-broadcast-stop-packets-for-all-decoders]: https://kakkun61.com/nmra-ja/ja/S-9.2-dcc-communications-standard.html#digital-decoder-broadcast-stop-packets-for-all-decoders
  dcc_BroadcastStopPacketForAllDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Digital Decoder Reset][spec-en-digital-decoder-reset]
  ///
  /// See `dcc_ResetPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-digital-decoder-reset]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#digital-decoder-reset
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; デジタルデコーダーリセット][spec-ja-digital-decoder-reset]
  ///
  /// `dcc_ResetPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-digital-decoder-reset]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#digital-decoder-reset
  dcc_ResetPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Hard Reset][spec-en-hard-reset]
  ///
  /// See `dcc_HardResetPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-hard-reset]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#herd-reset
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; 強制リセット][spec-ja-hard-reset]
  ///
  /// `dcc_HardResetPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-hard-reset]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#hard-reset
  dcc_HardResetPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Factory Test Instruction][spec-en-factory-test-instruction]
  ///
  /// See `dcc_FactoryTestInstructionPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-factory-test-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#factory-test-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; 工場試験命令][spec-ja-factory-test-instruction]
  ///
  /// `dcc_FactoryTestInstructionPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-factory-test-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#factory-test-instruction
  dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Set Decoder Flags][spec-en-set-decoder-flags]
  ///
  /// See `dcc_SetDecoderFlagsPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-set-decoder-flags]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#set-decoder-flags
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; デコーダーフラグの設定][spec-ja-set-decoder-flags]
  ///
  /// `dcc_SetDecoderFlagsPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-set-decoder-flags]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#set-decoder-flags
  dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Set Extended Address][spec-en-decoder-control]
  ///
  /// See `dcc_SetExtendedAddressingPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-decoder-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#decoder-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; 拡張アドレスの設定][spec-ja-decoder-control]
  ///
  /// `dcc_SetExtendedAddressingPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-decoder-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#decoder-control
  dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Decoder Control &gt; Decoder Acknowledgement Request][spec-en-decoder-control]
  ///
  /// See `dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-decoder-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#decoder-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; デコーダー制御 &gt; デコーダー応答要求][spec-ja-decoder-control]
  ///
  /// `dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-decoder-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#decoder-control
  dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Decoder and Consist Control Instruction &gt; Consist Control][spec-en-consist-control]
  ///
  /// See `dcc_ConsistControlPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-consist-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#consist-control
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; デコーダー制御命令と編成制御命令 &gt; 編成制御][spec-ja-consist-control]
  ///
  /// `dcc_ConsistControlPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-consist-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#consist-control
  dcc_ConsistControlPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; 128 Speed Step Control][spec-en-128-speed-step-control]
  ///
  /// See `dcc_SpeedStep128ControlPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-128-speed-step-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#128-speed-step-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 128段階速度制御][spec-ja-128-speed-step-control]
  ///
  /// `dcc_SpeedStep128ControlPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-128-speed-step-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#128-speed-step-control
  dcc_SpeedStep128ControlPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Restricted Speed Step Instruction][spec-en-restricted-speed-step-instruction]
  ///
  /// See `dcc_RestrictedSpeedStepPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-restricted-speed-step-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#restricted-speed-step-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 制限された速度制御命令][spec-ja-restricted-speed-step-instruction]
  ///
  /// `dcc_RestrictedSpeedStepPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-restricted-speed-step-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#restricted-speed-step-instruction
  dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Analog Function Group][spec-en-analog-function-group]
  ///
  /// See `dcc_AnalogFunctionPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-analog-function-group]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#analog-function-group
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; アナログ機能群][spec-ja-analog-function-group]
  ///
  /// `dcc_AnalogFunctionPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-analog-function-group]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#analog-function-group
  dcc_AnalogFunctionPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Speed and Direction Instructions][spec-en-speed-and-direction-instructions]
  ///
  /// See `dcc_SpeedAndDirectionPacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-speed-and-direction-instructions]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#speed-and-direction-instructions
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 速度方向命令][spec-ja-speed-and-direction-instructions]
  ///
  /// `dcc_SpeedAndDirectionPacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-speed-and-direction-instructions]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#speed-and-direction-instructions
  dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Function Group One Instruction][spec-en-function-group-one-instruction]
  ///
  /// See `dcc_FunctionGroup1PacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-function-group-one-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#function-group-one-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 第1機能群命令][spec-ja-function-group-one-instruction]
  ///
  /// `dcc_FunctionGroup1PacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-function-group-one-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#function-group-one-instruction
  dcc_FunctionGroup1PacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Function Group Two Instruction][spec-en-function-group-two-instruction]
  ///
  /// See `dcc_FunctionGroup2PacketForMultiFunctionDecoders`.
  ///
  /// [spec-en-function-group-two-instruction]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#function-group-two-instruction
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 第2機能群命令][spec-ja-function-group-two-instruction]
  ///
  /// `dcc_FunctionGroup2PacketForMultiFunctionDecoders` を参照。
  ///
  /// [spec-ja-function-group-two-instruction]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#function-group-two-instruction
  dcc_FunctionGroup2PacketForMultiFunctionDecodersTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Binary State Control Instruction - Long Form][spec-en-binary-state-control-instruction-long-form]
  ///
  /// See `dcc_BinaryStateControlLongFormPacket`.
  ///
  /// [spec-en-binary-state-control-instruction-long-form]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#binary-state-control-instruction-long-form
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 2値状態制御命令（長形式） ][spec-ja-binary-state-control-instruction-long-form]
  ///
  /// `dcc_BinaryStateControlLongFormPacket` を参照。
  ///
  /// [spec-ja-binary-state-control-instruction-long-form]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#binary-state-control-instruction-long-form
  dcc_BinaryStateControlLongFormPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; Binary State Control Instruction - Short Form][spec-en-binary-state-control-instruction-short-form]
  ///
  /// See `dcc_BinaryStateControlShortFormPacket`.
  ///
  /// [spec-en-binary-state-control-instruction-short-form]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#binary-state-control-instruction-short-form
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; 2値状態制御命令（短形式） ][spec-ja-binary-state-control-instruction-short-form]
  ///
  /// `dcc_BinaryStateControlShortFormPacket` を参照。
  ///
  /// [spec-ja-binary-state-control-instruction-short-form]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#binary-state-control-instruction-short-form
  dcc_BinaryStateControlShortFormPacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; F13-F20 Function Control][spec-en-f13-f20-function-control]
  ///
  /// See `dcc_FunctionControlF13F20Packet`.
  ///
  /// [spec-en-f13-f20-function-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#f13-f20-function-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; F13～F20 機能制御][spec-ja-f13-f20-function-control]
  ///
  /// `dcc_FunctionControlF13F20Packet` を参照。
  ///
  /// [spec-ja-f13-f20-function-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#f13-f20-function-control
  dcc_FunctionControlF13F20PacketTag,

  /// \~english
  /// \brief [S-9.2.1 &gt; C: Instruction Packets for Multi Function Digital Decoders &gt; Advanced Operations Instructions &gt; F21-F28 Function Control][spec-en-f21-f28-function-control]
  ///
  /// See `dcc_FunctionControlF21F28Packet`.
  ///
  /// [spec-en-f21-f28-function-control]: https://kakkun61.com/nmra-ja/en/S-9.2.1-extended-packet-formats.html#f21-f28-function-control
  ///
  /// \~japanese
  /// \brief [S-9.2.1 &gt; C：多機能デジタルデコーダー用命令パケット &gt; 発展的制御命令 &gt; F21～F28 機能制御][spec-ja-f21-f28-function-control]
  ///
  /// `dcc_FunctionControlF21F28Packet` を参照。
  ///
  /// [spec-ja-f21-f28-function-control]: https://kakkun61.com/nmra-ja/ja/S-9.2.1-extended-packet-formats.html#f21-f28-function-control
  dcc_FunctionControlF21F28PacketTag,

};

struct dcc_Packet {
  enum dcc_PacketTag tag;
  union {
    struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders speedAndDirectionPacketForLocomotiveDecoders;
    struct dcc_BroadcastStopPacketForAllDecoders broadcastStopPacketForAllDecoders;
    struct dcc_ResetPacketForMultiFunctionDecoders decoderResetPacketForMultiFunctionDecoders;
    struct dcc_HardResetPacketForMultiFunctionDecoders hardResetPacketForMultiFunctionDecoders;
    struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders
      factoryTestInstructionPacketForMultiFunctionDecoders;
    struct dcc_SetDecoderFlagsPacketForMultiFunctionDecoders setDecoderFlagsPacketForMultiFunctionDecoders;
    struct dcc_SetExtendedAddressingPacketForMultiFunctionDecoders setExtendedAddressingPacketForMultiFunctionDecoders;
    struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders
      decoderAcknowledgementRequestPacketForMultiFunctionDecoders;
    struct dcc_ConsistControlPacketForMultiFunctionDecoders consistControlPacketForMultiFunctionDecoders;
    struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders speedStep128ControlPacket;
  } packet;
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
enum dcc_StreamParserResult dcc_feedBit(struct dcc_BitStreamParser *const parser, dcc_Bit const bit,
                                        dcc_Byte *const bytes, size_t *const bytesSize);

enum dcc_Result dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize, bool flControl,
  struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders *const packet);

enum dcc_Result dcc_parseResetPacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseIdlePacketForAllDecoders(dcc_Byte const *const bytes, size_t const bytesSize);

enum dcc_Result dcc_parseBroadcastStopPacketForAllDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_BroadcastStopPacketForAllDecoders *const packet);

enum dcc_Result dcc_parseResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_ResetPacketForMultiFunctionDecoders *const packet);

enum dcc_Result dcc_parseHardResetPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_HardResetPacketForMultiFunctionDecoders *const packet);

enum dcc_Result dcc_parseDecoderAcknowledgementRequestPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_DecoderAcknowledgementRequestPacketForMultiFunctionDecoders *const packet);

enum dcc_Result dcc_parseFactoryTestInstructionPacketForMultiFunctionDecoders(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_FactoryTestInstructionPacketForMultiFunctionDecoders *const packet);

enum dcc_Result dcc_parseConsistControlPacket(dcc_Byte const *const bytes, size_t const bytesSize,
                                              struct dcc_ConsistControlPacketForMultiFunctionDecoders *const packet);

enum dcc_Result dcc_parseSpeedStep128ControlPacket(
  dcc_Byte const *const bytes, size_t const bytesSize,
  struct dcc_SpeedStep128ControlPacketForMultiFunctionDecoders *const packet);

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

int dcc_showSpeedAndDirectionPacketForLocomotiveDecoders(
  char *buffer, size_t bufferSize, struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders const packet);

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
