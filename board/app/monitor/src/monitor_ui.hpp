#pragma once

#include <okdcc.h>
#include <stddef.h>
#include <stdint.h>

#include <okdcc/ui.hpp>

namespace monitor_ui {

// 画面からさかのぼって見られるパケットの数
constexpr size_t historyCapacity = 128;

// 一度に画面に並べるパケットの最大数
// 実際に見えるのは画面に収まる分だけで、あふれた分は切り取られる
constexpr size_t maxRows = 24;

constexpr size_t rowTextSize = 192;

// view が 1 フレームに使う文字列のメモリーの大きさ
constexpr size_t frameMemorySize = maxRows * rowTextSize + 256;

// `clay_lgfx::GpioButtons` が返すマスクのビット
constexpr uint8_t buttonA = 1 << 0;
constexpr uint8_t buttonB = 1 << 1;
constexpr uint8_t buttonC = 1 << 2;

struct Entry {
  unsigned long seq;
  struct dcc_Packet packet;
};

// 受信したパケットは連番 `seq` (1 始まり) を振り、`history[seq % historyCapacity]` に置く
struct Model {
  Entry history[historyCapacity];
  unsigned long packetsCount;
  unsigned long errorsCount;
  bool paused;
  // 停止中に画面のいちばん上に表示するパケットの連番
  unsigned long anchorSeq;
};

enum class MessageTag {
  PacketReceived,
  DecodeFailed,
  TogglePause,
  ScrollOlder,
  ScrollNewer,
};

struct Message {
  MessageTag tag;
  struct dcc_Packet packet;
};

Model init();

Model update(Model const &model, Message const &message);

void view(Model const &model, okdcc::ui::Frame &frame);

bool onButtonPressed(Model const &model, uint8_t button, Message *message);

using Program = okdcc::ui::Program<Model, Message>;

constexpr Program::Definition definition{ init, update, view, onButtonPressed };

// `dcc_showPacket` の JSON を画面用の短い文字列にする
void formatPacket(char *buffer, size_t bufferSize, unsigned long seq, struct dcc_Packet const &packet);

}  // namespace monitor_ui
