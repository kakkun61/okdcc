#include <stdbool.h>

#include "dcc.h"
#include "munit/munit.h"

static MunitResult test_decodeSignal_58_58_is_1(MunitParameter const params[], void *fixture) {
  dcc_Bit bit;
  munit_assert_int(dcc_Success, ==, dcc_decodeSignal(58UL, 58UL, &bit));
  munit_assert_uint32(DCC_BITS32_C(1), ==, bit);
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
  munit_assert_uint32(DCC_BITS32_C(0), ==, bit);
  return MUNIT_OK;
}

static MunitResult test_decodeSignals_1_58_116_is_1(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec const signals[] = { DCC_BITS32_C(1), DCC_BITS32_C(58), DCC_BITS32_C(116) };
  dcc_Bits32 bits[1];
  size_t readSignalsSize;
  size_t writtenBitsSize;
  enum dcc_Result const result = dcc_decodeSignals(signals, 3, &readSignalsSize, bits, 0, 32, &writtenBitsSize);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_size(2, ==, readSignalsSize);
  munit_assert_size(1, ==, writtenBitsSize);
  dcc_Bits32 one = DCC_BITS32_C(1);
  munit_assert_uint32(one << 31, ==, bits[0] & (one << 31));
  return MUNIT_OK;
}

static MunitResult test_decodeSignals_1_58_116_174_232_is_1_1(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec const signals[] = { DCC_BITS32_C(1), DCC_BITS32_C(58), DCC_BITS32_C(116), DCC_BITS32_C(174),
                                       DCC_BITS32_C(232) };
  dcc_Bits32 bits[1];
  size_t readSignalsSize;
  size_t writtenBitsSize;
  enum dcc_Result const result = dcc_decodeSignals(signals, 5, &readSignalsSize, bits, 0, 32, &writtenBitsSize);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_size(4, ==, readSignalsSize);
  munit_assert_size(2, ==, writtenBitsSize);
  dcc_Bits32 one = DCC_BITS32_C(1);
  munit_assert_uint32(one << 31, ==, bits[0] & (one << 31));
  munit_assert_uint32(one << 30, ==, bits[0] & (one << 30));
  return MUNIT_OK;
}

static MunitResult test_decodeSignals_1_52_110_is_failure(MunitParameter const params[], void *fixture) {
  dcc_TimeMicroSec const signals[] = { DCC_BITS32_C(1), DCC_BITS32_C(52), DCC_BITS32_C(110) };
  dcc_Bits32 bits[1];
  size_t readSignalsSize;
  size_t writtenBitsSize;
  enum dcc_Result const result = dcc_decodeSignals(signals, 3, &readSignalsSize, bits, 0, 32, &writtenBitsSize);
  munit_assert_int(dcc_Failure, ==, result);
  munit_assert_size(2, ==, readSignalsSize);
  munit_assert_size(0, ==, writtenBitsSize);
  dcc_Bits32 one = DCC_BITS32_C(1);
  munit_assert_uint32(0, ==, bits[0] & (one << 31));
  return MUNIT_OK;
}

static MunitResult test_consumeThroughPreamble_empty_is_failure(MunitParameter const params[], void *fixture) {
  dcc_Bits32 const bits[0];
  size_t next;
  enum dcc_Result const result = dcc_consumeThroughPreamble(bits, 0, 0, &next);
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}

static MunitResult test_consumeThroughPreamble_1_1_1_1_1_1_1_1_1_1_1_1_is_failure(MunitParameter const params[],
                                                                                  void *fixture) {
  dcc_Bits32 const bits[1] = { ~DCC_BITS32_C(0) };
  size_t next;
  enum dcc_Result const result = dcc_consumeThroughPreamble(bits, 0, 12, &next);
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}

static MunitResult test_consumeThroughPreamble_1_1_1_1_1_1_1_1_1_1_1_1_1_is_success(MunitParameter const params[],
                                                                                    void *fixture) {
  dcc_Bits32 const bits[1] = { ~DCC_BITS32_C(0) };
  size_t next;
  enum dcc_Result const result = dcc_consumeThroughPreamble(bits, 0, 13, &next);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_size(13, ==, next);
  return MUNIT_OK;
}

static MunitResult test_consumeThroughPreamble_X_1_1_1_1_1_1_1_1_1_1_1_1_is_failure(MunitParameter const params[],
                                                                                    void *fixture) {
  dcc_Bits32 const bits[1] = { ~DCC_BITS32_C(0) };
  size_t next;
  enum dcc_Result const result = dcc_consumeThroughPreamble(bits, 1, 13, &next);
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}

static MunitResult test_consumeThroughPreamble_X_1_1_1_1_1_1_1_1_1_1_1_1_1_1_is_success(MunitParameter const params[],
                                                                                        void *fixture) {
  dcc_Bits32 const bits[1] = { ~DCC_BITS32_C(0) };
  size_t next;
  enum dcc_Result const result = dcc_consumeThroughPreamble(bits, 1, 14, &next);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_size(14, ==, next);
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
  munit_assert_uint8(1, ==, packet.address);
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
      "/dcc_decodeSignal",
      (MunitTest[]){
        // name, test, setup, tear down, options, parameters
        { "(58, 58) is 1", test_decodeSignal_58_58_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(100, 9000) is 0", test_decodeSignal_100_9000_is_0, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(51, 58) is falure", test_decodeSignal_51_58_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_decodeSignals",
      (MunitTest[]){
        { "([1, 58, 116]) is [1]", test_decodeSignals_1_58_116_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([1, 58, 116, 174, 232]) is [1, 1]", test_decodeSignals_1_58_116_174_232_is_1_1, NULL, NULL,
          MUNIT_TEST_OPTION_NONE, NULL },
        { "([1, 52, 110]) is failure", test_decodeSignals_1_52_110_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE,
          NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/dcc_consumeThroughPreamble",
      (MunitTest[]){
        { "([]) is failure", test_consumeThroughPreamble_empty_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]) is failure",
          test_consumeThroughPreamble_1_1_1_1_1_1_1_1_1_1_1_1_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]) is success",
          test_consumeThroughPreamble_1_1_1_1_1_1_1_1_1_1_1_1_1_is_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([X, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]) is failure",
          test_consumeThroughPreamble_X_1_1_1_1_1_1_1_1_1_1_1_1_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([X, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]) is success",
          test_consumeThroughPreamble_X_1_1_1_1_1_1_1_1_1_1_1_1_1_1_is_success, NULL, NULL, MUNIT_TEST_OPTION_NONE,
          NULL },
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
      NULL, 1, MUNIT_SUITE_OPTION_NONE } },
  1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *const argv[]) { return munit_suite_main(&suite, NULL, argc, argv); }
