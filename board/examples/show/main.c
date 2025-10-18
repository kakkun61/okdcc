#include <okdcc/logic.h>
#include <stdio.h>
#include <stdlib.h>

size_t makeExamplePackets(enum dcc_PacketTag const tag, struct dcc_Packet packets[], size_t const packetsSize) {
  size_t i = 0;
  switch (tag) {
    case dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag:
      packets[i++] = (struct dcc_Packet){
        .tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,
        .speedAndDirectionPacketForLocomotiveDecoders = { .address = 3,
                                                          .direction = dcc_Forward,
                                                          .flControl = false,
                                                          .speed5Bit = 23,
                                                          .directionMayBeIgnored = false,
                                                          .emergencyStop = false },
      };
      packets[i++] = (struct dcc_Packet){
        .tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,
        .speedAndDirectionPacketForLocomotiveDecoders = { .address = 3,
                                                          .direction = dcc_Forward,
                                                          .flControl = true,
                                                          .speed4Bit = 15,
                                                          .fl = true,
                                                          .emergencyStop = false },
      };
      return i;
    case dcc_ResetPacketForAllDecodersTag:
      packets[i++] = (struct dcc_Packet){
        .tag = dcc_ResetPacketForAllDecodersTag,
      };
      return i;
    case dcc_IdlePacketForAllDecodersTag:
      packets[i++] = (struct dcc_Packet){
        .tag = dcc_IdlePacketForAllDecodersTag,
      };
      return i;
    case dcc_BroadcastStopPacketForAllDecodersTag:
      packets[i++] = (struct dcc_Packet) {
        .tag = dcc_BroadcastStopPacketForAllDecodersTag,
        .broadcastStopPacketForAllDecoders = {
          .kind = dcc_BroadcastStopKind_Stop,
          .directionMayBeIgnored = false,
          .direction = dcc_Forward,
        },
      };
      return i;
    case dcc_ResetPacketForMultiFunctionDecodersTag:
      packets[i++] = (struct dcc_Packet) {
        .tag = dcc_ResetPacketForMultiFunctionDecodersTag,
        .resetPacketForMultiFunctionDecoders = {
          .address = 2355,
        },
      };
      return i;
    case dcc_HardResetPacketForMultiFunctionDecodersTag:
      packets[i++] = (struct dcc_Packet) {
        .tag = dcc_HardResetPacketForMultiFunctionDecodersTag,
        .hardResetPacketForMultiFunctionDecoders = {
          .address = 2355,
        },
      };
      return i;
    case dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag:
      packets[i++] = (struct dcc_Packet){ .tag = dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag,
                                          .factoryTestInstructionPacketForMultiFunctionDecoders = { .address = 2355,
                                                                                                    .set = true,
                                                                                                    .dataExists = true,
                                                                                                    .data = 0xFF } };
      packets[i++] = (struct dcc_Packet){
        .tag = dcc_FactoryTestInstructionPacketForMultiFunctionDecodersTag,
        .factoryTestInstructionPacketForMultiFunctionDecoders = { .address = 2355, .set = false, .dataExists = false }
      };
      return i;
    case dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag:
      packets[i++] = (struct dcc_Packet){ .tag = dcc_SetDecoderFlagsPacketForMultiFunctionDecodersTag,
                                          .setDecoderFlagsPacketForMultiFunctionDecoders = {
                                            .address = 2355,
                                            .set = true,
                                            .subaddress = 7,
                                            .instruction = dcc_Disable111Instructions } };
      return i;
    case dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag:
      packets[i++] =
        (struct dcc_Packet){ .tag = dcc_SetExtendedAddressingPacketForMultiFunctionDecodersTag,
                             .setExtendedAddressingPacketForMultiFunctionDecoders = { .address = 2355, .set = true } };
      return i;
    default:
      fprintf(stderr, "Unsupported packet tag: %d\n", tag);
      exit(EXIT_FAILURE);
  }
}

#define PACKETS_SIZE 5

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s <N>\n"
            "\n"
            "N is `enum dcc_PacketTag`.\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  struct dcc_Packet packets[PACKETS_SIZE];
  size_t exampleCount = makeExamplePackets((enum dcc_PacketTag) atoi(argv[1]), packets, PACKETS_SIZE);
  printf("[");
  for (size_t i = 0; i < exampleCount; i++) {
    if (i != 0) printf(",");
    char buffer[1024];
    dcc_showPacket(buffer, sizeof(buffer), packets[i]);
    printf("%s", buffer);
  }
  printf("]\n");
  return EXIT_SUCCESS;
}
