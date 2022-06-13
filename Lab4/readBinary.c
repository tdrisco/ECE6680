#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

int main(int argc, char *argv[]){
  if (argc != 2)
    {
    printf("Usage:  readBinary [filename]\n");
    exit(0);
    }

    FILE		*inputFile;
    unsigned char byte;

    // Open the inputed file for reading-----------------------------------------
    inputFile=fopen(argv[1],"rb");
    if (inputFile == NULL)
      {
        printf("Unable to open %s for reading\n",argv[2]);
        exit(0);
      }

    while(fread(&byte,sizeof(unsigned char),1,inputFile) == 1)
      printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));

    printf("\n");

  return 0;
}
