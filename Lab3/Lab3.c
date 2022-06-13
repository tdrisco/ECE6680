#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 65536

int main(int argc, char *argv[])
{
  //printf("here");
  //Check for proper input of commandline arguments-----------------------------
  if (argc != 3)
    {
    printf("Usage:  Lab3 [compress or decompress] [filename]\n");
    exit(0);
    }
  //----------------------------------------------------------------------------

  //Intialize all the starting variables----------------------------------------
  FILE		*inputFile, *outputFile;
  int compression;
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
    strcat(outputFileName,"lzw");
  else
    strcat(outputFileName,"ulzw");
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
      //Code to complete the lzw compression------------------------------------
      unsigned char C, *P, *PC, **dictionary;
      dictionary = (unsigned char **)calloc(MAX_LENGTH,sizeof(unsigned char *));
      int *length;
      unsigned short int pLength = 0;
      unsigned short int dictionaryMatch = 0;
      unsigned short int dictionaryLen = 0;
      unsigned short int code = 0;
      length = (int *)calloc(MAX_LENGTH,sizeof(int)); //corresponding array of pattern lengths

      //setup the roots in the dictionary
      for(i = 0; i < 256; i++)
        {
          //all single byte characters
          dictionary[i] = (unsigned char *)calloc(1,sizeof(unsigned char));
          dictionary[i][0] = i;
          length[i] = 1;
        }
      dictionaryLen = i-1;

      //set P to empty byte
      P = (unsigned char *)calloc(1,sizeof(unsigned char));

      while(fread(&C,sizeof(unsigned char),1,inputFile) == 1)
        {
          //need to combine P+C
          PC = (unsigned char *)calloc(pLength+1,sizeof(unsigned char));
          if(pLength != 0)
            memcpy(PC,P,pLength);

          memcpy((PC + pLength),&C,1);

          //Now need to check for PC in the dictionary
          for(i = 0; i < MAX_LENGTH; i++)
            {
              if((pLength+1) == length[i])
                {
                  if(0 == memcmp(PC,dictionary[i],(pLength+1)))
                    {
                      dictionaryMatch = 1;
                      code = i;
                    }
                }
              if(dictionaryMatch)
                break;
            }

          //check dictionaryMatch and proceed
          if(dictionaryMatch)
            {
              //Since P+C was in the dictionary P = P + C
              free(P);
              pLength = pLength + 1;
              P = (unsigned char *)calloc(pLength,sizeof(unsigned char));
              memcpy(P,PC,pLength);
            }
          else
            {
              //output code for p add PC to dictionary and let p=c
              //find p in the dictionary
              dictionaryMatch = 0;
              for(i = 0; i < MAX_LENGTH; i++)
                {
                  if((pLength) == length[i])
                    {
                      if(0 == memcmp(P,dictionary[i],(pLength)))
                        {
                          dictionaryMatch = 1;
                          code = i;
                        }
                    }
                  if(dictionaryMatch)
                    break;
                }
              //write the code for P to the output
              fwrite(&code,sizeof(unsigned short int),1,outputFile);

              //Add P+C to the dictionary and keep track of its length
              dictionaryLen++;
              dictionary[dictionaryLen] = (unsigned char *)calloc((pLength + 1),sizeof(unsigned char));
              memcpy(dictionary[dictionaryLen],PC,(pLength+1));
              length[dictionaryLen] = pLength + 1;
              free(P);
              //Assign C to P
              P = (unsigned char *)calloc(1,sizeof(unsigned char));
              *P = C;
              pLength = 1;
            }
          //Reset dictionary match check and clear P+C each run
          dictionaryMatch = 0;
          free(PC);
        }
      //clear the buffer
      for(i = 0; i < MAX_LENGTH; i++)
        {
          if((pLength) == length[i])
            {
              if(0 == memcmp(P,dictionary[i],(pLength)))
                {
                  dictionaryMatch = 1;
                  code = i;
                }
            }
          if(dictionaryMatch)
            break;
        }
      fwrite(&code,sizeof(unsigned short int),1,outputFile);
      //printf("%hd",code);

      for(i = 0; i < dictionaryLen; i++)
        free(dictionary[i]);
      free(dictionary);
      free(length);
    }
  else
    {
      //Code to complete the lzw decompression----------------------------------
      unsigned char *X, Y, Z, *XY_XZ, **dictionary;
      dictionary = (unsigned char **)calloc(MAX_LENGTH,sizeof(unsigned char *));
      int *length;
      unsigned short int dictionaryLen = 0;
      unsigned short int P, C;
      length = (int *)calloc(MAX_LENGTH,sizeof(int)); //corresponding array of pattern lengths

      //setup the roots in the dictionary
      for(i = 0; i < 256; i++)
        {
          //all single byte characters
          dictionary[i] = (unsigned char *)calloc(1,sizeof(unsigned char));
          dictionary[i][0] = i;
          length[i] = 1;
        }
      dictionaryLen = i-1;


      //read first value for c and output it
      fread(&C,sizeof(unsigned short int),1,inputFile);
      fwrite(dictionary[C],sizeof(unsigned char),1,outputFile);


      P = C;
      while(fread(&C,sizeof(unsigned short int),1,inputFile) == 1)
        {
            //X will be same regardless if C is in the dictionary (pattern for p)
            X = (unsigned char *)calloc(length[P],sizeof(unsigned char));
            memcpy(X,dictionary[P],length[P]);

            //First part of XY/XZ will always be X
            XY_XZ = (unsigned char *)calloc(length[P]+ 1,sizeof(unsigned char));
            memcpy(XY_XZ,X,length[P]);

          if(C <= dictionaryLen)
            {
              //C is in the dictionary -> Output pattern
              fwrite(dictionary[C],sizeof(unsigned char),length[C],outputFile);

              //Y is first char in C pattern -> make X + Y
              Y = dictionary[C][0];
              memcpy((XY_XZ + length[P]),&Y,1);

              //add XY to the dictionary and record its length
              dictionaryLen++;
              dictionary[dictionaryLen] = (unsigned char *)calloc((length[P] + 1),sizeof(unsigned char));
              memcpy(dictionary[dictionaryLen],XY_XZ,(length[P]+1));
              length[dictionaryLen] = length[P] + 1;
            }
          else
            {
              //Z is first char in P pattern -> make X + Z
              Z = dictionary[P][0];
              memcpy((XY_XZ + length[P]),&Z,1);

              //add XZ to the dictionary, output XZ, and record its length
              fwrite(XY_XZ,sizeof(unsigned char),length[P]+1,outputFile);
              dictionaryLen++;
              dictionary[dictionaryLen] = (unsigned char *)calloc((length[P] + 1),sizeof(unsigned char));
              memcpy(dictionary[dictionaryLen],XY_XZ,(length[P]+1));
              length[dictionaryLen] = length[P] + 1;
            }
          // Free pointers to Z and XY or XZ (both the same)
          free(X);
          free(XY_XZ);

          //Update Previous to equal current
          P = C;
        }
      //Free both the dictionary and the pointer storing pattern lengths
      for(i = 0; i <= dictionaryLen; i++)
        free(dictionary[i]);

      free(dictionary);
      free(length);
    }

  return 0;
}
