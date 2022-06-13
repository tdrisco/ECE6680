#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
Display			*Monitor;
Window			ImageWindow;
GC				ImageGC;
XWindowAttributes		Atts;
XImage			*Picture;
int				ROWS,COLS,BYTES,i;
unsigned char		*displaydata;
unsigned char		*filedata;
char		header[80];
FILE		*fpt;
int imageType; //1 is rgb and 0 is greyscale (code used to navigate if statements)

//------------------Read in file data and allocate memory-----------------------

//Check if the user properly inputed command line arguments
if (argc != 2)
  {
  printf("Usage:  ImageDisplayX [filename]\n");
  exit(0);
  }

// Open the inputed image for reading
fpt=fopen(argv[1],"r");
if (fpt == NULL)
  {
  printf("Unable to open %s for reading\n",argv[1]);
  exit(0);
  }

// Read image header
i=fscanf(fpt,"%s %d %d %d ",header,&COLS,&ROWS,&BYTES);

//Determine the image type based on the magic code
if (strcmp(header,"P5") == 0)
  {
  printf("The inputed file is a Greyscale image.\n");
  imageType = 0;
  }
else if (strcmp(header,"P6") == 0)
  {
    printf("The inputed file is a RGB image.\n");
    imageType = 1;
  }
else
  {
    printf("File is not a Greyscale or RGB image\n");
    fclose(fpt);
    exit(0);
  }

// allocate dynamic memory for orginal image (filedata) using header information
if(imageType)
  {
    // Need to multiple size by three for RGB
    filedata = (unsigned char *)calloc(ROWS*COLS*3,sizeof(unsigned char));
    printf("Allocating memory for a RGB image.\n");
    if (filedata == NULL)
      {
        printf("Unable to allocate %d x %d x 3 memory\n",COLS,ROWS);
        exit(0);
      }
    //Read binary data from the file and then close the file
    fread(filedata,1,ROWS*COLS*3,fpt);
    fclose(fpt);
  }
else
  {
    //Greyscale does not need x3
    filedata = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
    printf("Allocating memory for a Greyscale image.\n");
    if (filedata == NULL)
      {
      printf("Unable to allocate %d x %d memory\n",COLS,ROWS);
      exit(0);
      }
    //Read binary data from the file and then close the file
    fread(filedata,1,ROWS*COLS,fpt);
    fclose(fpt);
  }


// allocate dynamic memory for new image using header information
displaydata = (unsigned char *)calloc(ROWS*COLS*4,sizeof(unsigned char)); //Needs to be x4 for 32 bit display
if (displaydata == NULL)
  {
    printf("Unable to allocate %d x %d x 4 memory\n",COLS,ROWS);
    exit(0);
  }
//------------------------------------------------------------------------------

//------------------Convert from file data to dispay data-----------------------

int j = 0; //displaydata counter

if(imageType)
  {
    //Covert 24 bit RGB to 32 bit RGB(Personal Display)
    for(i=0;i < (ROWS*COLS*3); i+=3)
      {
        displaydata[j] = filedata[i+2];
        displaydata[j+1] = filedata[i+1];
        displaydata[j+2] = filedata[i];
        displaydata[j+3] = 0x00; //skips every fourth byte (don't care)
        j += 4;
      }
  }
else
  {
    //Covert 8 bit grey to 32 bit RGB(Personal Display)
    for(i=0;i < (ROWS*COLS); i ++)
      {
        displaydata[j] = filedata[i];
        displaydata[j+1] = filedata[i];
        displaydata[j+2] = filedata[i];
        displaydata[j+3] = 0x00; //skips every fourth byte (don't care)
        j += 4;
      }
  }
//------------------------------------------------------------------------------

Monitor=XOpenDisplay(NULL);
if (Monitor == NULL)
  {
  printf("Unable to open graphics display\n");
  exit(0);
  }

ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
	50,10,		/* x,y on screen */
	COLS,ROWS,	/* width, height */
	2, 		/* border width */
	BlackPixel(Monitor,0),
	WhitePixel(Monitor,0));

ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

XMapWindow(Monitor,ImageWindow);
XFlush(Monitor);
while(1)
  {
  XGetWindowAttributes(Monitor,ImageWindow,&Atts);
  if (Atts.map_state == IsViewable /* 2 */)
    break;
  }

Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
		DefaultDepth(Monitor,0),
		ZPixmap,	/* format */
		0,		/* offset */
		displaydata,/* the data */
		COLS,ROWS,	/* size of the image data */
		16,		/* pixel quantum (8, 16 or 32) */
		0);		/* bytes per line (0 causes compute) */

XPutImage(Monitor,ImageWindow,ImageGC,Picture,
		0,0,0,0,	/* src x,y and dest x,y offsets */
		COLS,ROWS);	/* size of the image data */

XFlush(Monitor);

//Wait until user decides to quit in order to close the image window
char key[10];
key[0] = 'q';
printf("Enter the character \"Q\" to close the image: ");
while(strcmp(key,"Q")!= 0)
{
  sleep(1);
  scanf("%s",key);
}
XCloseDisplay(Monitor);
}
