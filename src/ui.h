#ifndef DCC_UI_H
#define DCC_UI_H

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
  enum dcc_ui_ModelTag tag;
  union {
    struct dcc_ui_MonitorModel monitorModel;
  };
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

struct dcc_ui_Model_Command {
  struct dcc_ui_Model model;
  // struct dcc_ui_Command command;
};

struct dcc_ui_Model_Command dcc_ui_init(void);

void dcc_ui_view(struct dcc_ui_Model model);

struct dcc_ui_Message dcc_ui_update(struct dcc_ui_Model model, struct dcc_ui_Message message);

#endif
