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
  unsigned char readValueC, readValueD;
  unsigned char maxRunCount = 255;
  unsigned char marker = 0;
  char outputFileName[50];
  int i = 0;
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
printf("\n");
  if(compression)
    {
      runCount = 1;
      fread(&readValueA,sizeof(unsigned char),1,inputFile);
      //printf("%c\n",readValueA);
      while(fread(&readValueB,sizeof(unsigned char),1,inputFile) == 1)
        {
          //printf("top");
          if(readValueA == readValueB)
            {
              printf("top");
              if(runCount == maxRunCount)
                {
                  printf("#");
                  printf("%c",readValueA);
                  fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                  fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                  readValueA = readValueB;
                  runCount = 1;
                }
              else runCount++;
            }
          else
            {
              //consecutive symbols means rle is on
              if(runCount >= 2)
                {
                  //printf("here ");
                  printf("#");
                  printf("%c",readValueA);
                  fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                  fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                  readValueA = readValueB;
                  runCount = 1;
                }
              else
                {
                  //This means that we know a an b are different
                  //Need to check whether or not next two bytes also different
                  if(readValueA == marker || readValueB == marker)
                    {
                      //if end of file don't bother to flip compression
                      printf("HeRe");
                      printf("%c",readValueA);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      runCount = 1;
                    }
                  else if(fread(&readValueC,sizeof(unsigned char),1,inputFile) != 1 || readValueC == marker)
                    {
                      //if end of file don't bother to flip compression
                      //need to rewind by one bit so it comes back and deals with C
                      //fseek(inputFile, -1, SEEK_CUR);
                      printf("code break here");
                      printf("#");
                      printf("%c",readValueA);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueB,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueC;
                      runCount = 1;
                    }

                  else if(fread(&readValueD,sizeof(unsigned char),1,inputFile) != 1 || readValueD == marker)
                    {
                      //if end of file don't bother to flip compression
                      //need to rewind by two byte so it comes back and deals with C and D
                      //fseek(inputFile, -2, SEEK_CUR);
                      printf("#");
                      printf("%c",readValueA);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueB,sizeof(unsigned char),1,outputFile);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueC,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueD;
                      runCount = 1;
                    }
                  //check for the differences here determine if compression gets turned off
                  //else if(readValueB != readValueC && readValueC != readValueD)
                  else
                    {
                      //printf("\n");
                      printf("marker ");
                      printf("%c",readValueA);
                      printf("%c",readValueB);
                      printf("%c",readValueC);
                      fwrite(&marker,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueB,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueC,sizeof(unsigned char),1,outputFile);
                      //printf("\n");
                      //fwrite(&readValueD,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueD;
                      while((fread(&readValueB,sizeof(unsigned char),1,inputFile) == 1) && readValueA != readValueB && readValueB != marker)
                      {
                        printf("%c1",readValueA);
                        fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                        readValueA = readValueB;
                        runCount = 1;
                      }
                      printf("lost");
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      printf(" marker");
                      fwrite(&marker,sizeof(unsigned char),1,outputFile);
                      runCount = 1;
                    }
                }

            }
        }
      //printf("#");
      //printf("%c",readValueA);
      if(runCount > 1)
      {
        fwrite(&runCount,sizeof(unsigned char),1,outputFile);
        fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
      }

    }
  else //Decompression----------------------------------------------------------
    {
      //printf("\n");
      while(fread(&runCount,sizeof(unsigned char),1,inputFile) == 1)
        {
          printf("H");
          if(runCount != marker)
            {
              fread(&data,sizeof(unsigned char),1,inputFile);

              for(i = 0; i < runCount; i++)
                {
                  fwrite(&data,sizeof(unsigned char),1,outputFile);
                  printf("%c",data);
                }

            }
          else
            {
              while(fread(&data,sizeof(unsigned char),1,inputFile) == 1)
                {
                  if(data != marker)
                    {
                      fwrite(&data,sizeof(unsigned char),1,outputFile);
                      printf("%c",data);
                    }
                  else
                    break;

                }
            }

        }
    }

  printf("\n");
  fclose(inputFile);
  fclose(outputFile);
  return 0;
}
