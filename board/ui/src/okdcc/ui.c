#include "ui.h"

#include <lvgl.h>
#include <stdio.h>

static void updateStatus(struct dcc_ui_Model *const model) {
  lv_label_set_text_fmt(model->statusLabel,
                        "%s  packets: %lu  errors: %lu",
                        model->paused ? LV_SYMBOL_PAUSE " Paused" : LV_SYMBOL_PLAY " Running",
                        model->packetsCount,
                        model->errorsCount);
}

static void onClicked(lv_event_t *event) {
  struct dcc_ui_Model *const model = lv_event_get_user_data(event);
  dcc_ui_update(model, (struct dcc_ui_Message) { .tag = dcc_ui_PauseToggledMessageTag });
}

void dcc_ui_init(struct dcc_ui_Model *const model, lv_indev_t *buttonsIndev) {
  *model = (struct dcc_ui_Model) {
    .buttonsIndev = buttonsIndev,
    .packetsCount = 0,
    .errorsCount = 0,
    .paused = false,
  };

  lv_obj_t *screen = lv_screen_active();
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(screen, 6, 0);
  lv_obj_set_style_pad_row(screen, 4, 0);

  model->group = lv_group_create();
  if (buttonsIndev != NULL) lv_indev_set_group(buttonsIndev, model->group);

  {
    model->statusButton = lv_button_create(screen);
    lv_obj_set_width(model->statusButton, lv_pct(100));
    lv_obj_add_event_cb(model->statusButton, onClicked, LV_EVENT_CLICKED, model);
    lv_group_add_obj(model->group, model->statusButton);
    model->statusLabel = lv_label_create(model->statusButton);
  }

  {
    model->packetList = lv_list_create(screen);
    lv_obj_set_width(model->packetList, lv_pct(100));
    lv_obj_set_flex_grow(model->packetList, 1);
  }

  {
    lv_obj_t *buttonLabelsContainer = lv_obj_create(screen);
    lv_obj_set_size(buttonLabelsContainer, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(buttonLabelsContainer, 4, 0);
    lv_obj_set_style_pad_bottom(buttonLabelsContainer, 4, 0);
    lv_obj_set_flex_flow(buttonLabelsContainer, LV_FLEX_FLOW_ROW);

    char const *const texts[] = { "Down", "Up", "Pause" };
    for (size_t i = 0; i < sizeof texts / sizeof texts[0]; i++) {
      lv_obj_t *label = lv_label_create(buttonLabelsContainer);
      lv_obj_set_flex_grow(label, 1);
      lv_label_set_text(label, texts[i]);
      lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    }
  }

  updateStatus(model);
}

static void addPacket(struct dcc_ui_Model *const model, char const *const packetText) {
  if (DCC_UI_PACKETS_CAPACITY <= lv_obj_get_child_count(model->packetList)) {
    lv_obj_delete(lv_obj_get_child(model->packetList, 0));
  }
  char text[32 + 512];
  snprintf(text, sizeof text, "#%lu %s", model->packetsCount, packetText);
  lv_obj_t *button = lv_list_add_button(model->packetList, NULL, text);
  lv_obj_t *label = lv_obj_get_child(button, 0);
  if (label != NULL) lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_obj_add_event_cb(button, onClicked, LV_EVENT_CLICKED, model);
  lv_group_add_obj(model->group, button);
  lv_obj_scroll_to_view(button, LV_ANIM_OFF);
}

void dcc_ui_update(struct dcc_ui_Model *const model, struct dcc_ui_Message const message) {
  switch (message.tag) {
    case dcc_ui_PacketReceivedMessageTag:
      model->packetsCount++;
      if (!model->paused) addPacket(model, message.packetText);
      break;
    case dcc_ui_DecodeFailedMessageTag:
      model->errorsCount++;
      break;
    case dcc_ui_PauseToggledMessageTag:
      model->paused = !model->paused;
      break;
  }
  updateStatus(model);
}
