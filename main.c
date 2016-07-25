#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int main (int argc, char **argv) {
   float c;
   int i;
   float looped;

   c = pow(2, (float)1.0/12.0);

   printf("c = %f\r\n", c);

   printf("Log of c = %f\r\n", log(c));
   printf("5th semitone multiplier via pow = %f\r\n", pow(c, 5));
   printf("5th semitone multiplier via exp = %f\r\n", exp(5 * log(c)));

   looped = c;
   for (i=1; i<5; i++) {
      looped *= c;
   }

   printf("5th semitone multiplier via loop = %f\r\n", looped);

}
