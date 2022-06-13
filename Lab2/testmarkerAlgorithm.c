#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  int compression;
  unsigned char readValueA, readValueB, runCount, data;
  unsigned char readValueC, readValueD, readValueE;
  unsigned char maxRunCount = 255;
  unsigned char marker = 0;
  char outputFileName[50];
  int i = 0;
  //----------------------------------------------------------------------------

  //Determine the action to be perfromed (compress file or decompress file)-----
  if(strcmp("compress",argv[1]) == 0)
    compression = 1;
  else if(strcmp("decompress",argv[1]) == 0)
    compression = 0;
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
              //consecutive symbols means rle is on
              if(runCount >= 2)
                {
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
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      runCount = 1;
                    }
                  else if(fread(&readValueC,sizeof(unsigned char),1,inputFile) != 1 || readValueC == marker)
                    {
                      //if end of file don't bother to flip compression
                      //need to rewind by one bit so it comes back and deals with C
                      fseek(inputFile, -1, SEEK_CUR);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      runCount = 1;
                    }

                  else if(fread(&readValueD,sizeof(unsigned char),1,inputFile) != 1 || readValueD == marker)
                    {
                      //if end of file don't bother to flip compression
                      //need to rewind by two byte so it comes back and deals with C and D
                      fseek(inputFile, -2, SEEK_CUR);
                      fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      runCount = 1;
                    }
                  //check for the differences here determine if compression gets turned off
                  else if(readValueB != readValueC && readValueC != readValueD)
                    {
                      fwrite(&marker,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueB,sizeof(unsigned char),1,outputFile);
                      fwrite(&readValueC,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueD;
                      while((fread(&readValueB,sizeof(unsigned char),1,inputFile) == 1) && readValueA != readValueB && readValueB != marker)
                      {
                        fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                        readValueA = readValueB;
                        runCount = 1;
                      }
                      fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                      readValueA = readValueB;
                      fwrite(&marker,sizeof(unsigned char),1,outputFile);
                      runCount = 1;
                    }
                  else
                    {
                      //this means c and d are the same
                      //check if the next byte matched d
                      if(fread(&readValueE,sizeof(unsigned char),1,inputFile) != 1 || readValueE == marker || readValueE == readValueD)
                        {
                          //if end of file or marker don't bother to flip compression
                          fseek(inputFile, -3, SEEK_CUR);
                          fwrite(&runCount,sizeof(unsigned char),1,outputFile);
                          fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                          readValueA = readValueB;
                          runCount = 1;
                        }
                      else
                        {
                          fwrite(&marker,sizeof(unsigned char),1,outputFile);
                          fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                          fwrite(&readValueB,sizeof(unsigned char),1,outputFile);
                          fwrite(&readValueC,sizeof(unsigned char),1,outputFile);
                          fwrite(&readValueD,sizeof(unsigned char),1,outputFile);
                          readValueA = readValueE;
                          while((fread(&readValueB,sizeof(unsigned char),1,inputFile) == 1) && readValueA != readValueB && readValueB != marker)
                          {
                            fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                            readValueA = readValueB;
                            runCount = 1;
                          }
                          fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
                          readValueA = readValueB;
                          fwrite(&marker,sizeof(unsigned char),1,outputFile);
                          runCount = 1;
                        }
                    }
                }

            }
        }
      if(runCount > 1)
      {
        fwrite(&runCount,sizeof(unsigned char),1,outputFile);
        fwrite(&readValueA,sizeof(unsigned char),1,outputFile);
      }

    }
  else //Decompression----------------------------------------------------------
    {
      while(fread(&runCount,sizeof(unsigned char),1,inputFile) == 1)
        {
          if(runCount != marker)
            {
              fread(&data,sizeof(unsigned char),1,inputFile);

              for(i = 0; i < runCount; i++)
                {
                  fwrite(&data,sizeof(unsigned char),1,outputFile);
                }

            }
          else
            {
              while(fread(&data,sizeof(unsigned char),1,inputFile) == 1)
                {
                  if(data != marker)
                    {
                      fwrite(&data,sizeof(unsigned char),1,outputFile);
                    }
                  else
                    break;

                }
            }

        }
    }
  fclose(inputFile);
  fclose(outputFile);
  return 0;
}
