#include "MyDetecter.h"

vector<string> MyDetecter::Responses;

void MyDetecter::LoadFileSignNames(const string& filename)
{
	fstream file;
	file.open(filename, ios::in);
	if (!file)
	{
		cout << "Cannot open " << filename << "\n";
		return;
	}
	while (!file.eof())
	{
		string line = "";
		getline(file, line);
		Responses.push_back(line);
	}
	file.close();
}

void MyDetecter::FindEllipses(const Mat& img, const vector<Point>&contour, vector<TrafficSign>& trafficSigns) {

	if (contour.size() >= 5)
	{
		RotatedRect temp = fitEllipse(Mat(contour));
		//if (abs(temp.size.width - temp.size.height) > 20) continue;
		//=======loại bỏ các hình không nằm hoàn toàn trong image hoac những hình không phải hình tròn
		if (temp.size.width > temp.size.height) //ellipse nam ngang
		{
			if (temp.size.width / temp.size.height > RATIO) return;
			if (temp.center.x - temp.size.height / 2 < 0 || temp.center.x + temp.size.height / 2 > img.cols || temp.center.y - temp.size.width / 2 < 0 || temp.center.y + temp.size.width / 2 > img.rows) return;
		}
		else
		{
			if (temp.size.height / temp.size.width > RATIO) return;
			if (temp.center.x - temp.size.width / 2 < 0 || temp.center.x + temp.size.width / 2 > img.cols || temp.center.y - temp.size.height / 2 < 0 || temp.center.y + temp.size.height / 2 > img.rows) return;
		}

		TrafficSign traffic;
		traffic.setCenTer(temp.center);
		int length = (int)(temp.size.width < temp.size.height) ? (int)temp.size.height : (int)temp.size.width;
		traffic.setLength(length);

		if (length >= MINIMUM_SIZE)
			//push into trafficSigns
			trafficSigns.push_back(traffic);
	}

}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double MyDetecter::Angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void MyDetecter::FindSquares(const Mat& img, const  vector<Point>&contour, vector<TrafficSign>& trafficSigns)
{
	vector<Point> approx;

	// test each contour
	// approximate contour with accuracy proportional
	// to the contour perimeter
	approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true)*0.02, true);

	// square contours should have 4 vertices after approximation
	// relatively large area (to filter out noisy contours)
	// and be convex.
	// Note: absolute value of an area is used because
	// area may be positive or negative - in accordance with the
	// contour orientation
	if (approx.size() == 4 &&
		fabs(contourArea(Mat(approx))) > 1000 &&
		isContourConvex(Mat(approx)))
	{
		double maxCosine = 0;

		for (int j = 2; j < 5; j++)
		{
			// find the maximum cosine of the angle between joint edges
			double cosine = fabs(Angle(approx[j % 4], approx[j - 2], approx[j - 1]));
			maxCosine = MAX(maxCosine, cosine);
		}

		// if cosines of all angles are small
		// (all angles are ~90 degree) then write quandrange
		// vertices to resultant sequence
		if (maxCosine < 0.3)
		{
			TrafficSign traffic;
			//figure out square_center
			Point center = Point((approx[2].x + approx[0].x) / 2, (approx[2].y + approx[0].y) / 2);
			traffic.setCenTer(center);
			int length = approx[3].x - approx[0].x;
			traffic.setLength(length);

			if (length >= MINIMUM_SIZE)
				//push triangle_center into trafficSigns
				trafficSigns.push_back(traffic);
		}

	}
}

void MyDetecter::FindTriangles(const Mat& img, const  vector<Point>&contour, vector<TrafficSign>& trafficSigns)
{

	vector<Point> approx;

	// Approximate contour with accuracy proportional
	// to the contour perimeter
	approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true)*0.02, true);

	// Skip small or non-convex objects
	if (fabs(contourArea(contour)) < 100 || !isContourConvex(approx))
		return;

	if (approx.size() == 3)
	{
		TrafficSign traffic;
		//figure out triangle_center
		//3 dinh tam giac: approx[0] approx[1] approx[2]
		Point A = approx[0], B = approx[1], C = approx[2];

		double ab = sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));
		double bc = sqrt((B.x - C.x)*(B.x - C.x) + (B.y - C.y)*(B.y - C.y));
		double ac = sqrt((A.x - C.x)*(A.x - C.x) + (A.y - C.y)*(A.y - C.y));
		double len;
		Point center, D;
		if (abs(bc - ac) < 1) { //tam giac can tai C
			D = Point((A.x + B.x) / 2, (A.y + B.y) / 2);
			center = Point((C.x + D.x) / 2, (C.y + D.y) / 2);
			len = sqrt((C.x - D.x)*(C.x - D.x) + (C.y - D.y)*(C.y - D.y));
			if (ab > len) len = ab;
		}
		else if (abs(bc - ab) < 1) {//tam giac can tai B
			D = Point((A.x + C.x) / 2, (A.y + C.y) / 2);
			center = Point((B.x + D.x) / 2, (B.y + D.y) / 2);
			len = sqrt((B.x - D.x)*(B.x - D.x) + (B.y - D.y)*(B.y - D.y));
			if (ac > len) len = ac;
		}
		else  { //if (abs(ab - ac) < 1)
			D = Point((C.x + B.x) / 2, (C.y + B.y) / 2);
			center = Point((A.x + D.x) / 2, (A.y + D.y) / 2);
			len = sqrt((A.x - D.x)*(A.x - D.x) + (A.y - D.y)*(A.y - D.y));
			if (bc > len) len = bc;
		}

		traffic.setCenTer(center);
		traffic.setLength((int)len);

		if (len >= MINIMUM_SIZE)
			//push triangle_center into trafficSigns
			trafficSigns.push_back(traffic);
	}

}

void MyDetecter::DetectTrafficSigns(const Mat& imgSrc, vector<TrafficSign>& trafficSigns)
{
	Mat img;
	cvtColor(imgSrc, img, CV_BGR2HSV);

	Mat imgRed, maskRed1, maskRed2, imgBlue, imgYellow, imgBlack, imgWhilte;

	inRange(img, Scalar(SCARLAR_LOWER_RED_1), Scalar(SCARLAR_UPPER_RED_1), maskRed1);
	inRange(img, Scalar(SCARLAR_LOWER_RED_2), Scalar(SCARLAR_UPPER_RED_2), maskRed2);
	cv::add(maskRed1, maskRed2, imgRed);
	threshold(imgRed, imgRed, 127, 255, CV_THRESH_BINARY);

	inRange(img, Scalar(SCARLAR_LOWER_YELLOW), Scalar(SCARLAR_UPPER_YELLOW), imgYellow);
	threshold(imgYellow, imgYellow, 127, 255, CV_THRESH_BINARY);


	inRange(img, Scalar(SCARLAR_LOWER_BLUE), Scalar(SCARLAR_UPPER_BLUE), imgBlue);
	threshold(imgBlue, imgBlue, 127, 255, CV_THRESH_BINARY);

	// find contours and store them all as a list
	vector<vector<Point> >contours, contours_temp;
	findContours(imgRed, contours_temp, RETR_LIST, CHAIN_APPROX_SIMPLE);
	for each (vector<Point> contour in contours_temp)
	{
		contours.push_back(contour);
	}
	findContours(imgBlue, contours_temp, RETR_LIST, CHAIN_APPROX_SIMPLE);
	for each (vector<Point> contour in contours_temp)
	{
		contours.push_back(contour);
	}
	findContours(imgYellow, contours_temp, RETR_LIST, CHAIN_APPROX_SIMPLE);
	for each (vector<Point> contour in contours_temp)
	{
		contours.push_back(contour);
	}

	for each (vector<Point> contour in contours)
	{
		/*=============SQUARE=============*/
		FindSquares(img, contour, trafficSigns);

		/*===========TRIANGLE=============*/
		FindTriangles(img, contour, trafficSigns);

		/*============CIRCLE===============*/
		FindEllipses(img, contour, trafficSigns);
	}
	//sap xep theo thu giam dan
	stack<int> s;
	int left = 0, right = (int)trafficSigns.size() - 1;
	s.push(left); s.push(right);
	while (!s.empty())
	{
		right = s.top(); s.pop();
		left = s.top(); s.pop();
		int i = left, j = right, k = (i + j) / 2;
		while (i <= j)
		{
			while (trafficSigns[i].getLength() < trafficSigns[k].getLength()) i++;
			while (trafficSigns[j].getLength() > trafficSigns[k].getLength()) j--;
			if (i <= j)
			{
				TrafficSign t = trafficSigns[i];
				trafficSigns[i] = trafficSigns[j];
				trafficSigns[j] = t;
				i++; j--;
			}
		}
		if (i < right) {
			s.push(i); s.push(right);
		}
		if (j > left){
			s.push(left); s.push(j);
		}
	}

	//xoa nhung duong tron nam trong duong tron khac
	for (int i = (int)trafficSigns.size() - 1; i > 0; --i) {
		Point center_i = trafficSigns[i].getCenTer();
		float radius_i = (float)trafficSigns[i].getLength() / 2;

		for (int j = i - 1; j >= 0; --j){
			Point center_j = trafficSigns[j].getCenTer();
			float radius_j = (float)trafficSigns[j].getLength() / 2;

			double d = sqrt((center_i.x - center_j.x)*(center_i.x - center_j.x) + (center_i.y - center_j.y)*(center_i.y - center_j.y));
			if (d < radius_i + radius_j)
			{
				trafficSigns.erase(trafficSigns.begin() + j);
				i--;
			}
		}
	}
}

void MyDetecter::DrawTrafficSigns(Mat& imgSrc,TrafficSign traffic)
{
	//for each (TrafficSign traffic in trafficSigns)
	{
		//if (traffic.getId() == label_require)
		{
			Point center = traffic.getCenTer();
			int length = traffic.getLength();
			//tìm tọa độ 4 đỉnh A B C D
			Point A, B, C, D;
			A = Point(center.x - length / 2, center.y - length / 2);
			B = Point(center.x + length / 2, center.y - length / 2);
			C = Point(center.x + length / 2, center.y + length / 2);
			D = Point(center.x - length / 2, center.y + length / 2);
			//Draw square
			line(imgSrc, A, B, Scalar(SCALAR_SOLID), THICKNESS);
			line(imgSrc, B, C, Scalar(SCALAR_SOLID), THICKNESS);
			line(imgSrc, C, D, Scalar(SCALAR_SOLID), THICKNESS);
			line(imgSrc, D, A, Scalar(SCALAR_SOLID), THICKNESS);
		}
	}
	//trafficSigns.clear();
}

Mat MyDetecter::CutTrafficSign(const Mat& imgSrc, TrafficSign& traffic) {

	Point c = traffic.getCenTer();
	int a = traffic.getLength() / 2;
	Range x(0, 0), y(0, 0);
	x.start = (c.x - a >= 0) ? c.x - a : 0;
	x.end = (c.x + a <= imgSrc.cols) ? c.x + a : imgSrc.cols;
	y.start = (c.y - a >= 0) ? c.y - a : 0;
	y.end = (c.y + a <= imgSrc.rows) ? c.y + a : imgSrc.rows;

	Mat temp(imgSrc, y, x);
	return temp;
}

void MyDetecter::SetLabel(Mat& im, const TrafficSign& traffic)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.5;
	int thickness = 1;
	int baseline = 0;
	if (!Responses.empty())
	{
		string label = Responses[traffic.getId()];
		Point point = traffic.getCenTer();
		cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
		//cv::Rect r = cv::boundingRect(contour);

		//cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
		cv::rectangle(im, point + cv::Point(0, baseline), point + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
		cv::putText(im, label, point, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
	}

}
