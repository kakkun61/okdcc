#include <okdcc/logic.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  struct dcc_Packet const packet = {
    .tag = dcc_SpeedAndDirectionPacketForLocomotiveDecodersTag,
    .speedAndDirectionPacketForLocomotiveDecoders = {
      .direction = dcc_Forward,
      .flControl = false,
      .speed5Bit = 23,
      .directionMayBeIgnored = false,
    },
  };
  char buffer[1024];
  int const result = dcc_showPacket(buffer, sizeof(buffer), packet);
  printf("%d\n", result);
  printf("%s\n", buffer);
  return EXIT_SUCCESS;
}
