#include "ui.h"

#include <lvgl.h>

struct view {
  int a;
};

struct dcc_ui_Model_Command dcc_ui_init(lv_indev_t *buttonsIndev) {
  return (struct dcc_ui_Model_Command){ (struct dcc_ui_Model){
    .buttonsIndev = buttonsIndev, .tag = dcc_ui_MonitorModelTag, .model.monitorModel.packets = { 0 } } };
}

void packetList_cb(lv_event_t *event) {
  int value = (int) event->user_data;
  LV_LOG_USER("Packet %d", value);
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
  }
}

void dcc_ui_view(struct dcc_ui_Model model) {
  switch (model.tag) {
    case dcc_ui_MonitorModelTag: {
      lv_obj_t *screen = lv_screen_active();
      lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN_REVERSE);
      lv_obj_set_style_pad_all(screen, 6, 0);

      {
        lv_obj_t *buttonLabelsContainer = lv_obj_create(screen);
        lv_obj_set_size(buttonLabelsContainer, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_style_pad_top(buttonLabelsContainer, 4, 0);
        lv_obj_set_style_pad_bottom(buttonLabelsContainer, 4, 0);
        lv_obj_set_flex_flow(buttonLabelsContainer, LV_FLEX_FLOW_ROW);

        lv_obj_t *leftKeyLabel = lv_label_create(buttonLabelsContainer);
        lv_obj_set_flex_grow(leftKeyLabel, 1);
        lv_label_set_text(leftKeyLabel, "Down");
        lv_obj_set_style_text_align(leftKeyLabel, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *centerKeyLabel = lv_label_create(buttonLabelsContainer);
        lv_obj_set_flex_grow(centerKeyLabel, 1);
        lv_label_set_text(centerKeyLabel, "Up");
        lv_obj_set_style_text_align(centerKeyLabel, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *rightKeyLabel = lv_label_create(buttonLabelsContainer);
        lv_obj_set_flex_grow(rightKeyLabel, 1);
        lv_label_set_text(rightKeyLabel, "Enter");
        lv_obj_set_style_text_align(rightKeyLabel, LV_TEXT_ALIGN_CENTER, 0);
      }

      {
        lv_obj_t *packetList = lv_list_create(screen);
        lv_obj_set_width(packetList, lv_pct(100));
        lv_obj_set_flex_grow(packetList, 1);

        lv_group_t *group = lv_group_create();
        if (model.buttonsIndev != NULL) lv_indev_set_group(model.buttonsIndev, group);

        lv_obj_t *packet1Button = lv_list_add_button(packetList, NULL, "Packet 1");
        lv_obj_add_event_cb(packet1Button, packetList_cb, LV_EVENT_CLICKED, (void*) 1);
        lv_group_add_obj(group, packet1Button);
        lv_obj_t *packet2Button = lv_list_add_button(packetList, NULL, "Packet 2");
        lv_group_add_obj(group, packet2Button);
        lv_obj_t *packet3Button = lv_list_add_button(packetList, NULL, "Packet 3");
        lv_group_add_obj(group, packet3Button);
        lv_obj_t *packet4Button = lv_list_add_button(packetList, NULL, "Packet 4");
        lv_group_add_obj(group, packet4Button);
        lv_obj_t *packet5Button = lv_list_add_button(packetList, NULL, "Packet 5");
        lv_group_add_obj(group, packet5Button);
        lv_obj_t *packet6Button = lv_list_add_button(packetList, NULL, "Packet 6");
        lv_group_add_obj(group, packet6Button);
        lv_obj_t *packet7Button = lv_list_add_button(packetList, NULL, "Packet 7");
        lv_group_add_obj(group, packet7Button);
        lv_obj_t *packet8Button = lv_list_add_button(packetList, NULL, "Packet 8");
        lv_group_add_obj(group, packet8Button);
      }
    } break;
    case dcc_ui_SelectModelTag:
      break;
    case dcc_ui_ShowModelTag:
      break;
  }
}

struct dcc_ui_Command dcc_ui_update(struct dcc_ui_Model model, struct dcc_ui_Message message) {}
