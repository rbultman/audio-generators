// Create a C array that maps a 10V, 10-octave range to a step
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

/*
 * The FM input ADC samples at a 48KHZ rate.  The ADC ISR is used to create a square
 * wave.  A 32-bit software-based counter is used to generate the square wave.  The
 * most-significant bit of the counter is used as the square wave output.  The 
 * frequency is controlled via a step size that is added onto the counter.  The 
 * equation for the step is:
 *          2^32
 *  Step = ------- X F
 *          48000
 *
 * where F is the desired output frequency.
 *
 * The code here generates a lookup table using the above equation, along with a 
 * control voltage (CV) as the input.  The CV is assumed to follow the 1V/octave 
 * standard whose value is read via ADC.  The index of the table is the ADC count
 * of the CV input.  The value looked up is the step required for the associated
 * frequency.
 *
 * The original code (possibly changed here) generated a table the covered the 
 * human audio range of approximately 20-20KHZ.  This is a 10-octave range that
 * is covered by 10 volts.
 */

uint32_t freqToStep(double f) {
   double fcalc = f * UINT32_MAX / 48000;
   return fcalc;
}

#define ADC_COUNTS (4096)
#define TABLE_SIZE (ADC_COUNTS*2)

int main (int argc, char **argv) {
   uint32_t i;
   double oneHalfTone = pow(2, 1.0/12.0);
   double fbase = 440 * pow(oneHalfTone, -53); // E0 (20.6 hz) is 53 half tones below A4 (440 hz)
   double fractionalTone = 120.0 / (TABLE_SIZE-1); // A 10-volt range gives 120 half-tones
   double freq;
   uint32_t step;

   printf("// Table starting frequency: %.2f Hz\r\n", fbase);
   printf("// Step values at frequencies of interest:\r\n");
   freq = 440 * pow(oneHalfTone, -9);
   printf("// C4 - %6.2f Hz : %u\r\n", freq, freqToStep(freq));
   printf("// A4 - %6.2f Hz : %u\r\n", 440.0, freqToStep(440));
   printf("//      1Kz       : %u\r\n", freqToStep(1000));

   // Define the table and calculat the first entry.
   printf("const uint32_t countToStepLookup[%d] = { \r\n   %10uu,", TABLE_SIZE, freqToStep(fbase));

   // Generate the rest of the table minus the last entry.
   for (i=1; i<TABLE_SIZE-1; i++) {
      freq = fbase * pow(oneHalfTone, i * fractionalTone);
      step = freqToStep(freq);
      printf(" %10uu,", step);
      if (i%8 == 7) printf(" // %8.2f Hz\r\n  ", freq);
   }

   // Calculate the last entry and finish the array definition.
   freq = fbase * pow(oneHalfTone, i * fractionalTone);
   step = freqToStep(freq);
   printf(" %10uu  // %8.2f Hz\r\n};\r\n", step, freq);

   printf("// Table ending frequency: %.2f Hz\r\n", freq);
}

