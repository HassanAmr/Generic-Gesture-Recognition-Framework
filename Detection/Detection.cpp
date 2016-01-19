#include <SensorFunctions.h>

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <thread>
#include <cmath>
#include<string.h>    //strlen
#include<string>  //string
#include <sstream>
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent

#include <time.h>
#include "math.h"
//#include "GRT/GRT.h"

using namespace std;
//using namespace GRT;

/* bno055 I2C Address */
#define BNO055_I2C_ADDR1                0x28
#define BNO055_I2C_ADDR2                0x29


void LoopBreaker();
void sig_handler(int signo);



//Globals
//static struct bno055_t *bno055;

int running = 0;
bool breakLoop = false;

thread br (LoopBreaker);


int main(int argc, char** argv)
{
	signal(SIGINT, sig_handler);
	
	bool offset = true;//read later from config file

	int sensorVectorSize = 6;//read later from config file


	
	if (offset)
	{
		//read data first once, then have it as an offset (-ve) for the next readings
		double offset[sensorVectorSize];
		scanf(" %c", &c);

		stringstream ss;
		ss.str("");
		ss << c;
		
		ReadDataFromSensors(offset);
	}
	

	
	

	//Features data
	double sensorData[sensorVectorSize];
	double firstDer[sensorVectorSize];
	double secondDer[sensorVectorSize];
	double lastFirstDer[sensorVectorSize];
	for(int i = 0; i < sensorVectorSize; i++)
	{
		lastFirstDer[i] = 0;
	}


	//int dataDroppedCounter = 0;
	int seq_counter = 0;
	int seq_num = 1;

	//FILE * logFile;
	FILE * trainingFile;
	
	//logFile = fopen ("log","w");
	trainingFile = fopen ("Training","w");

	//string feedback_data = "";
	//printf("here1\n");
	//stringstream feedback_data;
	//printf("here2\n");
	//printf("Seq %d:\n", seq_num);
	fprintf (trainingFile, "Seq %d:\n", seq_num);
	//UINT confCharacters [20];
	//time_t start = time(0);
	//time_t timer = time(0);
	//double time_since_last_iteration = 0.0;
	while (1)
	{
		if(running == -1)
		{
			printf("Something went wrong on the board\n");
			break;
		}
		
		
		
		//readline here, and fill sensorData array with its contents... needs yet to be implemented
		//scanf(" %c", &c);

		//stringstream ss;
		//ss.str("");
		//ss << c;
		/*Read the six byte value of mag xyz from first sensor*/
		
		
		
		
		for(int i = 0; i < sensorVectorSize; i++)
		{
			firstDer[i] = (sensorData[i] - offset[i])/1;
			secondDer[i] = (firstDer[i] - lastFirstDer[i])/1;
			lastFirstDer[i] = firstDer[i];
			printf("%f \t %f \t %f", sensorData[i], firstDer[i], secondDer[i]);
			//fill training file
		}
		printf("\n");
		fflush(stdout);
		//time_since_last_iteration = difftime( time(0), timer);

		//timer = time(0);
		
		seq_counter++;
		if (seq_counter == 1000)
		{
			seq_num++;
			seq_counter = 0;
			//printf("Seq %d:\n", seq_num);
			fprintf (trainingFile, "Seq %d:\n", seq_num);
		}
		if (breakLoop)
		{
			//double seconds_since_start = difftime( time(0), start);
			//printf("Gestures recognised: %d in %lf seconds\n", classifiedCounter, seconds_since_start);
			printf("Exit command received\n");
			break;
		}
		BNO055_delay_msek(30);
	}

	fclose (trainingFile);

	return MRAA_SUCCESS;
	//return 0;
}

void sig_handler(int signo)
{
	if (signo == SIGINT) {
		printf("closing nicely\n");
		running = -1;
	}
}

void LoopBreaker()
{
	char c;
	while (1)
	{
		scanf("%c", &c);
		if (c == 's')
		{
			breakLoop = true;
			br.detach();
			break;
		}
	}
}