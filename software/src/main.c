/* Standard. */
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/* Altera. */
#include "system.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"

/* EE109. */
#include "isr.h"
#include "system_globals.h"
#include "ee109-lib/colors.h"
#include "ee109-lib/switches.h"
#include "ee109-lib/pushbuttons.h"
#include "ee109-lib/vga.h"
#include "ee109-lib/lcd.h"
#include "ee109-lib/char_lcd.h"
#include "ee109-lib/red_leds.h"
#include "ee109-lib/green_leds.h"
#include "ee109-lib/hex.h"
#include "ee109-lib/audio.h"
#include "ee109-lib/ethernet.h"
#include "ee109-lib/rs232.h"

#include "belfft/bel_fft.h"
#include "belfft/kiss_fft.h"

#define LCD_CHAR_DISPLAY_SIZE_X 16
#define LCD_CHAR_DISPLAY_SIZE_Y 2

/********************************
 ****  GLOBALS DECLARATIONS  ****
 ********************************/

// Number of FFT points to analyze, since the magnitude FFT of a 
// real signal is symmetric
#define numFFTPoints (FFT_LEN/2 +1)

// From system_globals.c

extern volatile kiss_fft_cpx samples_for_fft[NUM_SAMPLES];
extern volatile bool samples_for_fft_requested;
extern volatile bool working;
extern double mat[NUM_ROWS][NUM_CC*numFrames];
extern float TRIBANK[20][65];
extern float DCT_LIFTER[13][20]; 
extern int tribankStart[20];
extern int tribankEnd[20];
// For hardware FFT
kiss_fft_cfg fft_cfg;
static volatile double yesBuffer[NUM_SAMPLES * 2];
static volatile double noBuffer[NUM_SAMPLES * 2];
static volatile double sampleBuffer[NUM_SAMPLES * 2];
static volatile double windowedSampleBuffer[numFrames*numFFTPoints];
static double sampleMFCC[NUM_CC*numFrames];
volatile bool audio_ready = false;
static bool bruteForce = false;



/*******************************
 ****  FUNCTION PROTOTYPES  ****
 *******************************/
/* General functions */
static int initialize (void);
static void run (void);
static void clearLCDChar();
/* Hardware FFT functions */
int fft (double *outputBuffer);
void signal_audio_ready ();

/*Speech recognition functions*/
static void mfcc(double*, double*);
static void compareAndPrint();

/* Configuration functions */
static void configure_interrupts ();
static int configure_fft ();
static void training();
static void fft_mag_window(double*);

/********************************
 ****  FUNCTION DEFINITIONS  ****
 ********************************/

/**
 * Function: main
 * --------------
 * main simply calls initialize() then run().
 */

void testMFCC() {
  int i;
  for(i = 0; i < NUM_SAMPLES; i++) {
    samples_for_fft[i].r = 1000*i;
  }
  fft_mag_window(windowedSampleBuffer);
  mfcc(windowedSampleBuffer, sampleMFCC);
  for (i=0; i < NUM_CC*numFrames; i++) {
    printf("%lf", sampleMFCC[i]);
  }
}

int main(void) {
  /* Initialize devices and introduce application. */
  if (initialize () != 0)
    return 1;
  //testMFCC();
  // /* Repeatedly checks state and makes updates. */
  run ();

  return 0;
}

/**
 * Function: initialize
 * --------------------
 * Prepare all interrupts and interfaces. Returns 0 on success, nonzero otherwise.
 */

static void training() {
    char_lcd_move_cursor(0,0);
    char_lcd_write("Push BTN1");
    char_lcd_move_cursor(0,1);
    char_lcd_write("And say Yes");
    while(!audio_ready);
    clearLCDChar();
    char_lcd_move_cursor(0,0);
    char_lcd_write("Processing");
    fft(yesBuffer);
    audio_ready = false;
    working = false;

    clearLCDChar();
    char_lcd_move_cursor(0,0);
    char_lcd_write("Push BTN1");
    char_lcd_move_cursor(0,1);
    char_lcd_write("And say No");
    while(!audio_ready);
    clearLCDChar();
    char_lcd_move_cursor(0,0);
    char_lcd_write("Processing");
    fft(noBuffer);
    audio_ready = false;
    working = false;
}

static void getMat() {
  char ch;
  int i;
  char buf[16];
  size_t index;
  int j;
  printf("waiting to read...\n");
  for(i = 0; i < NUM_ROWS; i++) {
    for(j = 0; j < NUM_CC; j++) {
      index = 0;
      while((ch=get_char()) != ',') {
        if(ch != '\0') {
          buf[index] = ch;
          index++;
        }
      }
      buf[++index] = '\0';
      sscanf(buf,"%lf",(&(mat[i][j])));
    }
  }
  printf("done loading\n");
  printf("mat[0][0]: %lf\n", mat[0][0]);
}

static int initialize (void) {
  //configure_lcd ();
  configure_interrupts ();

  if (configure_fft() != 0) {
    return 1;
  }
  if(bruteForce) {
    training();
  } else{
    //do nothing for now
  }
  return 0;
}

/* Set up audio and pushbutton interrupts. */
static void configure_interrupts () {
  audio_init (audio_isr);
  pushbuttons_enable_interrupts (pushbuttons_isr);
  pushbuttons_set_interrupt_mask (BUTTON2|BUTTON1);
}

static void clearLCDChar() {
  char_lcd_move_cursor(0,0);
  int i;
  for (i = 0; i < LCD_CHAR_DISPLAY_SIZE_X; i++) {
    int j;
    for (j = 0; j < LCD_CHAR_DISPLAY_SIZE_Y; j++) {
      char_lcd_write(" ");
      char_lcd_move_cursor(i, j);
    }
  }
  char_lcd_cursor_off();
}

/* Allocate the FFT configuration stryct and prepare sample array. */
static int configure_fft () {
  fft_cfg = kiss_fft_alloc (FFT_LEN, 0, NULL, 0);
  if (! fft_cfg) {
    printf ("Error: Cannot allocate memory for FFT control structure.\n");
    return 1;
  }
  return 0;
}

void printMFCC(double r) {
  char buf[20];
  snprintf(buf, 20, "%.6lf,", r);
  size_t j;
  for (j = 0; j < strlen(buf); j++) {
    while(put_char(buf[j]));
  }
}

/**
 * Function: run
 * -------------
 * Request audio, then perform an FFT and draw it. Repeat.
 */
void run (void) {
  while (true) {
    clearLCDChar();
    char_lcd_move_cursor(0,0);
    char_lcd_write("Push BTN1");
    char_lcd_move_cursor(0,1);
    char_lcd_write("To Operate");
    //samples_for_fft_requested = true; // Request audio
    while (!audio_ready); // Wait for audio
    green_leds_set (0xFF);
    clearLCDChar();
    char_lcd_move_cursor(0,0);
    char_lcd_write("Processing");
    if (bruteForce) fft (sampleBuffer);
    else {
      fft_mag_window(windowedSampleBuffer);
      mfcc(windowedSampleBuffer, sampleMFCC);
    }
    green_leds_clear (0xFF);
    
    //********** code used while running ************
    compareAndPrint();
    //*********code used while collecting data *************
    // int i;
    // for (i = 0; i < NUM_CC*numFrames; i++) {
    //   printMFCC(sampleMFCC[i]);
    // }
    //end
    audio_ready = false;
    working = false;
  }
}

/**
 * Function: fft
 * -------------
 * Perform a hardware FFT on the samples_for_fft array.
 * Also store historical FFT outputs in a ring buffer of length AVERAGING_LENGTH
 * so the animation is smoother.
 */
int fft (double* output) {
  int i, j;
  kiss_fft_cpx fft_output[FFT_LEN];
  for (j = 0; j < NUM_SAMPLES; j+= FFT_LEN)
  {
    kiss_fft (fft_cfg, samples_for_fft + j, fft_output);
    double norm = 0;
    for (i = 0; i < FFT_LEN; i++)
      {
        norm += (double)fft_output[i].r*(double)fft_output[i].r + (double)fft_output[i].i*(double)fft_output[i].i;
      }
    norm = sqrt(norm);

    for (i = 0; i < FFT_LEN*2; i+=2)
      {
        output[i + 2*j] = (double)fft_output[i/2].r / norm;
        output[i + 2*j + 1] = (double)fft_output[i/2].i / norm;
      }
  }
  return 0;
}

void fft_mag_window (double* output) {
  int i, j;
  kiss_fft_cpx fft_output[FFT_LEN];
  double mag2;
  // Overlap by FFT_LEN / 2
  // Results in 2*NUM_SAMPLES / FFT_LEN - 1 overlapping frames
  // because we loose one at the end
  int offset = FFT_LEN / 2;
  for (j = 0; j < numFrames; j++)
  {
    kiss_fft (fft_cfg, samples_for_fft + j*offset, fft_output);
    double norm = 0;
    for (i = 0; i < FFT_LEN; i++)
      {
        //printf("%d %d\n", fft_output[i].r, fft_output[i].i);
        mag2 = (double)fft_output[i].r*(double)fft_output[i].r + (double)fft_output[i].i*(double)fft_output[i].i;
           // if(mag2 == 0) printf("mag2 %d %d\n", j, i);
        if (i < numFFTPoints){
          // Offset by j*offset
          output[i + j*numFFTPoints] = sqrt(mag2);
        } 
        norm += mag2; 
      }
      // FFT is symmetric, so magnitudes of negative freqs will repeat
    norm = sqrt(norm);
    // Normalize
    for (i = 0; i < numFFTPoints; i++)
      {
        output[i + j*numFFTPoints] = output[i + j*numFFTPoints] / norm;
        //printf("f:%lf\n", output[i + j*numFFTPoints]);
      }
  }
}

static void compareAndPrint() {
  if(bruteForce) {
    int i, j, offset_index;
    double dot_yes, dot_no;
    double dot_yes_max = (double) -INT_MAX;
    double dot_no_max = (double) -INT_MAX;
    int nFFTs = NUM_SAMPLES / FFT_LEN;
    for (j = 0; j < 2*nFFTs; j+= 2*FFT_LEN)
    {
      dot_yes = 0;
      dot_no = 0;
      for (i = 0; i < NUM_SAMPLES*2; i++)
       {
        offset_index = (i + j) % (2*NUM_SAMPLES);
        dot_yes += yesBuffer[i]*sampleBuffer[offset_index];
        dot_no += noBuffer[i]*sampleBuffer[offset_index];
       }
      if (dot_yes > dot_yes_max) dot_yes_max = dot_yes;
      if (dot_no > dot_no_max) dot_no_max = dot_no;
    }
    printf("%lf, %lf\n", dot_yes_max, dot_no_max);
    if (dot_yes_max >= dot_no_max) printf("Yes\n");
    else printf("No\n");
  }
  else {
    double prob[NUM_ROWS] = {0.0, 0.0};
    int j;
    //unrolled loop
    for (j = 0; j < NUM_CC*numFrames; j++) {
      prob[0] += mat[0][j] * sampleMFCC[j];
      prob[1] += mat[1][j] * sampleMFCC[j];
    }
    printf("%lf, %lf\n", prob[0], prob[1]);
    if (prob[0] >= prob[1]) printf("Yes\n");
    else printf("No\n");
  }
}
/* Called by the audio ISR to signal the main loop that samples are ready
 * for the FFT. */
inline void signal_audio_ready () {
  audio_ready = true;
}


// MFCC Finder
static void mfcc(double* frames, double* output) {
  //j is only used if last DCT_LIFTER is not unrolled
  int i, j, m, start, end;
  // Filter bank energies
  double FBE[numFrames*NUM_BANKS];
  double sum;
  // Apply triangle filter bank and take log
  for (m = 0; m < numFrames; m++) {
    for (i = 0; i < NUM_BANKS; i++) {
      sum = 0;
      //use LUT to skip zeros
      start = tribankStart[i];
      end = tribankEnd[i];
      for (j = start; j < end; j++) {
        sum += TRIBANK[i][j]*frames[j + m*numFFTPoints];
        //if(m == 0 && (frames[j + m*numFFTPoints] == 0)) printf(" %lf\n", sum);
      }
      FBE[i + m*NUM_BANKS] = (sum <= 0) ? -100000 : log10(sum);
    }
    // DCT and lifter
    for (i = 0; i < NUM_CC; i++) {
      sum = 0;
      //loop unrolled
      //for (j = 0; j < NUM_BANKS; j++) {
        //sum += DCT_LIFTER[i][j]*FBE[j + m*NUM_BANKS];
        sum += DCT_LIFTER[i][0]*FBE[0 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][1]*FBE[1 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][2]*FBE[2 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][3]*FBE[3 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][4]*FBE[4 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][5]*FBE[5 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][6]*FBE[6 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][7]*FBE[7 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][8]*FBE[8 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][9]*FBE[9 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][10]*FBE[10 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][11]*FBE[11 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][12]*FBE[12 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][13]*FBE[13 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][14]*FBE[14 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][15]*FBE[15 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][16]*FBE[16 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][17]*FBE[17 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][18]*FBE[18 + m*NUM_BANKS];
        sum += DCT_LIFTER[i][19]*FBE[19 + m*NUM_BANKS];
      //}
      output[i + m*NUM_CC] = sum;
    }
  }
}