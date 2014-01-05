#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#define LEN  64*1024*16

void test_1()
{
    struct timeval start;
  struct timeval end;
  unsigned long  timer;
  int i = 0;
  int arry[LEN] ;
 
  memset(arry,0,sizeof(arry));
  gettimeofday(&start,NULL);
  for ( i = 0 ; i < LEN ; i++, arry[i]*=3 );
  gettimeofday(&end,NULL);
  
  printf("%ld\r\n", 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);

  gettimeofday(&start,NULL);
  for ( i = 0 ; i < LEN ; i+=16, arry[i]*=3 );
  gettimeofday(&end,NULL);
  printf("%ld\r\n", 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);
}

void test_2()
{
 struct timeval start;
  struct timeval end;
  unsigned long  timer;
  int i = 0;
  int arry[LEN] ;
 
  memset(arry,0,sizeof(arry));
  
  int j = 0;
  for ( j = 1 ; j < 32 ; j++ ){
    //    sleep(1);
  gettimeofday(&start,NULL);
  for ( i = 0 ; i < LEN ; i+=j)
    arry[i]*=3 ;
  gettimeofday(&end,NULL);
  printf("j=%d, %lu\r\n",j, 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);
  }
}

void test_3()
{
  struct timeval start;
  struct timeval end;
  unsigned long  timer;
  int i = 0;
  
  int step = 64 * 1024 * 1024;

  int j = 0;
  for ( j = 1 ; j < step; j++){
if ( (j & (j-1)) == 0){
    int lenmod = j-1;
    int *arry = (int *)malloc(sizeof(int) * j);

gettimeofday(&start,NULL);
  for( i = 0 ; i < step; i++){
    arry[(i * 16) & lenmod]++;
  }
    gettimeofday(&end,NULL);
    
      printf("j=%8d, %lu\r\n",j, 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);
    }
  }

}

int main()
{
  struct timeval start;
  struct timeval end;
   unsigned long  timer;
  int step = 256 * 1024 * 1024;

  int array[2]={0};

  int i = 0;
gettimeofday(&start,NULL);
 for ( i = 0 ; i < step ; i++ ){
   array[0]++;
   array[0]++;
 }
gettimeofday(&end,NULL);
 printf(" %lu\r\n", 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);
    

  gettimeofday(&start,NULL);
 for ( i = 0 ; i < step ; i++ ){
   array[0]++;
   array[1]++;
 }
gettimeofday(&end,NULL);
 printf(" %lu\r\n", 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec);
    

  
}
