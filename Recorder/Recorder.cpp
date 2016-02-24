#include <iostream> // for standard I/O
//#include <sstream>      // std::stringstream, std::stringbuf
#include <string>   // for strings
#include <time.h>
#include <thread>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdexcept>  

//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
//#include <opencv2/highgui/highgui.hpp>  // Video write


#include "opencv2/opencv.hpp"
//#include <iostream>

using namespace std;
using namespace cv;

void Help();
void SensorStream();
void Timer();
void WriteSRT(int lineCounter, String data);
String FromMillisecondsToSRTFormat(unsigned long val);
int CreateDirectories(String, String);//this function will create the necessary folders for the recorder


//Globals
FILE * srtFile;
FILE * trainingFile;


bool recording = false;
bool terminating = false;

int sensorVectorSize;

struct timeval tv;

bool startedRecording = false;//to recognize a start recording event
bool stoppedRecording = false;//to recognize a stop recording event

unsigned long long startTime;
unsigned long timeNow;

int main(int argc, const char * argv[]){

  Help();

  String recorderName = "";
  String recorderObject = "";


  if (argc > 3)
  {
    recorderName = argv[1];
    recorderObject = argv[2];
    sensorVectorSize = atoi(argv[3]);//TODO: handle error later
  }
  else
  {
    printf("Too few arguments. Please enter the name of the recorder, then a string indicating what is being recorded, and finally the sensor data vector size (3 arguments)");
    return 1;
  }

  thread ss (SensorStream);
  thread tr (Timer);

  ss.detach();
  tr.detach();

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
  int recordingsCounter = 1;
  for(;;){

    if (startedRecording)
    {
      //TODO: make the files naming also suitable for windows
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
      String disp1 = "Timer: " + FromMillisecondsToSRTFormat(timeNow - startTime);//Check if possible to add a comma.
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
    //namedWindow("Control Panel");
    imshow( "Recorder", frame );
    char c = (char)waitKey(33);
    if( c == 27 ) //Escape button pressed
    {
      recording = false;
      terminating = true;
      break;
    }
    else if (c == 'r')
    {
      if (recording)
      {
        stoppedRecording = true;
        printf("Stopped recording!\n");
      }
      else
      {
       startedRecording = true;
       printf("Started recording!\n");
      }
      gettimeofday(&tv, NULL);
      startTime = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
      recording = !recording;
    }
    //a failed attempt to restart streaming in case the stream feed was interrupted, but eitherway, the piping program has to be restarted.
    //else if (c == 's')
    //{
    //  if (!streaming)
    //  {
    //    thread ss (SensorStream);
    //    ss.detach();
    //    streaming = true;
    //  }
    //} 
  } 
  delete video;
  
  return 0;

}

void SensorStream()
{
 //double sensorData[sensorVectorSize];

  char buffer[256];
  double val;
  
  bool streaming = true;
  
  bool startWriting = false;//this is to align the stream with the recording

  //Data specific to write the srt file
  int srtLineCounter = 1;
  String srtData = "";
  String srtTime = "";
  while (streaming)
  {
    for(int i = 0; i < sensorVectorSize; i++)
    {
      scanf("%s", buffer);  
      try
      {
        val = stof(buffer);
        if(recording)
        {
          if (startWriting)
          {
            if (i==0)//this is only concerned with the srt file to document the time at which the first value arrived
              srtTime += FromMillisecondsToSRTFormat(timeNow - startTime);

            fprintf (trainingFile, "%5.2f", val);
            srtData += buffer;//TODO: consider taking only 2 decimal places
            //sprintf(strData, "%5.2f", val);
            if (i == sensorVectorSize - 1)
            {
              fprintf (trainingFile, "\n");
              srtData += "\n";
              srtTime += " --> "+ FromMillisecondsToSRTFormat(timeNow - startTime) + "\n";
              WriteSRT(srtLineCounter, srtTime + srtData);//send here the print value to the WriteSRT function
              srtData = "";
              srtTime = "";
              srtLineCounter++;
            } 
            else
            {
              fprintf (trainingFile, "\t");
              srtData += "\t";
            }
          }
          else if (i == sensorVectorSize - 1)//this ensures that if the recording started in the middle of a vector, that the stream will be aligned with the recording
          {
            startWriting = true;
          }
        }
        else
        {
          srtLineCounter = 1;
        }
      }
      catch (const std::invalid_argument& ia)
      {
        printf("Sensor has finshed sending data!\n Please restart the Recorder.\n");
        //printf("Sensor has finshed sending data!\n If you are planning on restarting the sensor feed,"
          //      " please make sure to press the 's' key in advance, otherwise the feed will not be synchronized.\n");
        streaming = false;
      }
    }     
  } 
}

void Timer()
{

  struct timeval now;
  while(!terminating)//TODO, check if there is a better practice
  {
    gettimeofday(&now, NULL);
    timeNow = (unsigned long long)(now.tv_sec) * 1000 + (unsigned long long)(now.tv_usec) / 1000;
  }
  
}

void WriteSRT(int lineCounter, String data)
{
  fprintf (srtFile, "%d\n", lineCounter);
  fprintf (srtFile, "%s\n", data.c_str());
}

String FromMillisecondsToSRTFormat(unsigned long val)
{
  int milliseconds = val/1000;
  milliseconds = val - milliseconds*1000;
  int seconds = (int) (val / 1000) % 60 ;
  int minutes = (int) ((val / (1000*60)) % 60);
  int hours   = (int) ((val / (1000*60*60)) % 24);
  char str[20];
  sprintf(str, "%02d:%02d:%02d,%03d", hours, minutes, seconds, milliseconds);
  //ss<< setfill('0') << setw(2) <<hours<<":"<< setfill('0') << setw(2) <<minutes<<":"<< setfill('0') << setw(2) <<seconds<<","<< milliseconds<<endl;
  return str;

}

void Help()
{
    cout << "\nThis program helps you train your sensor data by piping the sensor stream directly into this program.\n\n"
            "Please make sure that the Recorder is active before starting the sensor stream feed, otherwise the feed will not be synchronized.\n\n"
            "Call:\n"
            "./Recorder [recorder_name] [symbol_to_train] [expected_#_inputs_from_sensor]\n" << endl;

    cout << "Hot keys: *Recorder window must be active for these controls to work.*\n"
            "\tESC - quit the program\n"
            "\tr - start, or stop a recording.\n"
            //"\ts - start stream again, in case the sensor was interrupted.\n"
            << endl;
}

int CreateDirectories(String recorderName, String sessionName)//TODO: make it also suitable for windows
{
  int returnValue = 0;
  int status = mkdir("Output", 0777);
  if ( status == 0)
  {
    printf("Folder created 'Output'\n");
  }
  else if (errno == EEXIST)
  {
    //printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
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
    //printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
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
    //printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create folder for the new session\n");
    returnValue = 1;
  }


  status = mkdir(String(directoryName + "/Video").c_str(), 0777);
  if ( status == 0)
  {
    //printf("");//Not important to mention it again, since we already told the user that a new folder for the recorder has been created.
  }
  else if (errno == EEXIST)
  {
    //printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create the 'Video' folder for the recorder\n");
    returnValue =  1;
  }

  status = mkdir(String(directoryName + "/Training").c_str(), 0777);
  if (status == 0)
  {
    //printf("");//Not important to mention it again, since we already told the user that a new folder for the recorder has been created.
  }
  else if (errno == EEXIST)
  {
    //printf("");//Here we don't print anything, because it is going to be distrubing whenever we get notfied about the Output folder being already there.
  }
  else
  {
    printf("Couldn't create the 'Training' folder for the recorder\n");
    returnValue =  1;
  }
  printf("\n");
  return returnValue;

}