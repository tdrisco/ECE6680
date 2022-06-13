#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
  unsigned char *image;
  FILE *fpt;
  int rows,cols,i;

  rows = 10;
  cols = 5;
  image = (unsigned char *)calloc(rows*cols,1);
  for(i=0; i<rows*cols; i++)
    {
      image[i] = 128;
      //image[i*3] = 255;
      //image[i*3+1] = 0;
      //image[i*3+2] = 0;
    }

  fpt = fopen("greyRecMin.ppm", "wb");
  fprintf(fpt,"P5 %d %d 255\n",cols,rows);
  fwrite(image,rows*cols,1,fpt);
  fclose(fpt);
}
