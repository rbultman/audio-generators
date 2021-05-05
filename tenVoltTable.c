// Create a table that maps a 10V, 13-bit range to frequency
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

/*
 * 12000000 is the counter clock divided by 2
 * 32727 is the scaling since the FM input will be 16 bits signed
 * 8 is because the up/down counter is divide by 8
 */
uint32_t freqToPeriod(double f) {
   //return (12000000.0*32767.0)/(8.0*f);
   static double oldCalc = 9999999999;
   double calc = (1500000.0*32767.0)/f;
   uint32_t smallVal;
   uint64_t bigVal;
   if (calc > oldCalc) printf("\r\nNew is greater than old.\r\n");
   oldCalc = calc;
   bigVal = (uint64_t)calc;
   smallVal = (uint32_t)calc;
   if (bigVal != smallVal) printf("\r\nIt looks like the number does not fit. %u %lu\r\n", smallVal, bigVal);
   return (1500000.0*32767.0)/f;
}

#define VOLTS_PER_OCTAVE (1.0)
#define VCC (5.0)
#define HALF_STEPS_PER_OCTAVE (12)
#define HALF_STEPS_OVER_VCC (VCC * HALF_STEPS_PER_OCTAVE)
#define ADC_COUNTS (4096)
#define CV_INPUT_MAX (20)
#define TABLE_SIZE (ADC_COUNTS*CV_INPUT_MAX/VCC)

int main (int argc, char **argv) {
   uint32_t i;
   double oneHalfTone = pow(2, 1.0/12.0);
   double fbase = 440 * pow(oneHalfTone, -53); // E0 (20.6 hz) is 53 half tones below A4 (440 hz)
   uint32_t period;
   double freq;
   double fractionalTone = 1.0 * CV_INPUT_MAX * HALF_STEPS_OVER_VCC / VCC / ((float)TABLE_SIZE - 1.0);

   // move starting position down some number of octaves
   fbase = fbase * pow(oneHalfTone, -120);

   printf("Fractional tone: %f\r\n", fractionalTone);
   printf ("// Table starting frequency: %f\r\n", fbase);
   printf("const uint32_t countToPeriodLookup[%d] = { \r\n   %uu,", (uint32_t)TABLE_SIZE, freqToPeriod(fbase));
   for (i=1; i<TABLE_SIZE-1; i++) {
      //freq = fbase * pow(oneHalfTone, 60.0 * i / (ADC_COUNTS-1));
      freq = fbase * pow(oneHalfTone, i * fractionalTone);
      period = freqToPeriod(freq);
      printf(" %uu /* %f */,", period, freq);
      if (i%8 == 7) printf(" // %f Hz\r\n  ", freq);
   }
   //freq = fbase * pow(oneHalfTone, 60.0 * i / (ADC_COUNTS-1));
   freq = fbase * pow(oneHalfTone, i * fractionalTone);
   period = freqToPeriod(freq);
   printf(" %uu\r\n};\r\n", period);
   printf("// Table ending frequency: %f\r\n", freq);
}
