#include <okdcc/logic.h>

#include <stdio.h>
#include <stdlib.h>

struct dcc_Packet makeExamplePacket(enum dcc_PacketTag const tag) {
  switch (tag) {
    case dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag:
      return (struct dcc_Packet) {
        .tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,
        .speedAndDirectionPacketForLocomotiveDecoders = {
          .direction = dcc_Forward,
          .flControl = false,
          .speed5Bit = 23,
          .directionMayBeIgnored = false,
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
      "N is:\n"
      "\t0: Speed and direction packet for locomotive decoders\n",
      argv[0]);
    return EXIT_FAILURE;
  }
  struct dcc_Packet const packet = makeExamplePacket((enum dcc_PacketTag) atoi(argv[1]));
  char buffer[1024];
  dcc_showPacket(buffer, sizeof(buffer), packet);
  printf("%s\n", buffer);
  return EXIT_SUCCESS;
}
