#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NUM_CHIPS 8
#define MESSAGE_SIZE 8
#define CHANNEL_SIZE MESSAGE_SIZE *NUM_CHIPS

int A[NUM_CHIPS] = {-1, -1, -1, 1, 1, -1, 1, 1};
int B[NUM_CHIPS] = {-1, -1, 1, -1, 1, 1, 1, -1};
int C[NUM_CHIPS] = {-1, 1, -1, 1, 1, 1, -1, -1};
int D[NUM_CHIPS] = {-1, 1, -1, -1, -1, -1, 1, -1};

void send_char(int channel[CHANNEL_SIZE], char message,
                  int chip_sequence[NUM_CHIPS]) {
  for (int i = 0; i < MESSAGE_SIZE; i++) {
    bool current_bit = message >> (MESSAGE_SIZE - i - 1) & 1;

    for (int j = 0; j < NUM_CHIPS; j++) {
      int current_chip = chip_sequence[j];
      int inversor = current_bit ? 1 : -1;
      channel[i * NUM_CHIPS + j] += current_chip * inversor;
    }
  }
}

char decode(int channel[CHANNEL_SIZE], int chip_sequence[NUM_CHIPS]) {
  uint8_t buf = 0;

  for (int i = 0; i < MESSAGE_SIZE; i++) {
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
    } // If result is 0, no signal was sent in this frame
  }

  return buf;
}

int main() {
  int channel[MESSAGE_SIZE * NUM_CHIPS] = {0};
  char a_msg = 'a';
  char b_msg = 'b';
  char c_msg = 'c';
  char d_msg = 'd';

  send_char(channel, a_msg, A);
  send_char(channel, b_msg, B);
  send_char(channel, c_msg, C);
  send_char(channel, d_msg, D);

  printf("a message: %c\n", decode(channel, A));
  printf("b message: %c\n", decode(channel, B));
  printf("c message: %c\n", decode(channel, C));
  printf("d message: %c\n", decode(channel, D));

  return 0;
}
