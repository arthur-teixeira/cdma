#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_CHIPS 8
#define BITS_PER_CHAR 8
#define CHIPS_PER_CHAR NUM_CHIPS * BITS_PER_CHAR

int A[NUM_CHIPS] = {-1, -1, -1, 1, 1, -1, 1, 1};
int B[NUM_CHIPS] = {-1, -1, 1, -1, 1, 1, 1, -1};
int C[NUM_CHIPS] = {-1, 1, -1, 1, 1, 1, -1, -1};
int D[NUM_CHIPS] = {-1, 1, -1, -1, -1, -1, 1, -1};

void send_char(int *channel, char message, int chip_sequence[NUM_CHIPS]) {
  for (int i = 0; i < BITS_PER_CHAR; i++) {
    bool current_bit = message >> (BITS_PER_CHAR - i - 1) & 1;

    for (int j = 0; j < NUM_CHIPS; j++) {
      int current_chip = chip_sequence[j];
      int inversor = current_bit ? 1 : -1;
      channel[i * NUM_CHIPS + j] += current_chip * inversor;
    }
  }
}

void send_str(int *channel, size_t channel_size, char *message,
              int chip_sequence[NUM_CHIPS]) {
  size_t message_size = strlen(message);
  assert(channel_size >= message_size * CHIPS_PER_CHAR);

  for (size_t i = 0; i < message_size; i++) {
    send_char(channel + (i * CHIPS_PER_CHAR), message[i], chip_sequence);
  }
}

char decode_char(int *channel, int chip_sequence[NUM_CHIPS]) {
  uint8_t buf = 0;

  for (int i = 0; i < BITS_PER_CHAR; i++) {
    int acc = 0;
    int *frame = channel + (i * NUM_CHIPS);
    for (int j = 0; j < NUM_CHIPS; j++) {
      acc += frame[j] * chip_sequence[j];
    }

    int result = (acc / NUM_CHIPS);
    if (result == 1) {
      buf = (buf << 1) | 1;
    } else if (result == -1) {
      buf = (buf << 1) | 0;
    } else {
      return 0;
    }
  }

  return buf;
}

void decode_str(int *channel, size_t channel_size,
                int chip_sequence[NUM_CHIPS]) {
  char buf[512] = {0};
  size_t i = 0;
  while (i < sizeof(buf)) {
    char next_char = decode_char(channel + (i * CHIPS_PER_CHAR), chip_sequence);
    if (next_char == 0) {
      break;
    }
    buf[i] = next_char;
    i++;
  }

  buf[i] = '\0';

  printf("%s\n", buf);
}

#define LEN(xs) sizeof(xs) / sizeof(*xs)

int main() {
  char *a_msg = "This message was sent over a multiplexed channel using CDMA!";
  char *b_msg = "\nThis \t message\n comes from\n B\n";
  char *c_msg = "this message comes from C!";
  char *d_msg = "And this one comes from D.";

  int channel[CHIPS_PER_CHAR * strlen(a_msg)];
  memset(channel, 0, sizeof(channel));

  send_str(channel, LEN(channel), a_msg, A);
  send_str(channel, LEN(channel), b_msg, B);
  send_str(channel, LEN(channel), c_msg, C);
  send_str(channel, LEN(channel), d_msg, D);

  decode_str(channel, LEN(channel), A);
  decode_str(channel, LEN(channel), B);
  decode_str(channel, LEN(channel), C);
  decode_str(channel, LEN(channel), D);

  return 0;
}
