#ifndef DCC_UI_H
#define DCC_UI_H

#include <lvgl.h>

#define DCC_UI_PACKETS_SIZE 100

enum dcc_ui_ModelTag {
  dcc_ui_MonitorModelTag,
  dcc_ui_SelectModelTag,
  dcc_ui_ShowModelTag,
};

struct dcc_ui_MonitorModel {
  char packets[28][DCC_UI_PACKETS_SIZE];
};

struct dcc_ui_Model {
  lv_indev_t *buttonsIndev;
  enum dcc_ui_ModelTag tag;
  union {
    struct dcc_ui_MonitorModel monitorModel;
  } model;
};

enum dcc_ui_ViewTag {
  dcc_ui_StartViewTag,
};

struct dcc_ui_View {
  enum dcc_ui_ViewTag tag;
};

enum dcc_ui_MessageTag {
  dcc_ui_NoneMessageTag,
};

struct dcc_ui_Message {
  enum dcc_ui_MessageTag tag;
};

enum dcc_ui_CommandTag {
  dcc_ui_NoneCommandTag,
};

struct dcc_ui_Command {
  enum dcc_ui_CommandTag tag;
};

struct dcc_ui_Model_Command {
  struct dcc_ui_Model model;
  struct dcc_ui_Command command;
};

struct dcc_ui_Model_Command dcc_ui_init(lv_indev_t *buttonsIndev);

void dcc_ui_view(struct dcc_ui_Model model);

struct dcc_ui_Command dcc_ui_update(struct dcc_ui_Model model, struct dcc_ui_Message message);

#endif
