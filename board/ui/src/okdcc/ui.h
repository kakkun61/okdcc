#ifndef DCC_UI_H
#define DCC_UI_H

#include <lvgl.h>
#include <stdbool.h>
#include <stddef.h>

/// \~english
/// \brief The maximum number of packets shown in the packet list.
///
/// \~japanese
/// \brief パケット一覧に表示するパケットの最大数。
#define DCC_UI_PACKETS_CAPACITY 50

/// \~english
/// \brief The size of a packet text including the terminating null character.
///
/// \~japanese
/// \brief 終端ヌル文字を含むパケット文字列の大きさ。
#define DCC_UI_PACKET_TEXT_SIZE 512

/// \~english
/// \brief The state of the monitor UI.
///
/// \~japanese
/// \brief モニター UI の状態。
struct dcc_ui_Model {
  /// \~english
  /// \brief A ring buffer of the packet texts shown in the list. The `i`-th added text is at `i % DCC_UI_PACKETS_CAPACITY`.
  ///
  /// \~japanese
  /// \brief 一覧に表示するパケット文字列の循環バッファ。`i` 番目に追加された文字列は `i % DCC_UI_PACKETS_CAPACITY` にある。
  char packetTexts[DCC_UI_PACKETS_CAPACITY][DCC_UI_PACKET_TEXT_SIZE];
  /// \~english
  /// \brief The number of texts ever added to `packetTexts`.
  ///
  /// \~japanese
  /// \brief `packetTexts` にこれまでに追加された文字列の数。
  unsigned long addedPacketTextsCount;
  unsigned long packetsCount;
  unsigned long errorsCount;
  /// \~english
  /// \brief While paused, received packets are counted but not added to the list.
  ///
  /// \~japanese
  /// \brief 一時停止中は受信したパケットを数えるが一覧には追加しない。
  bool paused;
};

enum dcc_ui_MessageTag {
  dcc_ui_PacketReceivedMessageTag,
  dcc_ui_DecodeFailedMessageTag,
  dcc_ui_PauseToggledMessageTag,
};

struct dcc_ui_Message {
  enum dcc_ui_MessageTag tag;
  union {
    char const *packetText;
  };
};

enum dcc_ui_CommandTag {
  dcc_ui_NoneCommandTag,
};

struct dcc_ui_Command {
  enum dcc_ui_CommandTag tag;
};

struct dcc_ui_Program;

/// \~english
/// \brief The LVGL objects that show the model.
///
/// \~japanese
/// \brief モデルを表示する LVGL のオブジェクト。
struct dcc_ui_View {
  /// \~english
  /// \brief The program to which messages from the screen are sent.
  ///
  /// \~japanese
  /// \brief 画面からのメッセージを送るプログラム。
  struct dcc_ui_Program *program;
  lv_group_t *group;
  lv_obj_t *statusLabel;
  lv_obj_t *packetList;
  /// \~english
  /// \brief `addedPacketTextsCount` of the model last shown.
  ///
  /// \~japanese
  /// \brief 最後に表示したモデルの `addedPacketTextsCount`。
  unsigned long shownPacketTextsCount;
};

/// \~english
/// \brief The runtime that connects the model, `dcc_ui_update` and `dcc_ui_view`.
///
/// \~japanese
/// \brief モデルと `dcc_ui_update` と `dcc_ui_view` をつなぐランタイム。
struct dcc_ui_Program {
  struct dcc_ui_Model models[2];
  size_t currentModelIndex;
  struct dcc_ui_View view;
};

/// \~english
/// \brief Makes the initial model.
///
/// \~japanese
/// \brief 初期モデルを作る。
struct dcc_ui_Command dcc_ui_init(struct dcc_ui_Model *const model);

/// \~english
/// \brief Makes the next model from the model and the message. It does not modify `model`.
///
/// \~japanese
/// \brief モデルとメッセージから次のモデルを作る。`model` は変更しない。
struct dcc_ui_Command dcc_ui_update(struct dcc_ui_Model const *const model, struct dcc_ui_Message const message,
                                    struct dcc_ui_Model *const nextModel);

/// \~english
/// \brief Makes the screen show the model.
///
/// \~japanese
/// \brief 画面にモデルを表示する。
void dcc_ui_view(struct dcc_ui_View *const view, struct dcc_ui_Model const *const model);

/// \~english
/// \brief Starts the program. `program` must be valid while the screen exists.
///
/// \~japanese
/// \brief プログラムを開始する。`program` は画面が存在する間は有効でなければならない。
void dcc_ui_start(struct dcc_ui_Program *const program, lv_indev_t *buttonsIndev);

/// \~english
/// \brief Sends the message to the program.
///
/// \~japanese
/// \brief プログラムにメッセージを送る。
void dcc_ui_dispatch(struct dcc_ui_Program *const program, struct dcc_ui_Message const message);

#endif
