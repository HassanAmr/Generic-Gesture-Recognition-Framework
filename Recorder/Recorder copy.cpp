#include <iostream> // for standard I/O
#include <string>   // for strings
#include <time.h>
#include <thread>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
//#include <opencv2/highgui/highgui.hpp>  // Video write


#include "opencv2/opencv.hpp"
//#include <iostream>

using namespace std;
using namespace cv;


void ToggleRecorder();
void SensorStream();
int CreateDirectories(String, String);//this function will create the necessary folders for the recorder


//Globals
FILE * srtFile;
FILE * trainingFile;

bool startedRecording = false;//to recognize a start recording event
bool stoppedRecording = false;//to recognize a stop recording event

bool recording = false;

int sensorVectorSize;
//time_t timer;

struct timeval tv;
unsigned long long startTime;
unsigned long long timeNow;

thread br (ToggleRecorder);
thread ss (SensorStream);


int main(int argc, const char * argv[]){


  String recorderName = "";
  String recorderObject = "";


  if (argc > 3)
  {
    recorderName = argv[1];
    recorderObject = argv[2];
    sensorVectorSize = atoi(argv[3]);
  }
  else
  {
    printf("Too few arguments. Please enter the name of the recorder, then a string indicating what is being recorded, and finally the sensor data vector size (3 arguments)");
    return 1;
  }
  //Preparing the string for the directories
  //-------------------------------------- 
  time_t session;
  char buffer [80];
  struct tm * timeinfo;
  time (&session);
  timeinfo = localtime (&session);
  //%d%m%H%M%S
  strftime (buffer,80,"%d%m%H%M%S",timeinfo);

  
  String sessionName = String(buffer);
  
  int status = CreateDirectories(recorderName, sessionName);

  if (status == 1)
  {
    printf("The was an error creating the necessary folders for the recorder, the program will now exit.\n");
    return 1;
  }
  //--------------------------------------
  
  //This part is repeated
  //String vidFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".avi";
  //String srtFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".srt";
  //String trainingFileName = "Output/" + recorderName + "/" + sessionName + "/Training/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter);
  //--------------------------------------
  //srtFile = fopen (srtFileName.c_str(),"w");
  //trainingFile = fopen (trainingFileName.c_str()  ,"w");
  //fprintf (srtFile, "\n");//this is only done to create the folder for the recorder if this person is recording for the first time.


  VideoCapture vcap(0); 
  if(!vcap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }

  //int blinks_number = 1;

  int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);


  //VideoWriter video(vidFileName,CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
  //VideoWriter video("Folder/file.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);

  VideoWriter * video;
  struct timeval now;
  int recordingsCounter = 1;
  for(;;){

    if (startedRecording)
    {
      String vidFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".avi";
      String srtFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".srt";
      String trainingFileName = "Output/" + recorderName + "/" + sessionName + "/Training/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter);
      srtFile = fopen (srtFileName.c_str(),"w");
      trainingFile = fopen (trainingFileName.c_str()  ,"w");
      video = new VideoWriter(vidFileName,CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
      startedRecording = false;
    }

    if (stoppedRecording)
    {
      fclose (srtFile);
      fclose (trainingFile);
      recordingsCounter++;
      stoppedRecording = false;
    }

    Mat frame;
    vcap >> frame;

    //TODO: put in a thread later to write the input received by sensors
    //fprintf (pFile, "Seq %d:\n", seq_num);

    if (recording)
    {
      //The timer
      
      gettimeofday(&now, NULL);
      timeNow = (unsigned long long)(now.tv_sec) * 1000 + (unsigned long long)(now.tv_usec) / 1000;

      String disp1 = "Time: " + std::to_string (timeNow - startTime);//Check if possible to add a comma.
      //String disp1 = "Time: " + std::to_string (difftime( time(0), timer));
      //The frame number (commented for now, as it has no useful purpose)
      //String disp2 = "Frame # " + std::to_string(blinks_number);
      
      putText(frame, disp1, Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
      //putText(frame, disp2, Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

      video->write(frame); //Record the video till here. It is not needed to have a recording indicator in the recoring output

      //Recording Indicator
      circle(frame, Point(26, 16), 8, Scalar(0, 0, 255), -1, 8, 0);
      putText(frame, "RECORDING", Point(40, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
    }

    //++blinks_number;
    
    imshow( "Frame", frame );
    char c = (char)waitKey(33);
    if( c == 27 ) break;
  } 
  delete video;
  br.detach();
  ss.detach();
  return 0;

}

void ToggleRecorder()
{
  char c;
  while (1)
  {
    scanf("%c", &c);
    if (c == 's')
    {
      if (recording)
        stoppedRecording = true;
      else
        startedRecording = true;
      //timer = time(0);
      gettimeofday(&tv, NULL);
      startTime = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
      recording = !recording;
      //br.detach();
      //break;
    }
  }
}

void SensorStream()
{
 //double sensorData[sensorVectorSize];

  double input;
  bool streaming = true;
  int i = 0;
  while (streaming)
  {
  //read something that indicates the termination of the program
    if(scanf("%lf", &input) == 1)
    {
      if(recording)
      {
        if (i < sensorVectorSize)
        {
          fprintf (trainingFile, "%5.2f\t", input);
          i++;
        }
        else
        {
          fprintf (trainingFile, "%5.2f\n", input);
          i = 0;
        }
      }
    }
    else
    {
      streaming = false;
      printf("Sensor has finshed sending data!\n");
    }
  } 
}


int CreateDirectories(String recorderName, String sessionName)
{
  int returnValue = 0;
  int status = mkdir("Output", 0777);
  if ( status == 0)
  {
    printf("Folder created 'Output'\n");
  }
  else if (errno == EEXIST)
  {
    printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create folder the 'Output' folder\n");
    returnValue = 1;
  }

  String directoryName = "Output/" + recorderName;

  status = mkdir(directoryName.c_str(), 0777);
  if ( status == 0)
  {
    printf("A new folder created for the recorder\n");
  }
  else if (errno == EEXIST)
  {
    printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create folder for the recorder\n");
    returnValue = 1;
  }

  
  directoryName = directoryName + "/" + sessionName;

  status = mkdir(directoryName.c_str(), 0777);
  if ( status == 0)
  {
    printf("A new folder created for this session\n");
  }
  else if (errno == EEXIST)
  {
    printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create folder for the new session\n");
    returnValue = 1;
  }


  status = mkdir(String(directoryName + "/Video").c_str(), 0777);
  if ( status == 0)
  {
    printf("");//Not important to mention it again, since we already told the user that a new folder for the recorder has been created.
  }
  else if (errno == EEXIST)
  {
    printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create the 'Video' folder for the recorder\n");
    returnValue =  1;
  }

  status = mkdir(String(directoryName + "/Training").c_str(), 0777);
  if (status == 0)
  {
    printf("");//Not important to mention it again, since we already told the user that a new folder for the recorder has been created.
  }
  else if (errno == EEXIST)
  {
    printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create the 'Training' folder for the recorder\n");
    returnValue =  1;
  }
  return returnValue;

}