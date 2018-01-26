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
//standard HSV
#define SCARLAR_LOWER_RED_1 0,50,50//160, 200, 160
#define SCARLAR_UPPER_RED_1 10,255,255//179, 255, 255
#define SCARLAR_LOWER_RED_2 160,50,50//160, 200, 160
#define SCARLAR_UPPER_RED_2 180,255,255//179, 255, 255
//#define SCARLAR_LOWER_RED_3 135,50,50//160, 200, 160
//#define SCARLAR_UPPER_RED_3 145,255,255//179, 255, 255

#define SCARLAR_LOWER_BLUE 75, 128, 128//98,109,20//170, 0, 0
#define SCARLAR_UPPER_BLUE 120, 255, 255//112,255,255//270,255,255

#define SCARLAR_LOWER_YELLOW 20,100,100//22, 128, 128
#define SCARLAR_UPPER_YELLOW 30,255,255//38, 255, 255
//
//#define SCARLAR_LOWER_BLACK 0, 0, 200
//#define SCARLAR_UPPER_BLACK 180, 255, 255
//
//#define SCARLAR_LOWER_WHITE 180, 200, 170
//#define SCARLAR_UPPER_WHITE 255, 255, 255

#define SCARLAR_LOWER_BLACK 0, 0, 0
#define SCARLAR_UPPER_BLACK 0, 50, 10

#define SCARLAR_LOWER_WHITE 110, 20, 200//110, 30, 225
#define SCARLAR_UPPER_WHITE 160, 80, 255//160, 88, 250

#define RATIO 1.75
#define SCALAR_SOLID 0, 255, 0
#define THICKNESS 1

#define MINIMUM_SIZE 20
class TrafficSign
{
	int id;
	Point center;
	Size size;
	
public:	
	TrafficSign(int id = -1, Point c = Point(0, 0), Size s = Size(0,0))
	{
		this->id = id;
		c = center;
		size = s;
	}
	~TrafficSign(){};
	Point getCenTer() const { return center; }
	Size getSize() const { return size; }
	void setCenTer(Point c) { center = c; } 
	void setSize(Size s) { size = s; }
	int getId() const { return id; }
	void setId(int i) { id = i; }
};




class MyDetecter
{
	static vector<string> Responses;
	static double Angle(Point pt1, Point pt2, Point pt0);
	static void FindSquares(const Mat& img, const  vector<Point>&contour, vector<TrafficSign>& trafficSigns);
	static void FindTriangles(const Mat& img, const vector<Point>&contour, vector<TrafficSign>& trafficSigns);
	static void FindCircles(const Mat& bw, vector<TrafficSign>& trafficSigns);
	static void FindEllipses(const Mat& img, const vector<Point>&contour, vector<TrafficSign>& trafficSigns);
	
public:
	~MyDetecter();
	static void LoadFileSignNames(const string& filename);
	static void DetectTrafficSigns(const Mat& imgSrc, vector<TrafficSign>& trafficSigns);
	static void DrawTrafficSigns(Mat& imgSrc, TrafficSign traffic);
	static Mat CutTrafficSign(const Mat& imgSrc, TrafficSign&);
	static void SetLabel(Mat&, const TrafficSign&);
};

