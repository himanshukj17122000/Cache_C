#include <stdio.h>
#include <stdlib.h>
#include <cstring>

int log2(int n) {
    int r=0;
    while (n>>=1) r++;
        return r;
}

int ones(int n) {
    return ((1<<n) - 1);
}

int lastdigits(int num, int n) {
    int oness = ones(n);
    return num&oness;
}

int main(int argc, char ** argv) {
    FILE* fileused = fopen(argv[1], "r");
    char address[24];
    char size[24];
    char page[24];
    fscanf(fileused, "%s", address);
    int addressinbits = atoi(address);
    fscanf(fileused, "%s", size);
    int pageinint = atoi(size);
    int shiftAm = log2(pageinint);
    int Addresearch = (int)strtol(argv[2], NULL, 16);
    int offset = lastdigits(Addresearch, shiftAm);
    Addresearch = Addresearch>>shiftAm;
    int virtuals = 0;
    int physicals = -1;
    
     while (virtuals != Addresearch) {
              fscanf(fileused, "%s\n", page);
              virtuals= virtuals + 1;
          }
        fscanf(fileused, "%s\n", page);
        physicals = atoi(page);
          if(physicals != -1) {
              physicals = physicals << shiftAm;
              physicals = physicals | offset;
              printf("%x\n", physicals);
              
          }
          else {
              printf("PAGEFAULT\n");
          }
       
          fclose(fileused);
          return EXIT_SUCCESS;
      }
