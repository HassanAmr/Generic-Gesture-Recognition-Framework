#include <iostream> // for standard I/O
#include <chrono>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include <unistd.h>


using namespace std;

int main(int argc, const char * argv[]){
	
	int sensorVectorSize;
  int maxValue;

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


	while (1)
	{
		for (int i = 1; i <= sensorVectorSize; i++)
    {
      if (maxValue <= 0)
      {
        int randomNum = rand() % 200  - 99;
        cout<<randomNum<<endl;
      }
      else
      {
        cout<<float(maxValue/i)<<endl;
      }
    }
		usleep(20000);
	}
	
  	
  
  return 0;

}
