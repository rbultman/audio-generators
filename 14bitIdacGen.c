/*
 * Generate 3 tables to be used for a summing DAC sine generator.
 * One table is used for each DAC.
 */

char lowHeaderText[] = "// idacValuesLow.h\r\n" \
                        "// IDAC values for the low range to be summed with the mid and high outputs.\r\n" \
                        "\r\n" \
                        "#ifndef IDAC_VALUES_LOW_H\r\n" \
                        "#define IDAC_VALUES_LOW_H\r\n" \
                        "\r\n" \
                        "#include <stdint.h>\r\n" \
                        "\r\n" \
                        "#define LOW_IDAC_VALUES_COUNT 16384\r\n"
                        "extern uint8_t iDacValuesLow[LOW_IDAC_VALUES_COUNT];\r\n" \
                        "\r\n" \
                        "#endif\r\n";

char midHeaderText[] = "// idacValuesMid.h\r\n" \
                        "// IDAC values for the mid range to be summed with the low and high outputs.\r\n" \
                        "\r\n" \
                        "#ifndef IDAC_VALUES_MID_H\r\n" \
                        "#define IDAC_VALUES_MID_H\r\n" \
                        "\r\n" \
                        "#include <stdint.h>\r\n" \
                        "\r\n" \
                        "#define MID_IDAC_VALUES_COUNT 16384\r\n"
                        "extern uint8_t iDacValuesMid[MID_IDAC_VALUES_COUNT];\r\n" \
                        "\r\n" \
                        "#endif\r\n";

char highHeaderText[] = "// idacValuesHigh.h\r\n" \
                        "// IDAC values for the high range to be summed with the low and mid outputs.\r\n" \
                        "\r\n" \
                        "#ifndef IDAC_VALUES_HIGH_H\r\n" \
                        "#define IDAC_VALUES_HIGH_H\r\n" \
                        "\r\n" \
                        "#include <stdint.h>\r\n" \
                        "\r\n" \
                        "#define HIGH_IDAC_VALUES_COUNT 16384\r\n"
                        "extern uint8_t iDacValuesHigh[HIGH_IDAC_VALUES_COUNT];\r\n" \
                        "\r\n" \
                        "#endif\r\n";

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#define MAX_VAL 16383

uint8_t createHeaderFile(char *pFilename, char *content) {
   FILE *fp = fopen(pFilename, "w");

   if (fp == NULL) {
      printf("Error opening %s\r\n", pFilename);
      return 1;
   }
   fprintf(fp, "%s", content);
   fclose(fp);

   return 0;
}

void copyStringAndConvertToUpperCase(char *dest, char *src) {
   do {
      *dest++ = toupper(*src);
   } while (*src++ != 0);
}

FILE *openCFileWithHeader(char *rangeName) {
   FILE *fp;
   char filename[128];
   char rangeUpperCase[32];

   copyStringAndConvertToUpperCase(rangeUpperCase, rangeName);

   sprintf(filename, "idacValues%s.c", rangeName);

   printf("Open C file %s\r\n", filename);

   fp = fopen(filename, "w");

   if (fp == NULL) {
      printf("Error creating file %s\r\n", filename);
      return NULL;
   }

   fprintf(fp, "// idaValues%s.c\r\n", rangeName);
   fprintf(fp, "// IDAC values for the %s range\n", rangeName);
   fprintf(fp, "\r\n");
   fprintf(fp, "#include \"idacValues%s.h\"\r\n", rangeName);
   fprintf(fp, "\r\n");
   fprintf(fp, "uint8_t iDacValues%s[%s_IDAC_VALUES_COUNT] = {\r\n", rangeName, rangeUpperCase);

   return fp;
}

void writeDacValuesToFiles(FILE *fpLow, FILE *fpMid, FILE *fpHigh) {
   uint16_t i;
   uint8_t low;
   uint8_t mid;
   uint8_t high;
   float sineVal;
   uint32_t sineTrunc;
   
   sineVal = 0.5;
   sineTrunc = MAX_VAL * sineVal;
   low = sineTrunc & 7;
   mid = (sineTrunc >> 3) & 0xf;
   high = (sineTrunc>>6) & 0xfe;
   fprintf(fpLow , "   /* 0x0000 */ 0x%02x, ", low);
   fprintf(fpMid , "   /* 0x0000 */ 0x%02x, ", mid);
   fprintf(fpHigh, "   /* 0x0000 */ 0x%02x, ", high);

   for (i=1; i<0x3fff; i++) {
      sineVal = (1 + sin(2 * M_PI * i / MAX_VAL))/2;
      sineTrunc = MAX_VAL * sineVal;
      printf("sin(%d) = %f, DAC = %u\r\n", i, sineVal, sineTrunc);
      low = sineTrunc & 7;
      mid = (sineTrunc >> 3) & 0xf;
      high = (sineTrunc>>6) & 0xfe;

      fprintf(fpLow, "0x%02x, ", low);
      fprintf(fpMid, "0x%02x, ", mid);
      fprintf(fpHigh, "0x%02x, ", high);

      if (i % 8 == 7) {
         fprintf(fpLow , "\r\n   /* 0x%04x */ ", i+1);
         fprintf(fpMid , "\r\n   /* 0x%04x */ ", i+1);
         fprintf(fpHigh, "\r\n   /* 0x%04x */ ", i+1);
      }
   }

   sineVal = (1 + sin(2 * M_PI * i / MAX_VAL))/2;
   sineTrunc = MAX_VAL * sineVal;
   printf("sin(%d) = %f, DAC = %u\r\n", i, sineVal, sineTrunc);
   low = sineTrunc & 7;
   mid = (sineTrunc >> 3) & 0xf;
   high = (sineTrunc>>6) & 0xfe;

   fprintf(fpLow, "0x%02x\r\n};\r\n", low);
   fprintf(fpMid, "0x%02x\r\n};\r\n", mid);
   fprintf(fpHigh, "0x%02x\r\n};\r\n", high);
}

int main(int argc, char **argv) {
   FILE *fpLow;
   FILE *fpMid;
   FILE *F_High;

   if(createHeaderFile("idacValuesLow.h", lowHeaderText)) return 1;
   if(createHeaderFile("idacValuesMid.h", midHeaderText)) return 1;
   if(createHeaderFile("idacValuesHigh.h", highHeaderText)) return 1;

   fpLow = openCFileWithHeader("Low");
   if (fpLow == NULL) {
      puts("Error creating file for low idac values.");
      return 1;
   }

   fpMid = openCFileWithHeader("Mid");
   if (fpMid == NULL) {
      puts("Error creating file for mid idac values.");
      fclose(fpLow);
      return 1;
   }

   F_High = openCFileWithHeader("High");
   if (F_High == NULL) {
      puts("Error creating file for high idac values.");
      fclose(fpLow);
      fclose(fpMid);
      return 1;
   }

   writeDacValuesToFiles(fpLow, fpMid, F_High);

   fclose(fpLow);
   fclose(fpMid);
   fclose(F_High);

   return 0;
}
