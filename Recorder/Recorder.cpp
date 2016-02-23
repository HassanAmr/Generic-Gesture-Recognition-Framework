#include <iostream> // for standard I/O
#include <string>   // for strings
#include <time.h>
#include <thread>

//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
//#include <opencv2/highgui/highgui.hpp>  // Video write


#include "opencv2/opencv.hpp"
//#include <iostream>

using namespace std;
using namespace cv;


void ToggleRecorder();

bool recording = false;
time_t timer;

thread br (ToggleRecorder);


int main(int argc, const char * argv[]){


  String recorderName = "";
  String recorderObject = "";


  if (argc > 2)
  {
    recorderName = argv[1];
    recorderObject = argv[2];
  }
  else
  {
    printf("Too few arguments. Please enter the name of the recorder, and a string indicating what is being recorded (2 arguments)");
    return 1;
  }

  VideoCapture vcap(0); 
  if(!vcap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }

  //int blinks_number = 1;

  int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
  
  //TODO later to make file names out of the input arguments
  int recordingsCounter = 1;
  String vidFileName = recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".avi";
  String srtFileName = recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter) + ".srt";
  String plainFileName = recorderObject + "_" + recorderName + "_" + std::to_string(recordingsCounter);

  VideoWriter video(vidFileName,CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);

  
  for(;;){
    Mat frame;
    vcap >> frame;



    if (recording)
    {
      //The timer
      String disp1 = "Time: " + std::to_string (difftime( time(0), timer));
      //The frame number (commented for now, as it has no useful purpose)
      //String disp2 = "Frame # " + std::to_string(blinks_number);
      
      putText(frame, disp1, Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
      //putText(frame, disp2, Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

      video.write(frame); //Record the video till here. It is not needed to have a recording indicator in the recoring output

      //Recording Indicator
      circle(frame, Point(26, 16), 8, Scalar(0, 0, 255), -1, 8, 0);
      putText(frame, "RECORDING", Point(40, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
    }

    //++blinks_number;
    
    imshow( "Frame", frame );
    char c = (char)waitKey(33);
    if( c == 27 ) break;
  }
  br.detach();
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
      timer = time(0);
      recording = !recording;
      //br.detach();
      //break;
    }
  }
}
