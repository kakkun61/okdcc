#include <okdcc/logic.h>
#include <stdio.h>
#include <stdlib.h>

struct dcc_Packet makeExamplePacket(enum dcc_PacketTag const tag) {
  switch (tag) {
    case dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,
        .speedAndDirectionPacketForLocomotiveDecoders = {
          .address = 3,
          .direction = dcc_Forward,
          .flControl = false,
          .speed5Bit = 23,
          .directionMayBeIgnored = false,
          .emergencyStop = false
        },
      };
    case dcc_ResetPacketForAllDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_ResetPacketForAllDecodersTag,
      };
    case dcc_IdlePacketForAllDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_IdlePacketForAllDecodersTag,
      };
    case dcc_BroadcastStopPacketForAllDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_BroadcastStopPacketForAllDecodersTag,
        .broadcastStopPacketForAllDecoders = {
          .kind = dcc_BroadcastStopKind_Stop,
          .directionMayBeIgnored = false,
          .direction = dcc_Forward,
        },
      };
    case dcc_ResetPacketForMultiFunctionDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_ResetPacketForMultiFunctionDecodersTag,
        .resetPacketForMultiFunctionDecoders = {
          .address = 2355,
        },
      };
    default:
      fprintf(stderr, "Unsupported packet tag: %d\n", tag);
      exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s <N>\n"
            "\n"
            "N is `enum dcc_PacketTag`.\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  struct dcc_Packet const packet = makeExamplePacket((enum dcc_PacketTag) atoi(argv[1]));
  char buffer[1024];
  dcc_showPacket(buffer, sizeof(buffer), packet);
  printf("%s\n", buffer);
  return EXIT_SUCCESS;
}
