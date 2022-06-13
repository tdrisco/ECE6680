#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

  FILE		*outputFile;
  unsigned char data1,data2,data3;
  //Open output file to be written to-------------------------------------------
  outputFile=fopen("testFile.txt","wb");
  if (outputFile == NULL)
    {
      printf("Unable to openfor writing\n");
      exit(0);
    }

  data1 = 65;
  data2 = 66;
  data3 = 67;

//  for(int i = 0; i < 2; i++)
    //fwrite(&data,sizeof(unsigned char),1,outputFile);

  for(int i = 0; i < 1000; i++)
    {
      fwrite(&data1,sizeof(unsigned char),1,outputFile);
      fwrite(&data2,sizeof(unsigned char),1,outputFile);
    }

fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data3,sizeof(unsigned char),1,outputFile);

fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data3,sizeof(unsigned char),1,outputFile);
fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data2,sizeof(unsigned char),1,outputFile);
fwrite(&data1,sizeof(unsigned char),1,outputFile);
fwrite(&data3,sizeof(unsigned char),1,outputFile);




  fclose(outputFile);


return 0;
}
