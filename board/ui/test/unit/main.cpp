#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <munit.h>
#include <stdlib.h>

#include <okdcc/ui.hpp>

namespace {

struct Model {
  int count;
  int views;
};

struct Message {
  int delta;
};

Model init() { return Model{ 0, 0 }; }

Model update(Model const &model, Message const &message) { return Model{ model.count + message.delta, model.views }; }

int viewCalls = 0;

void view(Model const &model, okdcc::ui::Frame &frame) {
  viewCalls++;
  CLAY(CLAY_ID("Root"), {}) { CLAY_TEXT(frame.format("count %d", model.count), CLAY_TEXT_CONFIG({})); }
}

bool onButtonPressed(Model const &, uint8_t button, Message *message) {
  if (button == 1) {
    *message = Message{ 1 };
    return true;
  }
  if (button == 4) {
    *message = Message{ -1 };
    return true;
  }
  return false;
}

using Program = okdcc::ui::Program<Model, Message>;
Program::Definition const definition{ init, update, view, onButtonPressed };

Clay_Dimensions measureText(Clay_StringSlice text, Clay_TextElementConfig *, void *) {
  return Clay_Dimensions{ static_cast<float>(text.length), 1 };
}

void initializeClay() {
  static bool initialized = false;
  if (initialized) return;
  initialized = true;
  Clay_SetMaxElementCount(16);
  uint32_t const size = Clay_MinMemorySize();
  Clay_Initialize(Clay_CreateArenaWithCapacityAndMemory(size, malloc(size)), Clay_Dimensions{ 100, 100 }, {});
  Clay_SetMeasureTextFunction(measureText, nullptr);
}

MunitResult test_Frame_format(MunitParameter const[], void *) {
  char buffer[16];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  Clay_String const a = frame.format("a%d", 1);
  munit_assert_int(2, ==, a.length);
  munit_assert_memory_equal(2, "a1", a.chars);
  Clay_String const b = frame.format("bc");
  munit_assert_memory_equal(2, "bc", b.chars);
  munit_assert_memory_equal(2, "a1", a.chars);
  return MUNIT_OK;
}

MunitResult test_Frame_format_truncates(MunitParameter const[], void *) {
  char buffer[4];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  Clay_String const a = frame.format("abcdef");
  munit_assert_int(3, ==, a.length);
  munit_assert_memory_equal(3, "abc", a.chars);
  Clay_String const b = frame.format("x");
  munit_assert_int(0, ==, b.length);
  return MUNIT_OK;
}

MunitResult test_Frame_allocate_and_reset(MunitParameter const[], void *) {
  char buffer[8];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  munit_assert_ptr_not_null(frame.allocate(3));
  munit_assert_ptr_not_null(frame.allocate(3));
  munit_assert_ptr_null(frame.allocate(0));
  frame.reset();
  munit_assert_ptr_equal(buffer, frame.allocate(7));
  return MUNIT_OK;
}

MunitResult test_Program_dispatch(MunitParameter const[], void *) {
  char buffer[64];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  Program program(definition, frame);
  program.start();
  program.dispatch(Message{ 2 });
  program.dispatch(Message{ 3 });
  munit_assert_int(5, ==, program.model().count);
  return MUNIT_OK;
}

MunitResult test_Program_handleButtons_reacts_to_presses_only(MunitParameter const[], void *) {
  char buffer[64];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  Program program(definition, frame);
  program.start();
  program.handleButtons(1);
  program.handleButtons(1);
  munit_assert_int(1, ==, program.model().count);
  program.handleButtons(0);
  program.handleButtons(1 | 2 | 4);
  munit_assert_int(1, ==, program.model().count);
  return MUNIT_OK;
}

MunitResult test_Program_frame_lays_out_view(MunitParameter const[], void *) {
  initializeClay();
  char buffer[64];
  okdcc::ui::Frame frame(buffer, sizeof buffer);
  Program program(definition, frame);
  program.start();
  program.dispatch(Message{ 7 });
  viewCalls = 0;
  Clay_RenderCommandArray const commands = program.frame(0);
  munit_assert_int(1, ==, viewCalls);
  bool found = false;
  for (int32_t i = 0; i < commands.length; i++) {
    Clay_RenderCommand const &command = commands.internalArray[i];
    if (command.commandType != CLAY_RENDER_COMMAND_TYPE_TEXT) continue;
    Clay_StringSlice const text = command.renderData.text.stringContents;
    found = found || (text.length == 7 && memcmp(text.chars, "count 7", 7) == 0);
  }
  munit_assert_true(found);
  return MUNIT_OK;
}

MunitTest tests[] = {
  { (char *) "/okdcc/ui/Frame/format", test_Frame_format, nullptr, nullptr, MUNIT_TEST_OPTION_NONE, nullptr },
  { (char *) "/okdcc/ui/Frame/format truncates",
    test_Frame_format_truncates,
    nullptr,
    nullptr,
    MUNIT_TEST_OPTION_NONE,
    nullptr },
  { (char *) "/okdcc/ui/Frame/allocate and reset",
    test_Frame_allocate_and_reset,
    nullptr,
    nullptr,
    MUNIT_TEST_OPTION_NONE,
    nullptr },
  { (char *) "/okdcc/ui/Program/dispatch", test_Program_dispatch, nullptr, nullptr, MUNIT_TEST_OPTION_NONE, nullptr },
  { (char *) "/okdcc/ui/Program/handleButtons reacts to presses only",
    test_Program_handleButtons_reacts_to_presses_only,
    nullptr,
    nullptr,
    MUNIT_TEST_OPTION_NONE,
    nullptr },
  { (char *) "/okdcc/ui/Program/frame lays out view",
    test_Program_frame_lays_out_view,
    nullptr,
    nullptr,
    MUNIT_TEST_OPTION_NONE,
    nullptr },
  { nullptr, nullptr, nullptr, nullptr, MUNIT_TEST_OPTION_NONE, nullptr },
};

MunitSuite const suite = { (char *) "", tests, nullptr, 1, MUNIT_SUITE_OPTION_NONE };

}  // namespace

int main(int argc, char *argv[]) { return munit_suite_main(&suite, nullptr, argc, argv); }
