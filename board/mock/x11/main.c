#include <okdcc/ui.h>

int main(void) {
  struct dcc_ui_Model_Command modelCommand = dcc_ui_init();
  dcc_ui_view(modelCommand.model);

  return 0;
}
