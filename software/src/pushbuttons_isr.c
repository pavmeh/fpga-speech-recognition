#include "system_globals.h"
#include "ee109-lib/pushbuttons.h"

/**
 * Function: pushbuttons_isr
 * ------------------------------------------
 */
 extern volatile bool samples_for_fft_requested;
 extern volatile bool working;
 
void pushbuttons_isr (void *context, unsigned int id)
{
  uint32_t edges = pushbuttons_get_edge_capture ();
  if (edges & BUTTON1 && !working)
    {
      samples_for_fft_requested = true;
      working = true;
    }
  pushbuttons_clear_edge_capture ();
}
