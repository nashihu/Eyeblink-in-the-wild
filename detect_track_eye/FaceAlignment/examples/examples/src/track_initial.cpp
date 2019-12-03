#include "Tracker.h"
#include "cf.h"
#include "Config.h"
#include "rect.h"
#include "math.h"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

struct hu_Rect
{
	int x;
	int y;
	int width;
	int height;
};
//����ԭʼRGBͼ��
/*#define BUILD_DLL
#ifdef BUILD_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
extern "C" EXPORT void *track_initial(cv::Rect eyefield, Mat init_frame, float &scale);*/
//������
void *track_initial(cv::Rect eyefield, cv::Mat init_frame,float &scale)
{
	// read config file
	string configPath = "KCFconfig.txt";
	Config conf(configPath);
	FloatRect initBB;
	float scaleW = 0.5;
	float scaleH = 0.5;
	double dWidth = conf.frameWidth;
	double dHeight = conf.frameHeight;
	//cout << "֡�ߴ磺" << dWidth << "x" << dHeight << endl;
	Size frameSize(init_frame.cols, init_frame.rows);		
	float xmin, ymin, width, height;
	xmin = eyefield.x;
	ymin = eyefield.y;
	width = eyefield.width;
	height = eyefield.height;
	bool scaleChange = 0;
	//�������ͼ��������ͼ�񽵵ͷֱ���
	if (width > 30 || height > 20)
	{
		scaleW = 0.5;// (float)conf.frameWidth / tmp.cols;
		scaleH = 0.5;// (float)conf.frameHeight / tmp.rows;
		scaleChange = 1;
		initBB = FloatRect((xmin*scaleW>2) ? (xmin*scaleW) : (xmin*scaleW), (ymin*scaleH>2) ? (ymin*scaleH) : (ymin*scaleH), width*scaleW, height*scaleH);
	}
	else
	{
		scaleW = 1.0;
		scaleH = 1.0;
		conf.frameWidth = init_frame.cols;
		conf.frameHeight = init_frame.rows;
		scaleChange = 0;
		initBB = FloatRect((xmin*scaleW>2) ? (xmin*scaleW) : (xmin*scaleW), (ymin*scaleH>2) ? (ymin*scaleH) : (ymin*scaleH), width*scaleW, height*scaleH);
	}
	scale = scaleW;
	//���������ʼ��
	//Tracker tracker_tmp(conf);
	//Tracker *trackerTemp = new(Tracker);
	//trackerTemp->Reset(configPath);
	
	cv::Mat frame1, framevideo;
	//���д���ѭ��
	cv::Mat frame;
	//resize(init_frame, rgbImg, Size(init_frame.cols * scaleW, init_frame.rows * scaleH));
	//ת��Ϊ�Ҷ�ͼ
	if (init_frame.channels() == 3)
	{
		cv::cvtColor(init_frame, frame1, CV_RGB2GRAY);
		resize(frame1, frame, Size(init_frame.cols * scaleW, init_frame.rows * scaleH));
				//cv::cvtColor(frame, result, CV_GRAY2RGB);
	}
	else
	{
		frame1 = init_frame.clone();
		resize(frame1, frame, Size(init_frame.cols * scaleW, init_frame.rows * scaleH));
			//cv::cvtColor(frame, result, CV_GRAY2RGB);
	}
	Tracker * tracker = new Tracker(conf);
	tracker->Initialise(frame, initBB);
	//trackerTemp->Initialise(frame, initBB);
	//*tracker = tracker_tmp;
	//tracker = trackerTemp;
	//��ʼ������׷����
	//xmin = eyefield_zuo.x;
	//ymin = eyefield_zuo.y;
	//width = eyefield_zuo.width;
	//height = eyefield_zuo.height;
	//initBB = FloatRect((xmin*scaleW>2) ? (xmin*scaleW) : (xmin*scaleW), (ymin*scaleH>2) ? (ymin*scaleH) : (ymin*scaleH), width*scaleW, height*scaleH);
	//tracker_tmp.Initialise(frame, initBB);
	//tracker_zuo = &tracker_tmp;
	return (void *)tracker;
}
