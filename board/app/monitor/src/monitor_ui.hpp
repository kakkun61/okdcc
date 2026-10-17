#pragma once

#include <okdcc.h>
#include <stddef.h>

namespace monitor_ui {

// 画面からさかのぼって見られるパケットの数
constexpr size_t historyCapacity = 128;

// 一度に画面に並べるパケットの最大数
// 実際に見えるのは画面に収まる分だけで、あふれた分は切り取られる
constexpr size_t maxRows = 24;

constexpr size_t rowTextSize = 192;

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

// 画面の各行の文字列を置く場所
struct ViewBuffers {
  char status[64];
  char rows[maxRows][rowTextSize];
};

// `Clay_BeginLayout` と `Clay_EndLayout` の間で呼ぶ
void view(Model const &model, ViewBuffers &buffers);

// `dcc_showPacket` の JSON を画面用の短い文字列にする
void formatPacket(char *buffer, size_t bufferSize, unsigned long seq, struct dcc_Packet const &packet);

}  // namespace monitor_ui
