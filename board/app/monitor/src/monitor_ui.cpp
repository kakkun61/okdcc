#include "monitor_ui.hpp"

#include <clay.h>
#include <stdio.h>
#include <string.h>

namespace monitor_ui {

namespace {

constexpr uint16_t bodyFontId = 0;
constexpr uint16_t headerFontId = 1;

constexpr Clay_Color backgroundColor{ 16, 20, 28, 255 };
constexpr Clay_Color barColor{ 40, 46, 60, 255 };
constexpr Clay_Color runningColor{ 80, 200, 120, 255 };
constexpr Clay_Color pausedColor{ 250, 200, 60, 255 };
constexpr Clay_Color textColor{ 230, 230, 230, 255 };
constexpr Clay_Color seqColor{ 120, 170, 230, 255 };
constexpr Clay_Color disabledColor{ 100, 100, 100, 255 };
constexpr Clay_Color rowColors[2] = { { 24, 28, 38, 255 }, { 30, 34, 46, 255 } };

unsigned long oldestSeq(Model const &model) {
  if (model.packetsCount == 0) return 1;
  return model.packetsCount > historyCapacity ? model.packetsCount - historyCapacity + 1 : 1;
}

Clay_String toClayString(char const *text) { return Clay_String{ false, static_cast<int32_t>(strlen(text)), text }; }

struct Replacement {
  char const *from;
  char const *to;
};

// タグ名から接頭辞・接尾辞と長い語を取り除く
void shortenTag(char *out, size_t outSize, char const *tag, size_t tagLength) {
  static Replacement const replacements[] = {
    { "PacketForLocomotiveDecoders", " (Loco)" },
    { "PacketForMultiFunctionDecoders", " (MF)" },
    { "PacketForAllDecoders", " (All)" },
    { "Packet", "" },
  };
  if (tagLength >= 4 && strncmp(tag, "dcc_", 4) == 0) {
    tag += 4;
    tagLength -= 4;
  }
  if (tagLength >= 3 && strncmp(tag + tagLength - 3, "Tag", 3) == 0) tagLength -= 3;
  size_t written = 0;
  size_t i = 0;
  while (i < tagLength && written + 1 < outSize) {
    bool replaced = false;
    for (Replacement const &replacement : replacements) {
      size_t const fromLength = strlen(replacement.from);
      if (fromLength <= tagLength - i && strncmp(tag + i, replacement.from, fromLength) == 0) {
        for (char const *c = replacement.to; *c != '\0' && written + 1 < outSize; c++) out[written++] = *c;
        i += fromLength;
        replaced = true;
        break;
      }
    }
    if (!replaced) out[written++] = tag[i++];
  }
  out[written] = '\0';
}

}  // namespace

void formatPacket(char *buffer, size_t bufferSize, unsigned long seq, struct dcc_Packet const &packet) {
  char json[256];
  dcc_showPacket(json, sizeof json, packet);

  char tag[64] = "?";
  {
    char const *const key = "\"tag\":\"";
    char const *const begin = strstr(json, key);
    if (begin != nullptr) {
      char const *const tagBegin = begin + strlen(key);
      char const *const tagEnd = strchr(tagBegin, '"');
      if (tagEnd != nullptr) shortenTag(tag, sizeof tag, tagBegin, static_cast<size_t>(tagEnd - tagBegin));
    }
  }

  int written = snprintf(buffer, bufferSize, "#%lu %s", seq, tag);
  if (written < 0 || static_cast<size_t>(written) >= bufferSize) return;

  // `"packet":{"address":3,"direction":"Forward"}` を ` address=3 direction=Forward` にする
  char const *const key = "\"packet\":";
  char const *body = strstr(json, key);
  if (body == nullptr) return;
  body += strlen(key);
  size_t position = static_cast<size_t>(written);
  if (position + 1 < bufferSize) buffer[position++] = ' ';
  for (char const *c = body; *c != '\0' && position + 1 < bufferSize; c++) {
    switch (*c) {
      case '"':
      case '{':
      case '}':
        break;
      case ',':
        buffer[position++] = ' ';
        break;
      case ':':
        buffer[position++] = '=';
        break;
      default:
        buffer[position++] = *c;
    }
  }
  buffer[position] = '\0';
}

Model init() {
  Model model{};
  model.packetsCount = 0;
  model.errorsCount = 0;
  model.paused = false;
  model.anchorSeq = 0;
  return model;
}

Model update(Model const &model, Message const &message) {
  Model next = model;
  switch (message.tag) {
    case MessageTag::PacketReceived:
      next.packetsCount++;
      next.history[next.packetsCount % historyCapacity] = Entry{ next.packetsCount, message.packet };
      // 停止中に見ていたパケットが新しいパケットに上書きされたら、残っている最も古いものを見る
      if (next.paused && next.anchorSeq < oldestSeq(next)) next.anchorSeq = oldestSeq(next);
      break;
    case MessageTag::DecodeFailed:
      next.errorsCount++;
      break;
    case MessageTag::TogglePause:
      next.paused = !next.paused;
      next.anchorSeq = next.packetsCount;
      break;
    case MessageTag::ScrollOlder:
      if (next.paused && next.anchorSeq > oldestSeq(next)) next.anchorSeq--;
      break;
    case MessageTag::ScrollNewer:
      if (next.paused && next.anchorSeq < next.packetsCount) next.anchorSeq++;
      break;
  }
  return next;
}

void view(Model const &model, ViewBuffers &buffers) {
  if (model.paused) {
    snprintf(buffers.status,
             sizeof buffers.status,
             "PAUSED  -%lu  pkt %lu  err %lu",
             model.packetsCount - model.anchorSeq,
             model.packetsCount,
             model.errorsCount);
  } else {
    snprintf(buffers.status, sizeof buffers.status, "RUNNING  pkt %lu  err %lu", model.packetsCount, model.errorsCount);
  }

  CLAY(CLAY_ID("Root"),
       { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .layoutDirection = CLAY_TOP_TO_BOTTOM },
         .backgroundColor = backgroundColor }) {
    CLAY(CLAY_ID("Status"),
         { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) }, .padding = { 6, 6, 4, 4 } },
           .backgroundColor = barColor }) {
      CLAY_TEXT(toClayString(buffers.status),
                CLAY_TEXT_CONFIG({ .textColor = model.paused ? pausedColor : runningColor, .fontId = headerFontId }));
    }

    // 新しいパケットほど上に並べる
    // あふれた古いパケットは下で切り取られる
    CLAY(CLAY_ID("Packets"),
         { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                       .padding = { 4, 4, 2, 2 },
                       .childGap = 1,
                       .layoutDirection = CLAY_TOP_TO_BOTTOM },
           .clip = { .vertical = true } }) {
      unsigned long const newest = model.paused ? model.anchorSeq : model.packetsCount;
      unsigned long const oldest = oldestSeq(model);
      size_t row = 0;
      for (unsigned long seq = newest; seq >= oldest && seq > 0 && row < maxRows; seq--, row++) {
        Entry const &entry = model.history[seq % historyCapacity];
        formatPacket(buffers.rows[row], rowTextSize, entry.seq, entry.packet);
        CLAY(CLAY_IDI("Row", static_cast<uint32_t>(row)),
             { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) }, .padding = { 2, 2, 1, 1 } },
               .backgroundColor = rowColors[seq % 2] }) {
          CLAY_TEXT(toClayString(buffers.rows[row]),
                    CLAY_TEXT_CONFIG({ .textColor = row == 0 ? seqColor : textColor, .fontId = bodyFontId }));
        }
      }
    }

    CLAY(CLAY_ID("Buttons"),
         { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) }, .padding = { 0, 0, 3, 3 } },
           .backgroundColor = barColor }) {
      char const *const labels[] = { "Older", model.paused ? "Resume" : "Pause", "Newer" };
      for (size_t i = 0; i < 3; i++) {
        bool const enabled = i == 1 || model.paused;
        CLAY(CLAY_IDI("Button", static_cast<uint32_t>(i)),
             { .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) },
                           .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } } }) {
          CLAY_TEXT(toClayString(labels[i]),
                    CLAY_TEXT_CONFIG({ .textColor = enabled ? textColor : disabledColor, .fontId = headerFontId }));
        }
      }
    }
  }
}

}  // namespace monitor_ui
