#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

  FILE		*outputFile;
  unsigned char data;
  //Open output file to be written to-------------------------------------------
  outputFile=fopen("testFile.txt","wb");
  if (outputFile == NULL)
    {
      printf("Unable to openfor writing\n");
      exit(0);
    }

  //data = 97;
  //for(int i = 0; i < 40; i++)
    //fwrite(&data,sizeof(unsigned char),1,outputFile);

// A RAT SAT ON A CAR
  data = 65;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 32;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data =82;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 65;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 84;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 32;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 83;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data =65;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 84;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 32;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 79;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 78;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data =32;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 65;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 32;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 67;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data = 65;
  fwrite(&data,sizeof(unsigned char),1,outputFile);
  data =82;
  fwrite(&data,sizeof(unsigned char),1,outputFile);

  //data = 97;
  //for(int i = 0; i < 40; i++)
    //fwrite(&data,sizeof(unsigned char),1,outputFile);
/*
    data = 98;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 99;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 100;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 101;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 102;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 0;
    printf("%c",data);
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 99;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 100;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 101;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
    data = 102;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
*/



  fclose(outputFile);


return 0;
}
