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
  outputFile=fopen("testFile2.txt","wb");
  if (outputFile == NULL)
    {
      printf("Unable to openfor writing\n");
      exit(0);
    }

//  A RAT SAT ON A CAR
for(int i = 0; i < 2000; i++){
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
    data =10;
    fwrite(&data,sizeof(unsigned char),1,outputFile);
}



  fclose(outputFile);


return 0;
}
