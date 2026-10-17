#ifndef OKDCC_UI_HPP
#define OKDCC_UI_HPP

#include <clay.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace okdcc::ui {

/// \~english
/// \brief Memory for the strings a view hands to Clay during a frame.
///
/// Clay does not copy strings, so they have to live until the frame is drawn. The memory is reused every frame.
///
/// \~japanese
/// \brief view がフレームの間に Clay に渡す文字列のためのメモリー。
///
/// Clay は文字列をコピーしないので、フレームが描画されるまで残っている必要がある。メモリーはフレームごとに使い回す。
class Frame {
 public:
  Frame(char *const buffer, size_t const size) : buffer(buffer), size(size) {}

  /// \~english
  /// \brief Discards every string of the previous frame.
  ///
  /// \~japanese
  /// \brief 前のフレームの文字列をすべて捨てる。
  void reset() { used = 0; }

  /// \~english
  /// \brief Allocates `length` bytes plus a terminating null character. Returns `nullptr` when the memory runs out.
  ///
  /// \~japanese
  /// \brief `length` バイトと終端ヌル文字の分を確保する。メモリーが足りないときは `nullptr` を返す。
  char *allocate(size_t const length) {
    if (size - used < length + 1) return nullptr;
    char *const result = buffer + used;
    result[0] = '\0';
    used += length + 1;
    return result;
  }

  /// \~english
  /// \brief Makes a Clay string from a null terminated string that lives until the frame is drawn.
  ///
  /// \~japanese
  /// \brief フレームが描画されるまで残っているヌル終端文字列から Clay の文字列を作る。
  static Clay_String string(char const *const text) {
    if (text == nullptr) return Clay_String{ false, 0, "" };
    return Clay_String{ false, static_cast<int32_t>(strlen(text)), text };
  }

  /// \~english
  /// \brief Formats a string like `printf` into the frame memory. Text that does not fit is cut off.
  ///
  /// \~japanese
  /// \brief `printf` のように文字列を書式化してフレームのメモリーに置く。収まらない分は切り捨てる。
  Clay_String format(char const *const format, ...) __attribute__((format(printf, 2, 3))) {
    size_t const available = size - used;
    if (available == 0) return string(nullptr);
    char *const result = buffer + used;
    va_list args;
    va_start(args, format);
    int const length = vsnprintf(result, available, format, args);
    va_end(args);
    if (length < 0) return string(nullptr);
    size_t const written = static_cast<size_t>(length) < available ? static_cast<size_t>(length) : available - 1;
    used += written + 1;
    return Clay_String{ false, static_cast<int32_t>(written), result };
  }

 private:
  char *const buffer;
  size_t const size;
  size_t used = 0;
};

/// \~english
/// \brief Runs an application in The Elm Architecture style on Clay.
///
/// `update` makes the next model without modifying the current one, and `view` lays the model out with `CLAY(...)`.
/// Drawing the render commands `frame` returns is up to the caller.
///
/// \~japanese
/// \brief Clay の上で The Elm Architecture 風のアプリケーションを動かす。
///
/// `update` は現在のモデルを変更せずに次のモデルを作り、`view` はモデルを `CLAY(...)` でレイアウトする。
/// `frame` が返す描画コマンドを描くのは呼び出し側に任せる。
template <typename Model, typename Message>
class Program {
 public:
  struct Definition {
    Model (*init)();
    Model (*update)(Model const &model, Message const &message);
    void (*view)(Model const &model, Frame &frame);
    /// \~english
    /// \brief Makes a message from a button that has just been pressed. Returns false when there is none. May be
    /// `nullptr`.
    ///
    /// \~japanese
    /// \brief 押されたばかりのボタンからメッセージを作る。メッセージが無いときは false を返す。`nullptr` でもよい。
    bool (*onButtonPressed)(Model const &model, uint8_t button, Message *message);
  };

  Program(Definition const &definition, Frame &frame) : definition(definition), frame_(frame), model_() {}

  void start() {
    model_ = definition.init();
    heldButtons = 0;
  }

  void dispatch(Message const &message) { model_ = definition.update(model_, message); }

  /// \~english
  /// \brief Takes the mask of buttons held down now and dispatches messages for the ones just pressed.
  ///
  /// \~japanese
  /// \brief いま押されているボタンのマスクを受け取り、押されたばかりのボタンのメッセージを送る。
  void handleButtons(uint8_t const held) {
    uint8_t const pressed = held & static_cast<uint8_t>(~heldButtons);
    heldButtons = held;
    if (definition.onButtonPressed == nullptr) return;
    for (unsigned bit = 0; bit < 8; bit++) {
      uint8_t const button = static_cast<uint8_t>(1u << bit);
      if ((pressed & button) == 0) continue;
      Message message;
      if (definition.onButtonPressed(model_, button, &message)) dispatch(message);
    }
  }

  /// \~english
  /// \brief Lays out the current model and returns the render commands.
  ///
  /// \~japanese
  /// \brief 現在のモデルをレイアウトし、描画コマンドを返す。
  Clay_RenderCommandArray frame(float const deltaTime) {
    frame_.reset();
    Clay_BeginLayout();
    definition.view(model_, frame_);
    return Clay_EndLayout(deltaTime);
  }

  Model const &model() const { return model_; }

 private:
  Definition const definition;
  Frame &frame_;
  Model model_;
  uint8_t heldButtons = 0;
};

}  // namespace okdcc::ui

#endif
