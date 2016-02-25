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
void PlotUpdater();
void SensorStream();
void Timer();
void WriteSRT(int lineCounter, String data);
String FromMillisecondsToSRTFormat(unsigned long val);
int CreateDirectories(String, String);//this function will create the necessary folders for the recorder


//Trackbars
int selectedItem = 0;
int selectedR = 0;
int selectedG = 0;
int selectedB = 0;
void Color_Modifier(int, void* );
//void Hist_and_Backproj(int, void* );//For Plot resolution


//Globals
FILE * srtFile;
FILE * trainingFile;


bool recording = false;//to help the SensorStream thread know when to write the 
bool terminating = false;//to terminate the timer thread when program exits adequately
bool streaming = true;//to terminate the SensorStream thread when program exits adequately

int sensorVectorSize;//the number of expected inputs per 1 feed from the sensor
double * plotValues;//This array will hold the values for the plots
Scalar *plotColors;//This array will hold the colors of the lines for the different inputs
int plotValuesArraySize;
double lastReceivedVal;//because I'm stupid, and didn't compile my opencv with c++11, I have to create this variable, and hope that a race condition will not occur

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

  VideoCapture vcap(0); 
  if(!vcap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }
  
  //allocate memory and start threads here after passing all cases in which program might exit
  
  
  int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
  plotColors = new Scalar [sensorVectorSize];
  RNG rng( 0xFFFFFFFF );
  for (int i = 0; i < sensorVectorSize; i++)
  {
    plotColors[i] = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
  }

  int plot_w = 250; int plot_h = frame_height;
  int r_Plot = 0;
  int g_Plot = 0;
  int b_Plot = 0;
  //int bin_w = cvRound( (double) hist_w/histSize );
  plotValuesArraySize = sensorVectorSize * plot_w;
  //int bin_w = cvRound( (double) hist_w/plotValuesArraySize );
  
  plotValues = new double [plotValuesArraySize];
  for (int i = 0; i < plotValuesArraySize; i++)
  {
    plotValues[i] = 0.0;
  }
  
  thread ss (SensorStream);
  thread tr (Timer);

  ss.detach();
  tr.detach();

  //VideoWriter video(vidFileName,CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
  //VideoWriter video("Folder/file.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);

  VideoWriter * video;
  int recordingsCounter = 1;


  //const char* control_window = "Plot Control Panel";

  namedWindow("Plot Control Panel", WINDOW_NORMAL);

  //const char* trackBarObject = "Object";
  //const char* trackBarRed = "Red";
  //const char* trackBarGreen = "Green";
  //const char* trackBarBlue = "Blue";
  createTrackbar("Object", "Plot Control Panel", &selectedItem, sensorVectorSize, NULL );  
  createTrackbar("Red", "Plot Control Panel", &selectedR, 255, Color_Modifier ); 
  createTrackbar("Green", "Plot Control Panel", &selectedG, 255, Color_Modifier );
  createTrackbar("Blue", "Plot Control Panel", &selectedB, 255, Color_Modifier );
  
  resizeWindow("Plot Control Panel", 500,150);
  for(;;){

    if (startedRecording)
    {
      //TODO: make the files naming also suitable for windows
      String vidFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".avi";
      String srtFileName =      "Output/" + recorderName + "/" + sessionName + "/Video/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".srt";
      String trainingFileName = "Output/" + recorderName + "/" + sessionName + "/Training/" + recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter);
      srtFile = fopen (srtFileName.c_str(),"w");
      trainingFile = fopen (trainingFileName.c_str()  ,"w");
      video = new VideoWriter(vidFileName,CV_FOURCC('M','J','P','G'),10, Size(frame_width + plot_w,frame_height),true);
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

    if (getTrackbarPos("Object", "Plot Control Panel") == 0)//check to see if user wants to change color of the background of the plot or not
    {
      r_Plot = getTrackbarPos("Red", "Plot Control Panel");
      g_Plot = getTrackbarPos("Green", "Plot Control Panel");
      b_Plot = getTrackbarPos("Blue", "Plot Control Panel");

    }

    Mat plotImage( plot_h, plot_w, CV_8UC3, Scalar( b_Plot,g_Plot,r_Plot) );

    //Plotting
    for( int i = 0; i < plot_w - 1; i++ )
    {
      for (int j = 0; j < sensorVectorSize; j++)
      {
        line( plotImage, Point( i, plot_h/2 - cvRound(plotValues[i*sensorVectorSize+j]) ) ,
                         Point( i + 1, plot_h/2 - cvRound(plotValues[i*sensorVectorSize+j + sensorVectorSize]) ),
                         plotColors[j], 1, 8, 0  );
      }
      
    }
    //namedWindow("Plot", CV_WINDOW_AUTOSIZE );
    //imshow("Plot", plotImage );

    Mat display = Mat::zeros ( MAX ( frame.rows, plotImage.rows ), frame.cols + plotImage.cols, frame.type() );

    plotImage.copyTo ( Mat ( display, Rect ( frame.cols, 0, plotImage.cols, plotImage.rows ) ) );
    if (recording)
    {
      String disp1 = "Timer: " + FromMillisecondsToSRTFormat(timeNow - startTime);//Check if possible to add a comma.
      //String disp1 = "Time: " + std::to_string (difftime( time(0), timer));
      //The frame number (commented for now, as it has no useful purpose)
      //String disp2 = "Frame # " + std::to_string(blinks_number);
      
      putText(frame, disp1, Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
      //putText(frame, disp2, Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
      
      frame.copyTo ( Mat ( display, Rect ( 0, 0, frame.cols, frame.rows ) ) );
      video->write(display); //Record the video till here. It is not needed to have a recording indicator in the recoring output

      //Recording Indicator
      circle(display, Point(26, 16), 8, Scalar(0, 0, 255), -1, 8, 0);
      putText(display, "RECORDING", Point(40, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
    }
    else
    {
      frame.copyTo ( Mat ( display, Rect ( 0, 0, frame.cols, frame.rows ) ) );
    }
    
    //++blinks_number;
    
    imshow( "Recorder", display );
    char c = (char)waitKey(33);
    if( c == 27 ) //Escape button pressed
    {
      if (recording)
      {
        fclose (srtFile);
        fclose (trainingFile);
        recording = false;
      }
     
      terminating = true;
      streaming = false;
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
    //a failed attempt to restart streaming in case the stream feed was interrupted, because eitherway, the piping program has to be restarted.
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
  delete plotValues;
  delete plotColors;
  delete video;

  
  return 0;

}

void Color_Modifier(int, void* )
{
  int index = getTrackbarPos("Object", "Plot Control Panel");
  if (index > 0)
  {
    plotColors[index - 1][0] = getTrackbarPos("Blue", "Plot Control Panel");
    plotColors[index - 1][1] = getTrackbarPos("Green", "Plot Control Panel");
    plotColors[index - 1][2] = getTrackbarPos("Red", "Plot Control Panel");
  }

}

void PlotUpdater()
{
  plotValues[plotValuesArraySize - 1] = lastReceivedVal;
  for (int i = 0; i < plotValuesArraySize - 1; i++)
  {
    plotValues[i] = plotValues[i + 1];
  }
  
}


void SensorStream()
{
 //double sensorData[sensorVectorSize];

  char buffer[256];
  double val;

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
        lastReceivedVal = val;
        thread pu (PlotUpdater);
        pu.detach();
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