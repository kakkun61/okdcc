#include "ui.h"

struct dcc_ui_Model_Command dcc_ui_init(void) {
  return (struct dcc_ui_Model_Command) { .model = (struct dcc_ui_Model) { .tag = dcc_ui_MonitorModelTag,
                                                                          .model.monitorModel.packets = { { 0 } } },
                                         .command = (struct dcc_ui_Command) { .tag = dcc_ui_NoneCommandTag } };
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

// struct dcc_ui_Command dcc_ui_update(struct dcc_ui_Model model, struct dcc_ui_Message message) {
//   // TODO
// }
