#ifndef DCC_UI_H
#define DCC_UI_H

#include <lvgl.h>
#include <stdbool.h>

/// \~english
/// \brief The maximum number of packets shown in the packet list.
///
/// \~japanese
/// \brief パケット一覧に表示するパケットの最大数。
#define DCC_UI_PACKETS_CAPACITY 50

/// \~english
/// \brief The state of the monitor UI.
///
/// \~japanese
/// \brief モニター UI の状態。
struct dcc_ui_Model {
  lv_indev_t *buttonsIndev;
  lv_group_t *group;
  lv_obj_t *statusButton;
  lv_obj_t *statusLabel;
  lv_obj_t *packetList;
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
    /// \~english
    /// \brief The text of the received packet. It is copied by `dcc_ui_update`.
    ///
    /// \~japanese
    /// \brief 受信したパケットの文字列。`dcc_ui_update` が複製する。
    char const *packetText;
  };
};

/// \~english
/// \brief Creates the screen of the monitor.
///
/// \~japanese
/// \brief モニターの画面を作る。
///
/// `model` は画面が存在する間は有効でなければならない。
void dcc_ui_init(struct dcc_ui_Model *const model, lv_indev_t *buttonsIndev);

/// \~english
/// \brief Updates the model and the screen by the message.
///
/// \~japanese
/// \brief メッセージによってモデルと画面を更新する。
void dcc_ui_update(struct dcc_ui_Model *const model, struct dcc_ui_Message const message);

#endif
