#include <iostream> // for standard I/O
//#include <chrono>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include <unistd.h>
#include <sys/time.h>

using namespace std;

int main(int argc, const char * argv[]){
	
	int sensorVectorSize;
  int maxValue;
  int counter = 0;

  long int timeNow;
  struct timeval now;

	if (argc > 2)
  	{
    	 sensorVectorSize = atoi(argv[1]);
       maxValue = atoi(argv[2]);

  	}
  	else
  	{
    	printf("Too few arguments.");
    	return 1;
  	}

  //usleep(2000000);
	
  while (1)
	{
    gettimeofday(&now, NULL);
    timeNow = (now.tv_sec * 1000000) + now.tv_usec;
		for (int i = 1; i <= sensorVectorSize; i++)
    {
      if (maxValue < 0)
      {
        int randomNum = rand() % 200  - 99;
        cout<<randomNum<<'\t';
      }
      else if (maxValue == 0)
      {
        cout<<counter<<'\t';
      }
      else
      {
        cout<<float(maxValue/i)<<'\t';
      }
    }
    cout<<endl;
    counter++;
    gettimeofday(&now, NULL);
    timeNow = ((now.tv_sec * 1000000) + now.tv_usec) - timeNow;
    if(timeNow < 20000)
		  usleep(20000 - timeNow);//50 hz
	}
	
  	
  
  return 0;

}
