#include "ui.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

static void onClicked(lv_event_t *event) {
  struct dcc_ui_Program *const program = lv_event_get_user_data(event);
  dcc_ui_dispatch(program, (struct dcc_ui_Message) { .tag = dcc_ui_PauseToggledMessageTag, .packetText = NULL });
}

struct dcc_ui_Command dcc_ui_init(struct dcc_ui_Model *const model) {
  model->addedPacketTextsCount = 0;
  model->packetsCount = 0;
  model->errorsCount = 0;
  model->paused = false;
  return (struct dcc_ui_Command) { .tag = dcc_ui_NoneCommandTag };
}

struct dcc_ui_Command dcc_ui_update(struct dcc_ui_Model const *const model, struct dcc_ui_Message const message,
                                    struct dcc_ui_Model *const nextModel) {
  *nextModel = *model;
  switch (message.tag) {
    case dcc_ui_PacketReceivedMessageTag:
      nextModel->packetsCount++;
      if (!nextModel->paused) {
        snprintf(nextModel->packetTexts[nextModel->addedPacketTextsCount % DCC_UI_PACKETS_CAPACITY],
                 DCC_UI_PACKET_TEXT_SIZE,
                 "#%lu %s",
                 nextModel->packetsCount,
                 message.packetText);
        nextModel->addedPacketTextsCount++;
      }
      break;
    case dcc_ui_DecodeFailedMessageTag:
      nextModel->errorsCount++;
      break;
    case dcc_ui_PauseToggledMessageTag:
      nextModel->paused = !nextModel->paused;
      break;
  }
  return (struct dcc_ui_Command) { .tag = dcc_ui_NoneCommandTag };
}

void dcc_ui_view(struct dcc_ui_View *const view, struct dcc_ui_Model const *const model) {
  lv_label_set_text_fmt(view->statusLabel,
                        "%s  packets: %lu  errors: %lu",
                        model->paused ? LV_SYMBOL_PAUSE " Paused" : LV_SYMBOL_PLAY " Running",
                        model->packetsCount,
                        model->errorsCount);

  unsigned long const added = model->addedPacketTextsCount;
  unsigned long first = view->shownPacketTextsCount;
  if (added - first > DCC_UI_PACKETS_CAPACITY) first = added - DCC_UI_PACKETS_CAPACITY;
  lv_obj_t *lastButton = NULL;
  for (unsigned long i = first; i < added; i++) {
    lastButton = lv_list_add_button(view->packetList, NULL, model->packetTexts[i % DCC_UI_PACKETS_CAPACITY]);
    lv_obj_t *label = lv_obj_get_child(lastButton, 0);
    if (label != NULL) lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_add_event_cb(lastButton, onClicked, LV_EVENT_CLICKED, view->program);
    lv_group_add_obj(view->group, lastButton);
  }
  while (DCC_UI_PACKETS_CAPACITY < lv_obj_get_child_count(view->packetList)) {
    lv_obj_delete(lv_obj_get_child(view->packetList, 0));
  }
  if (lastButton != NULL) lv_obj_scroll_to_view(lastButton, LV_ANIM_OFF);
  view->shownPacketTextsCount = added;
}

static void createView(struct dcc_ui_Program *const program, lv_indev_t *buttonsIndev) {
  struct dcc_ui_View *const view = &program->view;
  view->program = program;
  view->shownPacketTextsCount = 0;

  lv_obj_t *screen = lv_screen_active();
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(screen, 6, 0);
  lv_obj_set_style_pad_row(screen, 4, 0);

  view->group = lv_group_create();
  if (buttonsIndev != NULL) lv_indev_set_group(buttonsIndev, view->group);

  {
    lv_obj_t *statusButton = lv_button_create(screen);
    lv_obj_set_width(statusButton, lv_pct(100));
    lv_obj_add_event_cb(statusButton, onClicked, LV_EVENT_CLICKED, program);
    lv_group_add_obj(view->group, statusButton);
    view->statusLabel = lv_label_create(statusButton);
  }

  {
    view->packetList = lv_list_create(screen);
    lv_obj_set_width(view->packetList, lv_pct(100));
    lv_obj_set_flex_grow(view->packetList, 1);
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
}

void dcc_ui_start(struct dcc_ui_Program *const program, lv_indev_t *buttonsIndev) {
  program->currentModelIndex = 0;
  dcc_ui_init(&program->models[0]);
  createView(program, buttonsIndev);
  dcc_ui_view(&program->view, &program->models[0]);
}

void dcc_ui_dispatch(struct dcc_ui_Program *const program, struct dcc_ui_Message const message) {
  size_t const current = program->currentModelIndex;
  size_t const next = 1 - current;
  dcc_ui_update(&program->models[current], message, &program->models[next]);
  program->currentModelIndex = next;
  dcc_ui_view(&program->view, &program->models[next]);
}
