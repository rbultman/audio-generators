#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int main (int argc, char **argv) {
   float c;
   uint32_t i;
   float mult = 1.2434545445;
   volatile float res = 0;

   c = pow(2, (float)1.0/12.0);
   c = log(c);

   for (i=0; i<100000000; i++) {
      res += exp(c * mult);
      mult += 0.00001;
   }
   printf ("mult = %f, result = %f\r\n", mult, res);
}
