#include<string>  //string
#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
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
    //Create a new ANBC instance
    ANBC anbc;
    anbc.setNullRejectionCoeff( 10 );
    anbc.enableScaling( true );
    anbc.enableNullRejection( true );

    //Load some training data to train the classifier
    ClassificationData trainingData;

    if( !trainingData.loadDatasetFromFile(fileName) ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }

    //Use 20% of the training dataset to create a test dataset
    //ClassificationData testData = trainingData.partition( 80 );

    //Train the classifier
    if( !anbc.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }


    //Save the ANBC model to a file
    if( !anbc.saveModelToFile(fileName.append("_Model.txt")) ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}