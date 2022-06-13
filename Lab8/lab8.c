// Tim Driscoll
// Lab # 8 Real time scheduling using RMA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Task structure
typedef struct task {
    float period;
    float runTime;
    float maxBlocking;
} task;


int main(int argc, char *argv[]) {

  //Create all the tasks--------------------------------------------------------
  float overHead = 0.153;
  task Tasks[8];

  Tasks[0].period = 10.56;
  Tasks[0].runTime = 1.30;
  Tasks[0].maxBlocking = 3.30;

  Tasks[1].period = 40.96;
  Tasks[1].runTime = 4.70;
  Tasks[1].maxBlocking = 3.30;

  Tasks[2].period = 61.44;
  Tasks[2].runTime = 9.00;
  Tasks[2].maxBlocking = 9.30;

  Tasks[3].period = 100.00;
  Tasks[3].runTime = 23.00;
  Tasks[3].maxBlocking = 9.20;

  Tasks[4].period = 165.00;
  Tasks[4].runTime = 38.30;
  Tasks[4].maxBlocking = 5.20;

  Tasks[5].period = 285.00;
  Tasks[5].runTime = 10.00;
  Tasks[5].maxBlocking = 5.20;

  Tasks[6].period = 350.00;
  Tasks[6].runTime = 3.00;
  Tasks[6].maxBlocking = 2.00;

  Tasks[7].period = 700.00;
  Tasks[7].runTime = 2.00;
  Tasks[7].maxBlocking = 0.00;
  //----------------------------------------------------------------------------

  int i, k, l, j, totalCount;
  int floorVal;
  int ceilVal;
  float left, right;
  totalCount = 0;

  for(i = 1; i <= 8; i++){
    //printf("(%d)----------------------------------\n",i);
    for(k=1; k <= i; k++){
      floorVal = (int) (floor(Tasks[i-1].period/Tasks[k-1].period));
      for(l = 1; l <= floorVal; l++){
        left  = 0.0;
        right = l*Tasks[k-1].period;
        for(j = 1; j <= (i-1); j++){
          ceilVal = (int) (ceil((l*Tasks[k-1].period)/Tasks[j-1].period));
          left += ((Tasks[j-1].runTime + overHead)* ceilVal);
        }
        left += Tasks[i-1].runTime + Tasks[i-1].maxBlocking;
        if(left <= right){
          totalCount++;
          printf("(%d) Task is schedulable for i = %d\tk = %d\tl = %d\n",
            totalCount,i,k,l);
          //printf("(k,l)\t(%d,%d)\n",k,l);
        }
      }
    }
  }

return 0;
}
