#include "Tracker.h"
#include "cf.h"
#include "Config.h"
#include "rect.h"
#include "drawRect.h"
#include "VOT.hpp"
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

static Mat  rgbImg;

//����ԭʼRGBͼ��
Mat GKCgetInputRGBImage(void)
{
	return rgbImg;
}
//��ͼ���ϻ�������ο�
void rectangle(Mat& rMat, const FloatRect& rRect, const Scalar& rColour)
{
	IntRect r(rRect);
	rectangle(rMat, Point(r.XMin(), r.YMin()), Point(r.XMax(), r.YMax()), rColour,4);
}
//��ͼ���ϻ����ܽ����ʮ������
void GKCrectangle(Mat& rMat, const FloatRect& rRect, const Scalar& rColour)
{
	IntRect r(rRect);
	//rectangle(rMat, Point(r.XMin(), r.YMin()), Point(r.XMax(), r.YMax()), rColour);

	vector<Point2f> corners1;

	float cen_x = r.XCentre();
	float cen_y = r.YCentre();
	//��ʮ����
	corners1.push_back(Point2f(cen_x - 18, cen_y));
	corners1.push_back(Point2f(cen_x, cen_y - 18));
	corners1.push_back(Point2f(cen_x + 18, cen_y));
	corners1.push_back(Point2f(cen_x, cen_y + 18));

	line(rMat, corners1[0], corners1[2], rColour, 2);
	line(rMat, corners1[1], corners1[3], rColour, 2);
}

//������
int main(int argc, char* argv[])
{
	// read config file
	string configPath = "KCFconfig.txt";
	Config conf(configPath);
	//ͳ�ƽ�� ƽ��������� ƽ���㷨����ʱ��
	double avgTime = 0;

	for (int videoNow = 0; videoNow < conf.videoNum; videoNow++)
	{
		//���ÿ�ʼ֡�ͽ���֡
		int startFrame = -1;
		int endFrame = -1;
		FloatRect initBB;
		string imgFormat, imgFormatResult;
		float scaleW = 1.f;
		float scaleH = 1.f;

		double dWidth = conf.frameWidth;
		double dHeight = conf.frameHeight;

		imgFormat = conf.sequenceBasePath + "/" + conf.videos[videoNow] + "/%d.jpg"; //"/%04d.jpg";
		imgFormatResult = conf.sequenceBasePath + "/" + conf.videos[videoNow] + "/result/%d.jpg"; //"/%04d.jpg";
		// read first frame to get size
		char imgPath[256];
		startFrame = 1;
		sprintf(imgPath, imgFormat.c_str(), startFrame);

		Mat tmp = imread(imgPath);

		cout << "֡�ߴ磺" << dWidth << "x" << dHeight << endl;
		Size frameSize(tmp.cols, tmp.rows);		
		float xmin, ymin, width, height;
		ifstream gtInit((conf.sequenceBasePath + "/" + conf.videos[videoNow] + "/gt_init.txt").c_str());//gt_init.txt
		//��õ�һ֡��Ŀ�����ڵ�λ��
		if (gtInit.eof() || gtInit.bad() || gtInit.fail())
		{
			cout << "failed to open gt_init.txt" << endl;
			Mat liziframe;
			tmp.copyTo(liziframe);
			cv::Rect temprect = drawmain(liziframe);
			xmin = temprect.x;
			ymin = temprect.y;
			width = temprect.width;
			height = temprect.height;
		}
		else
		{
			float x, y, w, h;
			gtInit >> x >> y >> w >> h;
			cout << x << y << w << h << endl;
			xmin = x;
			ymin = y;
			width = w;
			height = h;
		}
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
			conf.frameWidth = tmp.cols;
			conf.frameHeight = tmp.rows;
			scaleChange = 0;
			initBB = FloatRect((xmin*scaleW>2) ? (xmin*scaleW) : (xmin*scaleW), (ymin*scaleH>2) ? (ymin*scaleH) : (ymin*scaleH), width*scaleW, height*scaleH);
		}

		//���������ʼ��
		Tracker tracker(conf);
		if (!conf.quietMode)
		{
			namedWindow("result");
		}

		Mat result(conf.frameHeight, conf.frameWidth, CV_8UC3);
		Mat frametoshow, frame1, framevideo;
		bool paused = false;
		bool doInitialise = false;
		startFrame = 1;
		endFrame = startFrame + conf.frameNum - 1;
		int frameInd = startFrame;
		double totalTime = 0;
		//���д���ѭ��
		for (frameInd = startFrame; frameInd <= endFrame; ++frameInd)
		{
			Mat frame;
			//cout << "frames: "<<frameInd<<endl;
			//��ȡ����ͼ��
			char imgPath[256];
			sprintf(imgPath, imgFormat.c_str(), frameInd);
			Mat frameOrig = cv::imread(imgPath);//0  CV_LOAD_IMAGE_COLOR

			if (frameOrig.empty())
			{
				cout << "error: could not read frame: " << imgPath << endl;
				break;
			}
			frametoshow = frameOrig.clone();
			frame1 = frameOrig.clone();
			resize(frameOrig, rgbImg, Size(frameOrig.cols * scaleW, frameOrig.rows * scaleH));
			//ת��Ϊ�Ҷ�ͼ
			if (frameOrig.channels() == 3)
			{
				cv::cvtColor(frameOrig, frame1, CV_RGB2GRAY);
				resize(frame1, frame, Size(frameOrig.cols * scaleW, frameOrig.rows * scaleH));
				cv::cvtColor(frame, result, CV_GRAY2RGB);
			}
			else
			{
				frame1 = frameOrig.clone();
				resize(frame1, frame, Size(frameOrig.cols * scaleW, frameOrig.rows * scaleH));
				cv::cvtColor(frame, result, CV_GRAY2RGB);
			}
			//���õ�һ֡��Ŀ��λ�ó�ʼ������������
			if (frameInd == startFrame)
			{
				tracker.Initialise(frame, initBB);
			}
			else
			{
				if (tracker.IsInitialised())
				{
					//�������庯��
					double t0 = (double)cvGetTickCount();
					tracker.Track(frame);
					t0 = (double)cvGetTickCount() - t0;
					totalTime += t0 / (cvGetTickFrequency() * 1000);

					float txmin = tracker.GetBB().XMin();
					float tymin = tracker.GetBB().YMin();
					float twidth = tracker.GetBB().Width();
					float theight = tracker.GetBB().Height();
					FloatRect lastBB(txmin / scaleW, tymin / scaleH, twidth / scaleW, theight / scaleH);
					cv::Rect grect;
					GKCrectangle(result, tracker.GetBB(), CV_RGB(0, 255, 0));
					GKCrectangle(frametoshow, lastBB, CV_RGB(255, 0, 0));
					//rectangle(result, tracker.GetBB(), CV_RGB(0, 255, 0));
					//rectangle(frametoshow, lastBB, CV_RGB(255, 0, 0));
				}
			}
			if (!conf.quietMode)
			{
				//��ʾԤ����ͼ��
				imshow("result", frametoshow);
				waitKey(1);
				sprintf(imgPath, imgFormatResult.c_str(), frameInd);
				imwrite(imgPath, frametoshow);
			}
		}
		cout <<"videoNum: "<< videoNow+1 << "\t average time :" << totalTime / (frameInd - 1) << "ms\t " << endl;
		avgTime += totalTime / (frameInd - 1);
	}
	cout << endl<< "Toatl " << conf.videoNum << " videos tested : " << "\t average time :" << avgTime / conf.videoNum << "ms\t "<< endl;	
	system("pause");
	return EXIT_SUCCESS;
}
