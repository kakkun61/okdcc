#include "ui.h"

struct dcc_ui_Model_Command dcc_ui_init(void) {
  return (struct dcc_ui_Model_Command) {
    (struct dcc_ui_Model) {
      dcc_ui_MonitorModelTag,
      {0}
    }
  };
}

void dcc_ui_view(struct dcc_ui_Model model) {
  switch (model.tag) {
    case dcc_ui_MonitorModelTag:
      break;
    case dcc_ui_SelectModelTag:
      break;
    case dcc_ui_ShowModelTag:
      break;
  }
}

struct dcc_ui_Message dcc_ui_update(struct dcc_ui_Model model, struct dcc_ui_Message message) {

}

