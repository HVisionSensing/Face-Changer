#define _USE_MATH_DEFINES

#include <iostream>
#include <cstdio>
#include <time.h>
#include <math.h>

#include "opencvheader.h"
#include "UnveilinWaves.h"
#include "ViewPointSimulation.h"
#ifndef min
#define min(x, y) ((x<y)? (x):(y))
#endif

// Image size (pixels)
#define	IMG_WIDTH			144 / 2
#define	IMG_HEIGHT			216 / 2
#define HALF_INTERVAL_DIST	IMG_WIDTH / 8

// Put 4 X 7 images in a window screen
#define IMG_ROW_NUM			4
#define IMG_COL_NUM			11


CvPoint MousePT;


void on_mouse( int event, int x, int y, int flags, void* param )
{
  switch (event) {
    case CV_EVENT_MOUSEMOVE:
      MousePT = cvPoint(x,y);
      break;

    case CV_EVENT_LBUTTONDOWN:
      break;

    case CV_EVENT_RBUTTONDOWN:
      break;

    default:
      break;
  }
}

IplImage *LoadAndResizeImage(const char* filename, int Height, int Width)
{
  IplImage *Img, *tmpImg;
  double ratio;
  CvRect roiRect;


  // Load Image
  tmpImg = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
  if (tmpImg == NULL) {
    return NULL;
  }

  // Calculate ROI
  ratio = ((double) Width / (double) Height);
  if (((double) tmpImg->width / (double) tmpImg->height) > ratio) {
    roiRect.height = tmpImg->height;
    roiRect.width = (int) (ratio * tmpImg->height);
    roiRect.y = 0;
    roiRect.x = (tmpImg->width - roiRect.width) / 2;
  }
  else {
    roiRect.height = (int) (tmpImg->width / ratio);
    roiRect.width = tmpImg->width;
    roiRect.y = (tmpImg->height - roiRect.height) / 2;
    roiRect.x = 0;
  }
  // Set ROI
  cvSetImageROI(tmpImg, roiRect);
  // Create image
  Img = cvCreateImage( cvSize(Width, Height), IPL_DEPTH_8U, 3);
  // Resize image
  cvResize(tmpImg, Img, CV_INTER_CUBIC);
  // Release temporal image
  cvReleaseImage(&tmpImg);


  return Img;
}


// 
void UpdateEnabledTime(int64 enable_time[],  const int &idx, const int64 &now_time, const double &freq)
{
  double delay_sec;
  int key;

  // Set unveiling effect enable times, clockwise
  // Center
  enable_time[idx] = now_time;
  delay_sec = 0.2;
  // Left-Up
  key = idx - IMG_COL_NUM - 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM > 0)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Upside
  key = idx - IMG_COL_NUM;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Right-Up
  key = idx - IMG_COL_NUM + 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM < IMG_COL_NUM - 1)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Right
  key = idx + 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM < IMG_COL_NUM - 1)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Right-down
  key = idx + IMG_COL_NUM + 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM < IMG_COL_NUM - 1)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Downside
  key = idx + IMG_COL_NUM;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Left-down
  key = idx + IMG_COL_NUM - 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM > 0)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
    delay_sec += 0.2;
  }
  // Left
  key = idx - 1;
  if (key >= 0 && key < IMG_COL_NUM * IMG_ROW_NUM && enable_time[key] == 0 && (idx % IMG_COL_NUM > 0)) {
    enable_time[key] = (int64) (now_time + freq * delay_sec);
  }

}



int main(int argc, char** argv)
{	
  const char *imagename = argc > 1 ? argv[1] : "./ArtImgSrc/000.jpg";  
  IplImage *clone[IMG_COL_NUM * IMG_ROW_NUM];
  IplImage *faceImg = NULL;									// Original foreground image
  IplImage *unveilingImg = NULL;
  IplImage *artImgs[IMG_COL_NUM * IMG_ROW_NUM];				// Original background images
  IplImage *singleUnveilingImg[IMG_COL_NUM * IMG_ROW_NUM];	// Unveiling images
  UnveilinWaves unveilinEffect[IMG_COL_NUM * IMG_ROW_NUM];	// Unveiling effect objects
  CvRect artROIRect[IMG_COL_NUM * IMG_ROW_NUM];				// The position for each image
  int64 enable_time[IMG_COL_NUM * IMG_ROW_NUM];				// Unveiling effect enable time 
  int64 start_time[IMG_COL_NUM * IMG_ROW_NUM];				// Unveiling effect start time 
  int last_effect_num[IMG_COL_NUM * IMG_ROW_NUM];				// Unveiling effect play number
  int changeOrder[IMG_COL_NUM * IMG_ROW_NUM];
  CvPoint realPT, simPT;
  ViewPointSimulation simulationObj;
  int64 now_time;
  double freq = cv::getTickFrequency();						// Tick frequency
  int key, idx;


  srand((unsigned int) time(NULL));

  // Load foreground image
  faceImg = LoadAndResizeImage(imagename, IMG_HEIGHT, IMG_WIDTH);
  if (faceImg == NULL) {
    return -1;
  }


  // Load background images
  unveilingImg = cvCreateImage(cvSize((IMG_WIDTH + 2 * HALF_INTERVAL_DIST) * IMG_COL_NUM, (IMG_HEIGHT + 2 * HALF_INTERVAL_DIST) * IMG_ROW_NUM), IPL_DEPTH_8U, 3);
  if (unveilingImg == NULL) {
    cvReleaseImage(&faceImg);
    return -1;
  }
  cvZero(unveilingImg);
  for (int i = 0; i < IMG_COL_NUM * IMG_ROW_NUM; i++) {
    char pathbuf[128];

    sprintf(pathbuf, "./ArtImgSrc/imgSrc%02d.jpg", i);
    artImgs[i] = LoadAndResizeImage(pathbuf, IMG_HEIGHT, IMG_WIDTH);
    if (artImgs[i] == NULL) {
      cvReleaseImage(&faceImg);
      cvReleaseImage(&unveilingImg);
      for (int k = 0; k < i; k++) {
        cvReleaseImage(&artImgs[k]);
        cvReleaseImage(&singleUnveilingImg[k]);
      }
      return -1;
    }
    singleUnveilingImg[i] = cvCloneImage(artImgs[i]);
	

    artROIRect[i].x = HALF_INTERVAL_DIST + (IMG_WIDTH + HALF_INTERVAL_DIST * 2) * (i % IMG_COL_NUM);
    artROIRect[i].y = HALF_INTERVAL_DIST + (IMG_HEIGHT + HALF_INTERVAL_DIST * 2) * (i / IMG_COL_NUM);
    artROIRect[i].width = IMG_WIDTH;
    artROIRect[i].height = IMG_HEIGHT;
    cvSetImageROI(unveilingImg, artROIRect[i]);
    cvCopy(artImgs[i], unveilingImg);
    cvResetImageROI(unveilingImg);

    unveilinEffect[i].SetImage(artImgs[i], faceImg);

    changeOrder[i] = 0;
    enable_time[i] = 0;
    start_time[i] = 0;
    last_effect_num[i] = 0;
  }


  // Create window
  cvNamedWindow("Unveiling Window", CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback("Unveiling Window", on_mouse, 0);

  // Show image
  cvShowImage("Unveiling Window", unveilingImg);
  cvResizeWindow("Unveiling Window", unveilingImg->width, unveilingImg->height);
  cvWaitKey(1);


  // Initilize
  MousePT = cvPoint(-1 , -1);
  simulationObj.SetParameter(unveilingImg->width / 2, unveilingImg->height / 2, min(IMG_WIDTH, IMG_HEIGHT) / 8.0, IMG_HEIGHT, IMG_WIDTH, HALF_INTERVAL_DIST, IMG_ROW_NUM, IMG_COL_NUM);


  // Main loop
  while (1) {
    now_time = cv::getTickCount();

    // Get moust point
    realPT = MousePT;
    realPT.x -= unveilingImg->width / 2;
    realPT.y -= unveilingImg->height / 2;
    // Get simulation point and focus image number
    idx = simulationObj.Simulation(realPT, simPT);

    if (idx >= 0) {

      // Update the activated time of each image
      UpdateEnabledTime(enable_time, idx, now_time, freq);

      // Start to play
      for (int i = 0; i < IMG_COL_NUM * IMG_ROW_NUM; i++) {
        if (unveilinEffect[i]._weHaveRotates == 1) {										// Playing
          continue;
        }
        if (changeOrder[i] == 1) {														// Change from foreground image to background image
		  clone[i] = cvCloneImage(artImgs[i]);
          unveilinEffect[i].SetImage(faceImg, artImgs[i]);
          unveilinEffect[i].PutDrop(IMG_WIDTH / 4, IMG_HEIGHT / 4, 200);
          changeOrder[i] = 0;
          start_time[i] = now_time;
          enable_time[i] = 0;
          last_effect_num[i] = 0;
        }
        else if (enable_time[i] != 0 && now_time >= enable_time[i]) {					// Change from background image to foreground image
          clone[i] = cvCloneImage(faceImg);
		  unveilinEffect[i].SetImage(artImgs[i], faceImg);
          unveilinEffect[i].PutDrop(IMG_WIDTH / 4, IMG_HEIGHT / 4, 200);
          changeOrder[i] = 1;
          start_time[i] = now_time;
          last_effect_num[i] = 0;
        }
      }
    }

	//IplImage *clone = cvCloneImage(singleUnveilingImg[i]);
	//cvReleaseImage(&clone);
	//cvCopy(src, dst);

    // Play animation
    cvZero(unveilingImg);
    for (int i = 0; i < IMG_COL_NUM * IMG_ROW_NUM; i++) {
      if (start_time[i]) {
        if (now_time - start_time[i] > freq * last_effect_num[i] / 30) {	// play for 30 fps
          if (unveilinEffect[i]._weHaveRotates == 1){
            unveilinEffect[i].ProcessRotates(singleUnveilingImg[i]);
            last_effect_num[i]++;
          }
          else {	// End
		    cvCopy(clone[i], singleUnveilingImg[i]);
			cvReleaseImage(&clone[i]);
            enable_time[i] = 0;
            start_time[i] = 0;
            last_effect_num[i] = 0;
          }
        }
      }

      // Set image
      cvSetImageROI(unveilingImg, artROIRect[i]);
      cvCopy(singleUnveilingImg[i], unveilingImg);
      cvResetImageROI(unveilingImg);
    }


    // Show simulation point
    cvCircle(unveilingImg, cvPoint(simPT.x + unveilingImg->width / 2, simPT.y + unveilingImg->height / 2), 2, CV_RGB(255, 255, 255), 9);


    // Show image
    cvShowImage("Unveiling Window", unveilingImg);


    // Wait and get key
    key = cvWaitKey(10);
    if (key == 27) {	// 'ESC' key
      break;
    }
  }

  cvDestroyAllWindows();
  cvReleaseImage(&faceImg);
  cvReleaseImage(&unveilingImg);
  for (int i = 0; i < IMG_COL_NUM * IMG_ROW_NUM; i++) {
    cvReleaseImage(&artImgs[i]);
    cvReleaseImage(&singleUnveilingImg[i]);
  }

  return 0;
}
