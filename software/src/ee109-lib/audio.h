#include <stdint.h>

#define AUDIO_BUF_SIZE 128

void audio_init (void *isr);

uint32_t audio_read (int *left_buffer, int *right_buffer, uint32_t count);

uint32_t audio_write (int *left_buffer, int *right_buffer, uint32_t count);

void audio_clear_read_fifo ();

void audio_clear_write_fifo ();
