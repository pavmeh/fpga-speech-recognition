#ifndef __SYSTEM_GLOBALS_H__
#define __SYSTEM_GLOBALS_H__

#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

/******************
 ****  MACROS  ****
 ******************/

// Hardware FFT macros
#define FIXED_POINT
#define DOUBLE_PRECISION
//length of bel_fft (have to modify verilog before changing this)
#define FFT_LEN 128
//divide audio_config sampling rate (current 32kHz) by this number
//to get actual sampling rate
#define STEP_SIZE 4
 //number of samples desired
 //ammount of time recorded in seconds = NUM_SAMPLES / config sampling rate / STEP_SIZE
#define NUM_SAMPLES 8192

 //Number of cepstral coefficients to find
 #define NUM_CC 13
 // Number of triangle filter banks to use in computing MFCC
 #define NUM_BANKS 20

#define NUM_ROWS 2

 // Number of overlapping frames in a chunk of audio
#define numFrames (2*(NUM_SAMPLES / FFT_LEN) - 1)

#endif /* __SYSTEM_GLOBALS_H__ */
