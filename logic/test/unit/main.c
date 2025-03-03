#include <munit.h>
#include <okdcc/logic_internal.h>
#include <stdbool.h>

static MunitResult test_writeSignalBuffer_1_is_success(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec array[1] = { 0 };
  struct dcc_SignalBuffer buffer = dcc_initializeSignalBuffer(array, 1);
  munit_assert_int(dcc_Success, ==, dcc_writeSignalBuffer(&buffer, 1));
  return MUNIT_OK;
}

static MunitResult test_writeSignalBuffer_1_2_is_failure(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec array[1] = { 0 };
  struct dcc_SignalBuffer buffer = dcc_initializeSignalBuffer(array, 1);
  munit_assert_int(dcc_Success, ==, dcc_writeSignalBuffer(&buffer, 1));
  munit_assert_int(dcc_Failure, ==, dcc_writeSignalBuffer(&buffer, 2));
  return MUNIT_OK;
}

static MunitResult test_SignalBuffer_1_read_is_failure(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec array[1] = { 0 };
  struct dcc_SignalBuffer buffer = dcc_initializeSignalBuffer(array, 1);
  dcc_TimeMicroSec signal;
  munit_assert_int(dcc_Failure, ==, dcc_readSignalBuffer(&buffer, &signal));
  return MUNIT_OK;
}

static MunitResult test_SignalBuffer_1_write_read_write_read_is_success(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec array[1] = { 0 };
  struct dcc_SignalBuffer buffer = dcc_initializeSignalBuffer(array, 1);
  munit_assert_int(dcc_Success, ==, dcc_writeSignalBuffer(&buffer, 1));
  dcc_TimeMicroSec signal;
  munit_assert_int(dcc_Success, ==, dcc_readSignalBuffer(&buffer, &signal));
  munit_assert_uint64(1, ==, signal);
  munit_assert_int(dcc_Success, ==, dcc_writeSignalBuffer(&buffer, 2));
  munit_assert_int(dcc_Success, ==, dcc_readSignalBuffer(&buffer, &signal));
  munit_assert_uint64(2, ==, signal);
  return MUNIT_OK;
}

static MunitResult test_SignalBuffer_1_write_write_read_is_failure_once_and_success(MunitParameter const params[],
                                                                                    void *fixture) {
  dcc_TimeMicroSec array[1] = { 0 };
  struct dcc_SignalBuffer buffer = dcc_initializeSignalBuffer(array, 1);
  munit_assert_int(dcc_Success, ==, dcc_writeSignalBuffer(&buffer, 1));
  munit_assert_int(dcc_Failure, ==, dcc_writeSignalBuffer(&buffer, 2));
  dcc_TimeMicroSec signal;
  munit_assert_int(dcc_Success, ==, dcc_readSignalBuffer(&buffer, &signal));
  munit_assert_uint64(1, ==, signal);
  return MUNIT_OK;
}

static MunitResult test_decodeSignal_58_58_is_1(MunitParameter const params[], void *fixture) {
  dcc_Bit bit;
  munit_assert_int(dcc_Success, ==, dcc_decodeSignal(58UL, 58UL, &bit));
  munit_assert_uint32(1, ==, bit);
  return MUNIT_OK;
}

static MunitResult test_decodeSignal_51_58_is_failure(MunitParameter const params[], void *fixture) {
  dcc_Bit bit;
  munit_assert_int(dcc_Failure, ==, dcc_decodeSignal(51UL, 58UL, &bit));
  return MUNIT_OK;
}

static MunitResult test_decodeSignal_100_9000_is_0(MunitParameter const params[], void *fixture) {
  dcc_Bit bit;
  munit_assert_int(dcc_Success, ==, dcc_decodeSignal(100UL, 9000UL, &bit));
  munit_assert_uint32(0, ==, bit);
  return MUNIT_OK;
}

static MunitResult test_feedSignal_0_is_continue(MunitParameter const params[], void *fixture) {
  struct dcc_SignalStreamParser parser = dcc_initializeSignalStreamParser();
  dcc_Bit bit;
  enum dcc_StreamParserResult const result = dcc_feedSignal(&parser, 0, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result);
  return MUNIT_OK;
}

static MunitResult test_feedSignal_0_1_is_continue(MunitParameter const params[], void *fixture) {
  struct dcc_SignalStreamParser parser = dcc_initializeSignalStreamParser();
  dcc_Bit bit;
  enum dcc_StreamParserResult const result1 = dcc_feedSignal(&parser, 0, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result1);
  enum dcc_StreamParserResult const result2 = dcc_feedSignal(&parser, 1, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result2);
  return MUNIT_OK;
}

static MunitResult test_feedSignal_0_1_2_is_failure(MunitParameter const params[], void *fixture) {
  struct dcc_SignalStreamParser parser = dcc_initializeSignalStreamParser();
  dcc_Bit bit;
  enum dcc_StreamParserResult const result1 = dcc_feedSignal(&parser, 0, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result1);
  enum dcc_StreamParserResult const result2 = dcc_feedSignal(&parser, 1, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result2);
  enum dcc_StreamParserResult const result3 = dcc_feedSignal(&parser, 2, &bit);
  munit_assert_int(dcc_StreamParserResult_Failure, ==, result3);
  return MUNIT_OK;
}

static MunitResult test_feedSignal_0_57_114_is_1(MunitParameter const params[], void *fixture) {
  struct dcc_SignalStreamParser parser = dcc_initializeSignalStreamParser();
  dcc_Bit bit;
  enum dcc_StreamParserResult const result1 = dcc_feedSignal(&parser, 0, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result1);
  enum dcc_StreamParserResult const result2 = dcc_feedSignal(&parser, 57, &bit);
  munit_assert_int(dcc_StreamParserResult_Continue, ==, result2);
  enum dcc_StreamParserResult const result3 = dcc_feedSignal(&parser, 114, &bit);
  munit_assert_int(dcc_StreamParserResult_Success, ==, result3);
  munit_assert_int(1, ==, bit);
  return MUNIT_OK;
}

static MunitResult test_validatePacket_0x00_0x00_is_success(MunitParameter const params[], void *fixture) {
  uint8_t const bits[1] = { 0 };
  enum dcc_Result const result = dcc_validatePacket(bits, 1, UINT8_C(0));
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_validatePacket_0x00_0x01_is_failure(MunitParameter const params[], void *fixture) {
  uint8_t const bits[1] = { 0 };
  enum dcc_Result const result = dcc_validatePacket(bits, 1, UINT8_C(1));
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}

static MunitResult test_validatePacket_0x00_0x01_0x01_is_success(MunitParameter const params[], void *fixture) {
  uint8_t const bits[2] = { UINT8_C(0), UINT8_C(1) };
  enum dcc_Result const result = dcc_validatePacket(bits, 2, UINT8_C(1));
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parseSpeedAndDirectionPacket_0b00000001_0b01101010_0b01101011_is_1_forward_20(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x01), UINT8_C(0x6A), UINT8_C(0x6B) };
  struct dcc_SpeedAndDirectionPacket packet;
  enum dcc_Result const result = dcc_parseSpeedAndDirectionPacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(1, ==, packet.address);
  munit_assert_int(dcc_Forward, ==, packet.direction);
  munit_assert_uint8(20, ==, packet.speed);
  return MUNIT_OK;
}

static MunitResult test_parseAllDecoderResetPacket_0b00000000_0b00000000_0b00000000_is_success(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x00) };
  enum dcc_Result const result = dcc_parseAllDecoderResetPacket(bytes, 3);
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parseAllDecoderIdlePacket_0b11111111_0b00000000_0b11111111_is_success(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0xFF), UINT8_C(0x00), UINT8_C(0xFF) };
  enum dcc_Result const result = dcc_parseAllDecoderIdlePacket(bytes, 3);
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parseDecoderResetPacket_0b00000011_0b0000000_0b00000011_is_3(MunitParameter const params[],
                                                                                     void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x00), UINT8_C(0x03) };
  struct dcc_DecoderResetPacket packet;
  enum dcc_Result const result = dcc_parseDecoderResetPacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(3, ==, packet.address);
  return MUNIT_OK;
}

static MunitResult test_parseDecoderResetPacket_0b11000011_0b0000000_0b0000000_0b11000011_is_768(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0xC3), UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0xC3) };
  struct dcc_DecoderResetPacket packet;
  enum dcc_Result const result = dcc_parseDecoderResetPacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(768, ==, packet.address);
  return MUNIT_OK;
}

static MunitSuite const suite = {
  "/m5-dcc", NULL,
  (MunitSuite[]){
    { // name, tests, suites, iterations, options
      "/SignalBuffer",
      (MunitTest[]){
        { "(1)/write is success", test_writeSignalBuffer_1_is_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(1)/read is failure", test_SignalBuffer_1_read_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(1)/write write is failure", test_writeSignalBuffer_1_2_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE,
          NULL },
        { "(1)/write read write read is success", test_SignalBuffer_1_write_read_write_read_is_success, NULL, NULL,
          MUNIT_TEST_OPTION_NONE, NULL },
        { "(1)/write write read is failure once and success",
          test_SignalBuffer_1_write_write_read_is_failure_once_and_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_decodeSignal",
      (MunitTest[]){
        // name, test, setup, tear down, options, parameters
        { "(58, 58) is 1", test_decodeSignal_58_58_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(100, 9000) is 0", test_decodeSignal_100_9000_is_0, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(51, 58) is failure", test_decodeSignal_51_58_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_feedSignal",
      (MunitTest[]){
        { "([0]) is continue", test_feedSignal_0_is_continue, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 1]) is continue", test_feedSignal_0_1_is_continue, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 1, 2]) is failure", test_feedSignal_0_1_2_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 57, 114]) is success", test_feedSignal_0_57_114_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_validatePacket",
      (MunitTest[]){ { "([0x00], 0x00) is success", test_validatePacket_0x00_0x00_is_success, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { "([0x00], 0x01) is failure", test_validatePacket_0x00_0x01_is_failure, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { "([0x00, 0x01], 0x01) is success", test_validatePacket_0x00_0x01_0x01_is_success, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseSpeedAndDirectionPacket",
      (MunitTest[]){ { "([0b00000001, 0b01101010], 0b01101011) is { address = 1, direction = forward, speed = 20 }",
                       test_parseSpeedAndDirectionPacket_0b00000001_0b01101010_0b01101011_is_1_forward_20, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseAllDecoderResetPacket",
      (MunitTest[]){ { "([0b00000000, 0b00000000, 0b00000000]) is success",
                       test_parseAllDecoderResetPacket_0b00000000_0b00000000_0b00000000_is_success, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseAllDecoderIdlePacket",
      (MunitTest[]){ { "([0b11111111, 0b00000000, 0b11111111]) is success",
                       test_parseAllDecoderIdlePacket_0b11111111_0b00000000_0b11111111_is_success, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseDecoderResetPacket",
      (MunitTest[]){ { "([0b00000110, 0b0000000, 0b00000110]) is { address = 3 }",
                       test_parseDecoderResetPacket_0b00000011_0b0000000_0b00000011_is_3, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { "([0b11000011, 0b0000000, 0b0000000, 0b11000011]) is { address = 768 }",
                       test_parseDecoderResetPacket_0b11000011_0b0000000_0b0000000_0b11000011_is_768, NULL, NULL,
                       MUNIT_TEST_OPTION_NONE, NULL },
                     { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE } },
  1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *const argv[]) { return munit_suite_main(&suite, NULL, argc, argv); }
