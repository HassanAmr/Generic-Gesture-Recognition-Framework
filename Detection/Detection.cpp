#include <stdio.h>

#include "GRT.h"
using namespace GRT;

int main(int argc, char** argv)
{	

	string fileName = "";


	if (argc > 1)
    {
    	fileName = argv[1];
  	}
  	else
  	{
  		printf("File name not entered");
		return 1;
  	}


	/*---------------------------------------------------------------------*
	 ************************* start GRT **********************
	 *---------------------------------------------------------------------*/

	ANBC anbc;
	anbc.setNullRejectionCoeff( 10 );
	anbc.enableScaling( true );
	anbc.enableNullRejection( true );
	vector< double > inputVector;
	int sensorVectorSize = 10;//read later from config file
	for(int i = 0; i < sensorVectorSize; i++) //read later from config file
		{
			inputVector.push_back(0);
		}	
	UINT predictedClass;

	//Load the ANBC model from a file
	if( !anbc.load(fileName) )
	{
		cout << "Failed to load the classifier model!\n";
		return EXIT_FAILURE;
	}

	/*-----------------------------------------------------------------------*
	 ************************* START INITIALIZATION ***********************
	 *-------------------------------------------------------------------------*/

	//Features data
	double sensorData[sensorVectorSize];
	double firstDer[sensorVectorSize];
	double secondDer[sensorVectorSize];
	//double lastFirstDer[sensorVectorSize];
	for(int i = 0; i < sensorVectorSize; i++)
	{
		firstDer[i] = 0;
		secondDer[i] = 0;
	}


	//int dataDroppedCounter = 0;
	int seq_counter = 0;
	int seq_num = 1;

	//FILE * logFile;
	//FILE * trainingFile;
	
	//logFile = fopen ("log","w");
	//trainingFile = fopen ("Training","w");

	//string feedback_data = "";
	//printf("here1\n");
	//stringstream feedback_data;
	//printf("here2\n");
	//printf("Seq %d:\n", seq_num);
	//fprintf (trainingFile, "Seq %d:\n", seq_num);
	//UINT confCharacters [20];
	//time_t start = time(0);
	//time_t timer = time(0);
	//double time_since_last_iteration = 0.0;
	bool running = true;
	while (running)
	{
	
		//readline here, and fill sensorData array with its contents... needs yet to be implemented
		//scanf(" %c", &c);

		//stringstream ss;
		//ss.str("");
		//ss << c;
		/*Read the six byte value of mag xyz from first sensor*/
		
		
		for(int i = 0; i < sensorVectorSize; i++)
		{
			//read something that indicates the termination of the program
			if(scanf("%lf", &sensorData[i]) == 1)
			{
				firstDer[i] = sensorData[i] - firstDer[i];
				secondDer[i] = firstDer[i] - secondDer[i];
				//printf("%5.2f \t %5.2f \t %5.2f\n", sensorData[i], firstDer[i], secondDer[i]);
			}
			else
			{
				running = false;
				printf("Sensor has finshed sending data!\n");
				break;
			}

			inputVector.at(i) = sensorData[i];
			
			//fill training file
		}
		//Perform a prediction using the classifier
		anbc.predict( inputVector );
		predictedClass = anbc.getPredictedClassLabel();
		switch(predictedClass)
		{
		case 1:
			printf("A\n");
		break;
		case 2: 
			printf("B\n");
		break;
		case 3:
			printf("C\n");
		break;
		case 4: 
			printf("D\n");
		break;
		case 5:
			printf("E\n");
		break;
		case 6: 
			printf("F\n");
		break;
		case 7:
			printf("G\n");
		break;
		case 8: 
			printf("H\n");
		break;
		case 9:
			printf("I\n");
		break;
		case 0: 
			printf("?\n");
		break;
		}
		fflush(stdout);
		//time_since_last_iteration = difftime( time(0), timer);

		//timer = time(0);
		
		seq_counter++;
		if (seq_counter == 1000)
		{
			seq_num++;
			seq_counter = 0;
			//printf("Seq %d:\n", seq_num);
			//fprintf (trainingFile, "Seq %d:\n", seq_num);
		}
	}

	//fclose (trainingFile);

	return 0;
}