#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  //Check for proper input of commandline arguments-----------------------------
  if (argc != 3)
    {
    printf("Usage:  Lab2 [compress or decompress] [filename]\n");
    exit(0);
    }
  //----------------------------------------------------------------------------

  //Intialize all the starting variables----------------------------------------
  FILE		*inputFile, *outputFile;
  Bool compression;
  unsigned char readValueA, readValueB, runCount, data;
  unsigned char maxRunCount = 255;
  char outputFileName[50];
  //----------------------------------------------------------------------------

  //Determine the action to be perfromed (compress file or decompress file)-----
  if(strcmp("compress",argv[1]) == 0)
    compression = True;
  else if(strcmp("decompress",argv[1]) == 0)
    compression = False;
  else
    {
      printf("Please enter a valid action (compress or decompress)\n");
      exit(0);
    }
  //----------------------------------------------------------------------------

  // Open the inputed image for reading-----------------------------------------
  inputFile=fopen(argv[2],"rb");
  if (inputFile == NULL)
    {
      printf("Unable to open %s for reading\n",argv[2]);
      exit(0);
    }
  //----------------------------------------------------------------------------

  //setup outputfile with proper extension--------------------------------------
  strcpy(outputFileName,argv[2]);
  int fileNameLength = strlen(outputFileName);
  outputFileName[fileNameLength-3] = '\0';
  if(compression)
    strcat(outputFileName,"rle");
  else
    strcat(outputFileName,"urle");
  //----------------------------------------------------------------------------

  //Open output file to be written to-------------------------------------------
  outputFile=fopen(outputFileName,"wb");
  if (outputFile == NULL)
    {
      printf("Unable to open %s for writing\n",outputFileName);
      exit(0);
    }
  //----------------------------------------------------------------------------

  if(compression)
    {
      runCount = 1;
      fread(&readValueA,sizeof(unsigned char),1,inputFile);
      while(fread(&readValueB,sizeof(unsigned char),1,inputFile) == 1)
        {
          if(readValueA == readValueB)
            {
              if(runCount == maxRunCount)
                {
                  fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                  fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                  readValueA = readValueB;
                  runCount = 1;
                }
              else runCount++;
            }
          else
            {
              fwrite(&runCount,sizeof(unsigned char),1,outputFile);
              fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
              readValueA = readValueB;
              runCount = 1;
            }
        }
      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
    }
  else
    {
      while(fread(&runCount,sizeof(unsigned char),1,inputFile) == 1)
        {
          fread(&data,sizeof(unsigned char),1,inputFile);

          for(int i = 0; i < runCount; i++)
            fwrite(&data,sizeof(unsigned char),1,outputFile);
        }
    }

  fclose(inputFile);
  fclose(outputFile);
  return 0;
}
