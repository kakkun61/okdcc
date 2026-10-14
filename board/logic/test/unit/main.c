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

static MunitResult
test_parseSpeedAndDirectionPacketForLocomotiveDecoders_0b00000001_0b01101010_0b01101011_is_1_forward_20(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x01), UINT8_C(0x6A), UINT8_C(0x6B) };
  struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders packet;
  enum dcc_Result const result = dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(bytes, 3, false, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(1, ==, packet.address);
  munit_assert_int(dcc_Forward, ==, packet.direction);
  munit_assert_uint8(false, ==, packet.emergencyStop);
  munit_assert_uint8(false, ==, packet.flControl);
  munit_assert_uint8(17, ==, packet.speed5Bit);
  munit_assert_int(false, ==, packet.directionMayBeIgnored);
  return MUNIT_OK;
}

static MunitResult
test_parseSpeedAndDirectionPacketForLocomotiveDecoders_0b00000001_0b01101010_0b01101011_is_1_forward_9_off(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x01), UINT8_C(0x6A), UINT8_C(0x6B) };
  struct dcc_SpeedAndDirectionPacketForLocomotiveDecoders packet;
  enum dcc_Result const result = dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders(bytes, 3, true, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(1, ==, packet.address);
  munit_assert_int(dcc_Forward, ==, packet.direction);
  munit_assert_uint8(false, ==, packet.emergencyStop);
  munit_assert_uint8(true, ==, packet.flControl);
  munit_assert_uint8(9, ==, packet.speed4Bit);
  munit_assert_int(false, ==, packet.fl);
  return MUNIT_OK;
}

static MunitResult test_parseResetPacketForAllDecoders_0b00000000_0b00000000_0b00000000_is_success(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x00) };
  enum dcc_Result const result = dcc_parseResetPacketForAllDecoders(bytes, 3);
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parseIdlePacketForAllDecoders_0b11111111_0b00000000_0b11111111_is_success(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0xFF), UINT8_C(0x00), UINT8_C(0xFF) };
  enum dcc_Result const result = dcc_parseIdlePacketForAllDecoders(bytes, 3);
  munit_assert_int(dcc_Success, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parseResetPacketForMultiFunctionDecoders_0b00000011_0b0000000_0b00000011_is_3(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x00), UINT8_C(0x03) };
  struct dcc_ResetPacketForMultiFunctionDecoders packet;
  enum dcc_Result const result = dcc_parseResetPacketForMultiFunctionDecoders(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(3, ==, packet.address);
  return MUNIT_OK;
}

static MunitResult test_parseResetPacketForMultiFunctionDecoders_0b11000011_0b0000000_0b0000000_0b11000011_is_768(
  MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0xC3), UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0xC3) };
  struct dcc_ResetPacketForMultiFunctionDecoders packet;
  enum dcc_Result const result = dcc_parseResetPacketForMultiFunctionDecoders(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_uint16(768, ==, packet.address);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_broadcast_stop_shutdown(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x00), UINT8_C(0x41), UINT8_C(0x41) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_BroadcastStopPacketForAllDecodersTag, ==, packet.tag);
  munit_assert_int(dcc_BroadcastStopKind_Shutdown, ==, packet.broadcastStopPacketForAllDecoders.kind);
  munit_assert_int(dcc_Backward, ==, packet.broadcastStopPacketForAllDecoders.direction);
  munit_assert_int(false, ==, packet.broadcastStopPacketForAllDecoders.directionMayBeIgnored);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_broadcast_stop_direction_may_be_ignored(MunitParameter const params[],
                                                                            void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x00), UINT8_C(0x70), UINT8_C(0x70) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_BroadcastStopPacketForAllDecodersTag, ==, packet.tag);
  munit_assert_int(dcc_BroadcastStopKind_Stop, ==, packet.broadcastStopPacketForAllDecoders.kind);
  munit_assert_int(dcc_Forward, ==, packet.broadcastStopPacketForAllDecoders.direction);
  munit_assert_int(true, ==, packet.broadcastStopPacketForAllDecoders.directionMayBeIgnored);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_locomotive_speed_and_direction(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x6A), UINT8_C(0x69) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.speedAndDirectionPacketForLocomotiveDecoders.address);
  munit_assert_int(dcc_Forward, ==, packet.speedAndDirectionPacketForLocomotiveDecoders.direction);
  munit_assert_uint8(17, ==, packet.speedAndDirectionPacketForLocomotiveDecoders.speed5Bit);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_broadcast_speed_and_direction(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x00), UINT8_C(0x6A), UINT8_C(0x6A) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(0, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.address);
  munit_assert_int(dcc_Forward, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.direction);
  munit_assert_uint8(17, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.speed5Bit);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_long_address_speed_and_direction(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0xC3), UINT8_C(0x00), UINT8_C(0x4A), UINT8_C(0x89) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_SpeedAndDirectionPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(768, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.address);
  munit_assert_int(dcc_Backward, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.direction);
  munit_assert_uint8(17, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.speed5Bit);
  munit_assert_int(false, ==, packet.speedAndDirectionPacketForMultiFunctionDecoders.emergencyStop);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_factory_test_without_data(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x02), UINT8_C(0x01) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.address);
  munit_assert_int(false, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.set);
  munit_assert_int(false, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.dataExists);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_factory_test_with_data(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0x03), UINT8_C(0x55), UINT8_C(0x55) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.address);
  munit_assert_int(true, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.set);
  munit_assert_int(true, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.dataExists);
  munit_assert_uint8(0x55, ==, packet.factoryTestInstructionPacketForMultiFunctionDecoders.data);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_set_decoder_flags(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0x07), UINT8_C(0x93), UINT8_C(0x97) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.setDecoderFlagsPacketForMultiFunctionDecoders.address);
  munit_assert_int(true, ==, packet.setDecoderFlagsPacketForMultiFunctionDecoders.set);
  munit_assert_uint8(3, ==, packet.setDecoderFlagsPacketForMultiFunctionDecoders.subaddress);
  munit_assert_int(dcc_Set111Instruction, ==, packet.setDecoderFlagsPacketForMultiFunctionDecoders.instruction);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_set_extended_addressing(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x0B), UINT8_C(0x08) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.setExtendedAddressingPacketForMultiFunctionDecoders.address);
  munit_assert_int(true, ==, packet.setExtendedAddressingPacketForMultiFunctionDecoders.set);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_restricted_speed_step(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0x3E), UINT8_C(0x0A), UINT8_C(0x37) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_RestrictedSpeedStepPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.restrictedSpeedStepPacketForMultiFunctionDecoders.address);
  munit_assert_int(true, ==, packet.restrictedSpeedStepPacketForMultiFunctionDecoders.enabled);
  munit_assert_int(false, ==, packet.restrictedSpeedStepPacketForMultiFunctionDecoders.flControl);
  munit_assert_uint8(17, ==, packet.restrictedSpeedStepPacketForMultiFunctionDecoders.speed5Bit);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_analog_function(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[5] = { UINT8_C(0x03), UINT8_C(0x3D), UINT8_C(0x01), UINT8_C(0x80), UINT8_C(0xBF) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 5, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_AnalogFunctionPacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.analogFunctionPacketForMultiFunctionDecoders.address);
  munit_assert_uint8(1, ==, packet.analogFunctionPacketForMultiFunctionDecoders.output);
  munit_assert_uint8(0x80, ==, packet.analogFunctionPacketForMultiFunctionDecoders.data);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_function_group_1(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0x95), UINT8_C(0x96) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FunctionGroup1PacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.functionGroup1PacketForMultiFunctionDecoders.address);
  munit_assert_int(true, ==, packet.functionGroup1PacketForMultiFunctionDecoders.flControl);
  munit_assert_int(true, ==, packet.functionGroup1PacketForMultiFunctionDecoders.fl);
  munit_assert_int(true, ==, packet.functionGroup1PacketForMultiFunctionDecoders.f1);
  munit_assert_int(false, ==, packet.functionGroup1PacketForMultiFunctionDecoders.f2);
  munit_assert_int(true, ==, packet.functionGroup1PacketForMultiFunctionDecoders.f3);
  munit_assert_int(false, ==, packet.functionGroup1PacketForMultiFunctionDecoders.f4);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_function_group_2_f5_f8(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0xB5), UINT8_C(0xB6) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FunctionGroup2PacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.functionGroup2PacketForMultiFunctionDecoders.address);
  munit_assert_int(dcc_FunctionGroup2Group_F5_F8, ==, packet.functionGroup2PacketForMultiFunctionDecoders.group);
  munit_assert_int(true, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f5);
  munit_assert_int(false, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f6);
  munit_assert_int(true, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f7);
  munit_assert_int(false, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f8);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_function_group_2_f9_f12(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x03), UINT8_C(0xAA), UINT8_C(0xA9) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FunctionGroup2PacketForMultiFunctionDecodersTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.functionGroup2PacketForMultiFunctionDecoders.address);
  munit_assert_int(dcc_FunctionGroup2Group_F9_F12, ==, packet.functionGroup2PacketForMultiFunctionDecoders.group);
  munit_assert_int(false, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f9);
  munit_assert_int(true, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f10);
  munit_assert_int(false, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f11);
  munit_assert_int(true, ==, packet.functionGroup2PacketForMultiFunctionDecoders.functions.f12);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_binary_state_control_long_form(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[5] = { UINT8_C(0x03), UINT8_C(0xC0), UINT8_C(0x85), UINT8_C(0x02), UINT8_C(0x44) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 5, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_BinaryStateControlLongFormPacketTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.binaryStateControlLongFormPacket.address);
  munit_assert_uint16(261, ==, packet.binaryStateControlLongFormPacket.stateAddress);
  munit_assert_int(true, ==, packet.binaryStateControlLongFormPacket.state);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_binary_state_control_short_form(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0xDD), UINT8_C(0x05), UINT8_C(0xDB) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_BinaryStateControlShortFormPacketTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.binaryStateControlShortFormPacket.address);
  munit_assert_uint8(5, ==, packet.binaryStateControlShortFormPacket.stateAddress);
  munit_assert_int(false, ==, packet.binaryStateControlShortFormPacket.state);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_function_control_f13_f20(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0xDE), UINT8_C(0x81), UINT8_C(0x5C) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FunctionControlF13F20PacketTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.functionControlF13F20Packet.address);
  munit_assert_int(true, ==, packet.functionControlF13F20Packet.f13);
  munit_assert_int(false, ==, packet.functionControlF13F20Packet.f14);
  munit_assert_int(true, ==, packet.functionControlF13F20Packet.f20);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_function_control_f21_f28(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0xDF), UINT8_C(0x02), UINT8_C(0xDE) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Success, ==, result);
  munit_assert_int(dcc_FunctionControlF21F28PacketTag, ==, packet.tag);
  munit_assert_uint16(3, ==, packet.functionControlF21F28Packet.address);
  munit_assert_int(false, ==, packet.functionControlF21F28Packet.f21);
  munit_assert_int(true, ==, packet.functionControlF21F28Packet.f22);
  munit_assert_int(false, ==, packet.functionControlF21F28Packet.f28);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_accessory_address_is_failure(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[3] = { UINT8_C(0x81), UINT8_C(0x00), UINT8_C(0x81) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 3, &packet);
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}

static MunitResult test_parsePacket_too_long_packet_is_failure(MunitParameter const params[], void *fixture) {
  uint8_t const bytes[4] = { UINT8_C(0x03), UINT8_C(0x00), UINT8_C(0x00), UINT8_C(0x03) };
  struct dcc_Packet packet;
  enum dcc_Result const result = dcc_parsePacket(bytes, 4, &packet);
  munit_assert_int(dcc_Failure, ==, result);
  return MUNIT_OK;
}
static MunitSuite const suite = {
  "/okdcc",
  NULL,
  (MunitSuite[]) {
    { // name, tests, suites, iterations, options
      "/SignalBuffer",
      (MunitTest[]) {
        { "(1)/write is success", test_writeSignalBuffer_1_is_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(1)/read is failure", test_SignalBuffer_1_read_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(1)/write write is failure",
          test_writeSignalBuffer_1_2_is_failure,
          NULL,
          NULL,
          MUNIT_TEST_OPTION_NONE,
          NULL },
        { "(1)/write read write read is success",
          test_SignalBuffer_1_write_read_write_read_is_success,
          NULL,
          NULL,
          MUNIT_TEST_OPTION_NONE,
          NULL },
        { "(1)/write write read is failure once and success",
          test_SignalBuffer_1_write_write_read_is_failure_once_and_success,
          NULL,
          NULL,
          MUNIT_TEST_OPTION_NONE,
          NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_decodeSignal",
      (MunitTest[]) {
        // name, test, setup, tear down, options, parameters
        { "(58, 58) is 1", test_decodeSignal_58_58_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(100, 9000) is 0", test_decodeSignal_100_9000_is_0, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "(51, 58) is failure", test_decodeSignal_51_58_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_feedSignal",
      (MunitTest[]) {
        { "([0]) is continue", test_feedSignal_0_is_continue, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 1]) is continue", test_feedSignal_0_1_is_continue, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 1, 2]) is failure", test_feedSignal_0_1_2_is_failure, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { "([0, 57, 114]) is success", test_feedSignal_0_57_114_is_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_validatePacket",
      (MunitTest[]) { { "([0x00], 0x00) is success",
                        test_validatePacket_0x00_0x00_is_success,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x00], 0x01) is failure",
                        test_validatePacket_0x00_0x01_is_failure,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x00, 0x01], 0x01) is success",
                        test_validatePacket_0x00_0x01_0x01_is_success,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseSpeedAndDirectionPacketForLocomotiveDecoders",
      (MunitTest[]) {
        { "([0b00000001, 0b01101010], 0b01101011) is { address = 1, direction = forward, emergencyStop = false, "
          "flControl = false, speed5Step = 17, directionMayBeIgnored = false }",
          test_parseSpeedAndDirectionPacketForLocomotiveDecoders_0b00000001_0b01101010_0b01101011_is_1_forward_20,
          NULL,
          NULL,
          MUNIT_TEST_OPTION_NONE,
          NULL },
        { "([0b00000001, 0b01101010], 0b01101011) is { address = 1, direction = forward, emergencyStop = false, "
          "flControl = true, speed4Step = 9, fl = false }",
          test_parseSpeedAndDirectionPacketForLocomotiveDecoders_0b00000001_0b01101010_0b01101011_is_1_forward_9_off,
          NULL,
          NULL,
          MUNIT_TEST_OPTION_NONE,
          NULL },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseResetPacketForAllDecoders",
      (MunitTest[]) { { "([0b00000000, 0b00000000, 0b00000000]) is success",
                        test_parseResetPacketForAllDecoders_0b00000000_0b00000000_0b00000000_is_success,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseIdlePacketForAllDecoders",
      (MunitTest[]) { { "([0b11111111, 0b00000000, 0b11111111]) is success",
                        test_parseIdlePacketForAllDecoders_0b11111111_0b00000000_0b11111111_is_success,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parseResetPacketForMultiFunctionDecoders",
      (MunitTest[]) { { "([0b00000110, 0b0000000, 0b00000110]) is { address = 3 }",
                        test_parseResetPacketForMultiFunctionDecoders_0b00000011_0b0000000_0b00000011_is_3,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0b11000011, 0b0000000, 0b0000000, 0b11000011]) is { address = 768 }",
                        test_parseResetPacketForMultiFunctionDecoders_0b11000011_0b0000000_0b0000000_0b11000011_is_768,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { "/dcc_parsePacket",
      (MunitTest[]) { { "([0x00, 0x41, 0x41]) broadcast stop shutdown",
                        test_parsePacket_broadcast_stop_shutdown,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x00, 0x70, 0x70]) broadcast stop direction may be ignored",
                        test_parsePacket_broadcast_stop_direction_may_be_ignored,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x6A, 0x69]) locomotive speed and direction",
                        test_parsePacket_locomotive_speed_and_direction,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x00, 0x6A, 0x6A]) broadcast speed and direction",
                        test_parsePacket_broadcast_speed_and_direction,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0xC3, 0x00, 0x4A, 0x89]) long address speed and direction",
                        test_parsePacket_long_address_speed_and_direction,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x02, 0x01]) factory test without data",
                        test_parsePacket_factory_test_without_data,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x03, 0x55, 0x55]) factory test with data",
                        test_parsePacket_factory_test_with_data,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x07, 0x93, 0x97]) set decoder flags",
                        test_parsePacket_set_decoder_flags,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x0B, 0x08]) set extended addressing",
                        test_parsePacket_set_extended_addressing,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x3E, 0x0A, 0x37]) restricted speed step",
                        test_parsePacket_restricted_speed_step,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x3D, 0x01, 0x80, 0xBF]) analog function",
                        test_parsePacket_analog_function,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x95, 0x96]) function group 1",
                        test_parsePacket_function_group_1,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xB5, 0xB6]) function group 2 f5 f8",
                        test_parsePacket_function_group_2_f5_f8,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xAA, 0xA9]) function group 2 f9 f12",
                        test_parsePacket_function_group_2_f9_f12,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xC0, 0x85, 0x02, 0x44]) binary state control long form",
                        test_parsePacket_binary_state_control_long_form,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xDD, 0x05, 0xDB]) binary state control short form",
                        test_parsePacket_binary_state_control_short_form,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xDE, 0x81, 0x5C]) function control f13 f20",
                        test_parsePacket_function_control_f13_f20,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0xDF, 0x02, 0xDE]) function control f21 f28",
                        test_parsePacket_function_control_f21_f28,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x81, 0x00, 0x81]) accessory address is failure",
                        test_parsePacket_accessory_address_is_failure,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { "([0x03, 0x00, 0x00, 0x03]) too long packet is failure",
                        test_parsePacket_too_long_packet_is_failure,
                        NULL,
                        NULL,
                        MUNIT_TEST_OPTION_NONE,
                        NULL },
                      { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } },
      NULL,
      1,
      MUNIT_SUITE_OPTION_NONE },
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE } },
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *const argv[]) { return munit_suite_main(&suite, NULL, argc, argv); }
