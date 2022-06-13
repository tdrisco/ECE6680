#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265
#define ROWS 256
#define COLS 256

int main(int argc, char *argv[])
{
  //printf("here");
  //Check for proper input of commandline arguments-----------------------------
  if (argc != 5)
    {
    printf("Usage:  Lab3 [filename] [Angle 1] [Angle 2] [Angle 3]\n");
    exit(0);
    }
  float xangle, yangle, zangle;
  xangle = atof(argv[2]);
  yangle = atof(argv[3]);
  zangle = atof(argv[4]);
  //----------------------------------------------------------------------------

  //Intialize all the starting variables----------------------------------------
  FILE		*inputFile, *outputFile;
  char outputFileName[50];
  char headerParse[50];
  int numVertices, numFaces;
  //----------------------------------------------------------------------------

  // Open the inputed image for reading-----------------------------------------
  inputFile=fopen(argv[1],"rb");
  if (inputFile == NULL)
    {
      printf("Unable to open %s for reading\n",argv[1]);
      exit(0);
    }
  //----------------------------------------------------------------------------

  //setup outputfile with proper extension--------------------------------------
  strcpy(outputFileName,argv[1]);
  int fileNameLength = strlen(outputFileName);
  outputFileName[fileNameLength-3] = '\0';
  strcat(outputFileName,"ppm");
  //----------------------------------------------------------------------------

  //Open output file to be written to-------------------------------------------
  outputFile=fopen(outputFileName,"wb");
  if (outputFile == NULL)
    {
      printf("Unable to open %s for writing\n",outputFileName);
      exit(0);
    }
  //----------------------------------------------------------------------------

  //Parse Header for number of vertices and faces-------------------------------
  fgets(headerParse, 50, inputFile);
  fgets(headerParse, 50, inputFile);
  fscanf(inputFile,"%s %s %d ",headerParse,headerParse,&numVertices);
  fgets(headerParse, 50, inputFile);
  fgets(headerParse, 50, inputFile);
  fgets(headerParse, 50, inputFile);
  fscanf(inputFile,"%s %s %d ",headerParse,headerParse,&numFaces);
  fgets(headerParse, 50, inputFile);
  fgets(headerParse, 50, inputFile);

  printf("Vertices %d\tFaces %d\n",numVertices,numFaces);
  //----------------------------------------------------------------------------

  //Read PLY file verices and faces---------------------------------------------
  int *faces, i;
  float *vertices;

  //read in vertices
  vertices = (float *)calloc(numVertices*3,sizeof(float));
  faces = (int *)calloc(numFaces*4,sizeof(int));

  for(i = 0; i < numVertices; i++){
    fscanf(inputFile,"%f %f %f ",&vertices[i*3],&vertices[i*3+1],&vertices[i*3+2]);
  }

  //for(i = 0; i < numVertices; i++){
  //  printf("%f %f %f\n",vertices[i*3],vertices[i*3+1], vertices[i*3+2]);
//  }

  //read in the faces
  for(i = 0; i < numFaces; i++){
    fscanf(inputFile,"%d %d %d %d ",&faces[i*4],&faces[i*4+1],&faces[i*4+2],&faces[i*4+3]);
  }

  for(i = 0; i < numFaces; i++){
    if(i == 684)
      printf("%d %d %d %d\n",faces[i*4],faces[i*4+1],faces[i*4+2],faces[i*4+3]);
  }
  //----------------------------------------------------------------------------

  //Calculate the bounding box on the vertices----------------------------------
  float max[3], min[3], center[3], E;

  min[0] = max[0] = vertices[0];
  min[1] = max[1] = vertices[1];
  min[2] = max[2] = vertices[2];

  for(i = 0; i < numVertices; i++){
    //max and min x
    if(vertices[i*3] < min[0])
      min[0] = vertices[i*3];
    else if (vertices[i*3] > max[0])
      max[0] = vertices[i*3];
    //max and min Y
    if(vertices[i*3+1] < min[1])
      min[1] = vertices[i*3+1];
    else if (vertices[i*3+1] > max[1])
      max[1] = vertices[i*3+1];
    //max and min Z
    if(vertices[i*3+2] < min[2])
      min[2] = vertices[i*3+2];
    else if (vertices[i*3+2] > max[2])
      max[2] = vertices[i*3+2];
  }

  center[0] = (min[0] + max[0])/2;
  center[1] = (min[1] + max[1])/2;
  center[2] = (min[2] + max[2])/2;

  printf("X -> min: %f\tmax: %f\tcenter: %f\n",min[0],max[0],center[0]);
  printf("Y -> min: %f\tmax: %f\tcenter: %f\n",min[1],max[1],center[1]);
  printf("Z -> min: %f\tmax: %f\tcenter: %f\n",min[2],max[2],center[2]);

  //Find E
  E = max[0]-min[0];
  if((max[1]-min[1]) > E)
    E = (max[1]-min[1]);
  if((max[2]-min[2]) > E)
    E = (max[2]-min[2]);

  printf("E = %f\n",E);
  //----------------------------------------------------------------------------

  //Calculate camera position and orientation-----------------------------------
  float camera[3],up[3];
  float cameraDef[] = {1,0,0};
  float upDef[] = {0,0,1};
  double Convert = PI/180.0;
  int r,c,pixelnum;
  unsigned char *pixels;
  float A, B, C, D, n, d, dot1, dot2, dot3, cdiv, rdiv;
  float zbuffer, image[3], v0[3], v1[3], v2[3], intersect[3], cross1[3], cross2[3];
  float left[2], a, right[2], top[2], bottom[2], topleft[2], top2[2], bottom2[2];


  //printf("Angles-> x: %f\ty: %f\tz: %f\n",Convert*xangle,Convert*yangle,Convert*zangle);

  //rotate by xangle
  camera[0] = cameraDef[0];
  camera[1] = cameraDef[1]*cos(Convert*xangle) + cameraDef[2]*sin(Convert*xangle);
  camera[2] = cameraDef[1]*-1*sin(Convert*xangle) + cameraDef[2]*cos(Convert*xangle);
  cameraDef[1] = camera[1];
  cameraDef[2] = camera[2];
  //rotate by yangle
  camera[1] = cameraDef[1];
  camera[0] = cameraDef[0]*cos(Convert*yangle) + cameraDef[2]*-1*sin(Convert*yangle);
  camera[2] = cameraDef[0]*sin(Convert*yangle) + cameraDef[2]*cos(Convert*yangle);
  cameraDef[0] = camera[0];
  cameraDef[2] = camera[2];
  //rotate by zangle
  camera[2] = cameraDef[2];
  camera[0] = cameraDef[0]*cos(Convert*zangle) + cameraDef[1]*sin(Convert*zangle);
  camera[1] = cameraDef[0]*-1*sin(Convert*zangle) + cameraDef[1]*cos(Convert*zangle);
  cameraDef[0] = camera[0];
  cameraDef[1] = camera[1];

  //For Up
  //rotate by xangle
  up[0] = upDef[0];
  up[1] = upDef[1]*cos(Convert*xangle) + upDef[2]*sin(Convert*xangle);
  up[2] = upDef[1]*-1*sin(Convert*xangle) + upDef[2]*cos(Convert*xangle);
  upDef[1] = up[1];
  upDef[2] = up[2];
  //rotate by yangle
  up[1] = upDef[1];
  up[0] = upDef[0]*cos(Convert*yangle) + upDef[2]*-1*sin(Convert*yangle);
  up[2] = upDef[0]*sin(Convert*yangle) + upDef[2]*cos(Convert*yangle);
  upDef[0] = up[0];
  upDef[2] = up[2];
  //rotate by zangle
  up[2] = upDef[2];
  up[0] = upDef[0]*cos(Convert*zangle) + upDef[1]*sin(Convert*zangle);
  up[1] = upDef[0]*-1*sin(Convert*zangle) + upDef[1]*cos(Convert*zangle);
  upDef[0] = up[0];
  upDef[1] = up[1];

  printf("Camera-> X: %f\tY: %f\tZ: %f\n",camera[0],camera[1],camera[2]);
  printf("Up-> X: %f\tY: %f\tZ: %f\n",up[0],up[1],up[2]);

  //move and scale camera vector
  camera[0] = 1.5*E*camera[0] + center[0];
  camera[1] = 1.5*E*camera[1] + center[1];
  camera[2] = 1.5*E*camera[2] + center[2];
  printf("Camera(move and scale)-> X: %f\tY: %f\tZ: %f\n\n",camera[0],camera[1],camera[2]);
//------------------------------------------------------------------------------

//Determine the 3D coordinates bounding the image-------------------------------


  //calc left
  left[0] = up[1]*(center[2]-camera[2]) - up[2]*(center[1]-camera[1]);
  left[1] = up[2]*(center[0]-camera[0]) - up[0]*(center[2]-camera[2]);
  left[2] = up[0]*(center[1]-camera[1]) - up[1]*(center[0]-camera[0]);
  //calc a
  a = left[0]*left[0] + left[1]*left[1] + left[2]*left[2];
  a = sqrt(a);
  printf("a: %f\n",a);
  //calc left
  left[0] = (E/(2*a))*left[0] + center[0];
  left[1] = (E/(2*a))*left[1] + center[1];
  left[2] = (E/(2*a))*left[2] + center[2];
  printf("left: <%f, %f, %f>\n",left[0],left[1],left[2]);
  //calc right
  right[0] = up[2]*(center[1]-camera[1]) - up[1]*(center[2]-camera[2]);
  right[1] = up[0]*(center[2]-camera[2]) - up[2]*(center[0]-camera[0]);
  right[2] = up[1]*(center[0]-camera[0]) - up[0]*(center[1]-camera[1]);
  //calc right
  right[0] = (E/(2*a))*right[0] + center[0];
  right[1] = (E/(2*a))*right[1] + center[1];
  right[2] = (E/(2*a))*right[2] + center[2];
  printf("right: <%f, %f, %f>\n",right[0],right[1],right[2]);
  //calc top
  top[0] = (E/2)*up[0] + center[0];
  top[1] = (E/2)*up[1] + center[1];
  top[2] = (E/2)*up[2] + center[2];
  top2[0] = top[0];
  top2[1] = top[1];
  top2[2] = top[2];
  printf("top: <%f, %f, %f>\n",top[0],top[1],top[2]);
  //calc bottom
  bottom[0] = -1*(E/2)*up[0] + center[0];
  bottom[1] = -1*(E/2)*up[1] + center[1];
  bottom[2] = -1*(E/2)*up[2] + center[2];
  bottom2[0] = bottom[0];
  bottom2[1] = bottom[1];
  bottom2[2] = bottom[2];
  printf("bottom: <%f, %f, %f>\n",bottom[0],bottom[1],bottom[2]);

  //calc topleft
  topleft[0] = (E/2)*up[0] + left[0];
  topleft[1] = (E/2)*up[1] + left[1];
  topleft[2] = (E/2)*up[2] + left[2];
  printf("topleft: <%f, %f, %f>\n",topleft[0],topleft[1],topleft[2]);

  //printf("%f %f %f\n",topleft[2],left[2],right[2]-left[2]);
  //----------------------------------------------------------------------------

  //Define each pixel value in the image----------------------------------------

  //printf("%f %f %f\n",right[2],left[2],right[2]-left[2]);
  //printf("%f %f %f\n",bottom[2],top[2],bottom[2]-top[2]);


  pixels = (unsigned char *)calloc(ROWS*COLS,1);
  pixelnum = -1;

  for(r = 0; r < ROWS; r++){
    //printf("here--------------------------------------%d\n",r);
    for(c = 0; c < COLS; c++){
      pixelnum++;
      zbuffer = 9999999;
      cdiv = (float)c/(float)(COLS -1);
      rdiv = (float)r/(float)(ROWS -1);
      //printf("%f\n",rdiv*(bottom[2]-top[2]));
      image[0] = topleft[0] + (cdiv*(right[0]-left[0])) + (rdiv*(bottom2[0]-top2[0]));
      image[1] = topleft[1] + (cdiv*(right[1]-left[1])) + (rdiv*(bottom2[1]-top2[1]));
      image[2] = topleft[2] + (cdiv*(right[2]-left[2])) + (rdiv*(bottom2[2]-top2[2]));
      //printf("image: <%f, %f, %f>\n",image[0],image[1],image[2]);

      //printf("%f\n",test);
      for(i = 0; i < numFaces; i++){
        //get triangle points
        v0[0] = vertices[faces[i*4+1]*3];
        v0[1] = vertices[faces[i*4+1]*3+1];
        v0[2] = vertices[faces[i*4+1]*3+2];
        v1[0] = vertices[faces[i*4+2]*3];
        v1[1] = vertices[faces[i*4+2]*3+1];
        v1[2] = vertices[faces[i*4+2]*3+2];
        v2[0] = vertices[faces[i*4+3]*3];
        v2[1] = vertices[faces[i*4+3]*3+1];
        v2[2] = vertices[faces[i*4+3]*3+2];
        //printf("v0: <%f, %f, %f>\n",v0[0],v0[1],v0[2]);
        //printf("v1: <%f, %f, %f>\n",v1[0],v1[1],v1[2]);
        //printf("v2: <%f, %f, %f> %d\n",v2[0],v2[1],v2[2],i);
        //find the plane equation
        A = (v1[1] - v0[1])*(v2[2] - v0[2]) - (v1[2] - v0[2])*(v2[1] - v0[1]);
        B = (v1[2] - v0[2])*(v2[0] - v0[0]) - (v1[0] - v0[0])*(v2[2] - v0[2]);
        C = (v1[0] - v0[0])*(v2[1] - v0[1]) - (v1[1] - v0[1])*(v2[0] - v0[0]);
        D = -1*A*v0[0] + -1*B*v0[1] + -1*C*v0[2];

        n = -1*A*camera[0] + -1*B*camera[1] + -1*C*camera[2] - D;
        d = A*(image[0] - camera[0]) + B*(image[1] - camera[1]) + C*(image[2] - camera[2]);

        intersect[0] = camera[0] + (n/d)*(image[0] - camera[0]);
        intersect[1] = camera[1] + (n/d)*(image[1] - camera[1]);
        intersect[2] = camera[2] + (n/d)*(image[2] - camera[2]);

        cross1[0] = (v2[1] - v0[1])*(v1[2] - v0[2]) - (v2[2] - v0[2])*(v1[1] - v0[1]);
        cross1[1] = (v2[2] - v0[2])*(v1[0] - v0[0]) - (v2[0] - v0[0])*(v1[2] - v0[2]);
        cross1[2] = (v2[0] - v0[0])*(v1[1] - v0[1]) - (v2[1] - v0[1])*(v1[0] - v0[0]);
        cross2[0] = (intersect[1] - v0[1])*(v1[2] - v0[2]) - (intersect[2] - v0[2])*(v1[1] - v0[1]);
        cross2[1] = (intersect[2] - v0[2])*(v1[0] - v0[0]) - (intersect[0] - v0[0])*(v1[2] - v0[2]);
        cross2[2] = (intersect[0] - v0[0])*(v1[1] - v0[1]) - (intersect[1] - v0[1])*(v1[0] - v0[0]);
        dot1 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];
        //printf("dot1: %f\n",dot1);

        cross1[0] = (v0[1] - v1[1])*(v2[2] - v1[2]) - (v0[2] - v1[2])*(v2[1] - v1[1]);
        cross1[1] = (v0[2] - v1[2])*(v2[0] - v1[0]) - (v0[0] - v1[0])*(v2[2] - v1[2]);
        cross1[2] = (v0[0] - v1[0])*(v2[1] - v1[1]) - (v0[1] - v1[1])*(v2[0] - v1[0]);
        cross2[0] = (intersect[1] - v1[1])*(v2[2] - v1[2]) - (intersect[2] - v1[2])*(v2[1] - v1[1]);
        cross2[1] = (intersect[2] - v1[2])*(v2[0] - v1[0]) - (intersect[0] - v1[0])*(v2[2] - v1[2]);
        cross2[2] = (intersect[0] - v1[0])*(v2[1] - v1[1]) - (intersect[1] - v1[1])*(v2[0] - v1[0]);
        dot2 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];

        cross1[0] = (v1[1] - v2[1])*(v0[2] - v2[2]) - (v1[2] - v2[2])*(v0[1] - v2[1]);
        cross1[1] = (v1[2] - v2[2])*(v0[0] - v2[0]) - (v1[0] - v2[0])*(v0[2] - v2[2]);
        cross1[2] = (v1[0] - v2[0])*(v0[1] - v2[1]) - (v1[1] - v2[1])*(v0[0] - v2[0]);
        cross2[0] = (intersect[1] - v2[1])*(v0[2] - v2[2]) - (intersect[2] - v2[2])*(v0[1] - v2[1]);
        cross2[1] = (intersect[2] - v2[2])*(v0[0] - v2[0]) - (intersect[0] - v2[0])*(v0[2] - v2[2]);
        cross2[2] = (intersect[0] - v2[0])*(v0[1] - v2[1]) - (intersect[1] - v2[1])*(v0[0] - v2[0]);
        dot3 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];

        //if(r == 0 && c == 0)
          //printf("dot1: %f\tdot2: %f\tdot3: %f\t\n",dot1,dot2,dot3);

        //if(dot1 < 0 || dot2 < 0 || dot3 < 0)
          //continue; //pixels[pixelnum] = 0;
      //  if(n/d > zbuffer)
          //continue; //pixels[pixelnum] = 0;
        //if(n/d <= zbuffer)
          //zbuffer = n/d;
      //  if(abs(d) < 0.00001)
        //  continue; //pixels[pixelnum] = 0;
        //else{
      //  pixels[pixelnum] = 155 + (i%100);

        //zbuffer = n/d;
        //}
      if(dot1 >= 0 && dot2 >= 0 && dot3 >= 0){
        if((n/d) < zbuffer){
          zbuffer = (n/d);
          pixels[r*COLS+c] = 155 + (i%100);
        }
      }

        //if(n/d <= zbuffer)
          //zbuffer = n/d;
      }
    }
  }

  //write ppm image
  //for(i = 0; i < ROWS*COLS; i++)
    //printf("%c",pixels[i]);

  fprintf(outputFile,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(pixels,COLS*ROWS,1,outputFile);
  return 0;
}
