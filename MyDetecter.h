#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <stack>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;
//standard 
#define SCARLAR_LOWER_RED_1 0,50,50//160, 200, 160
#define SCARLAR_UPPER_RED_1 10,255,255//179, 255, 255
#define SCARLAR_LOWER_RED_2 170,50,50//160, 200, 160
#define SCARLAR_UPPER_RED_2 180,255,255//179, 255, 255

#define SCARLAR_LOWER_BLUE 75, 128, 128//170, 0, 0//75, 128, 128
#define SCARLAR_UPPER_BLUE 120, 255, 255//270,255,255//120, 255, 255

#define SCARLAR_LOWER_YELLOW 20,100,100//22, 128, 128
#define SCARLAR_UPPER_YELLOW 30,255,255//38, 255, 255

#define SCARLAR_LOWER_BLACK 0, 0, 0, 0
#define SCARLAR_UPPER_BLACK 180, 255, 30, 0

#define SCARLAR_LOWER_WHITE 0, 0, 200, 0
#define SCARLAR_UPPER_WHITE 180, 20, 255, 0

#define RATIO 1.8
#define SCALAR_SOLID 0, 255, 0
#define THICKNESS 2

#define MINIMUM_SIZE 20
class TrafficSign
{
	int id;
	Point center;
	int length;
	
public:	
	TrafficSign(int id = -1, Point c = Point(0, 0), int len = 0.0)
	{
		this->id = id;
		c = center;
		length = len;
	}
	~TrafficSign(){};
	Point getCenTer() const { return center; }
	int getLength() const { return length; }
	void setCenTer(Point c) { center = c; } 
	void setLength(int len) { length = len; }
	int getId() const { return id; }
	void setId(int i) { id = i; }
};




class MyDetecter
{
	static vector<string> Responses;
	static double Angle(Point pt1, Point pt2, Point pt0);
	static void FindSquares(const Mat& img, const Mat& imgRed, const Mat& imgBlue, const Mat& imgYellow, vector<TrafficSign>& trafficSigns);
	static void FindTriangles(const Mat& img, const Mat& imgRed, const Mat& imgBlue, const Mat& imgYellow, vector<TrafficSign>& trafficSigns);
	static void FindCircles(const Mat& bw, vector<TrafficSign>& trafficSigns);
	static void FindEllipses(const Mat& img,const Mat& imgRed, const Mat& imgBlue, const Mat& imgYellow, vector<TrafficSign>& trafficSigns);
	
public:
	~MyDetecter();
	static void LoadFileSignNames(const string& filename);
	static void DetectTrafficSigns(const Mat& imgSrc, vector<TrafficSign>& trafficSigns);
	static void DrawTrafficSigns( Mat& imgSrc,  vector<TrafficSign>& trafficSigns);
	static Mat CutTrafficSign(const Mat& imgSrc, TrafficSign&);
	static void SetLabel(Mat&, const TrafficSign&);
};

