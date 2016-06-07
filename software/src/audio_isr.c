#include "ee109-lib/audio.h"
#include "ee109-lib/red_leds.h"
#include "belfft/kiss_fft.h"
#include "system_globals.h"
#include <stdbool.h>
#include <stdint.h>

// From system_globals.c
static volatile int left_buffer[AUDIO_BUF_SIZE];
static volatile int right_buffer[AUDIO_BUF_SIZE];
extern volatile kiss_fft_cpx samples_for_fft[FFT_LEN];
extern volatile bool samples_for_fft_requested;
extern volatile size_t numRead;
static size_t startVal = 0;
// from main.c
int fft ();
void draw_fft ();
void signal_audio_ready ();

int clamp (int value, int ceiling)
{
  if (value < ceiling) return 0;
  else return value;
}

void audio_isr (void *context, unsigned int id) {
  uint32_t numToRead = (samples_for_fft_requested) ? (NUM_SAMPLES - numRead) : AUDIO_BUF_SIZE;
  // Want to read step
  size_t count = audio_read (left_buffer, right_buffer, STEP_SIZE*numToRead);
  audio_write (left_buffer, right_buffer, count);
  if (samples_for_fft_requested) {
      size_t i;
      red_leds_set (0xFF);
      //modify count by STEP_SIZE since only count/STEP_SIZE samples actually copied
      size_t numSamples = count/STEP_SIZE;
      if (numRead + numSamples >= NUM_SAMPLES) {
        numRead = 0;
        samples_for_fft_requested = false;
        signal_audio_ready();
        startVal = 0;
      }
      for (i = startVal; i < count; i+= STEP_SIZE) {
        samples_for_fft[i/STEP_SIZE + numRead].r = left_buffer[i];
      }
      numRead += numSamples;
      //maintian proper spacing on next set of samples when next interrupt received
      startVal = count%STEP_SIZE;
      red_leds_clear (0xFF);
    }
}
