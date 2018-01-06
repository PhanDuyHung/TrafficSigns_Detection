#include <Python.h>
#include <iostream>
#include "MyDetecter.h"

/*====================NII=====================*/
/*============NOTHING IS IMPOSSIBLE===========*/
/*1 Jan 2018*/

struct Frame
{
	int id;
	TrafficSign traffic;
};


void SaveFileOutput(const string& filename, const vector<Frame>& frameOutput)
{
	fstream file;
	file.open(filename, ios::out);

	int n = (int)frameOutput.size();
	file << n << "\n";
	for (int i = 0; i < n; i++)
	{
		int id, length;
		Point p0, p1;
		if (frameOutput[i].traffic.getId() == -1) //frame hasn't traffic sign
		{
			file << i << "\t" << -1<<"\n";
			continue;
		}
	
		length = frameOutput[i].traffic.getLength();
		p0 = Point(frameOutput[i].traffic.getCenTer().x - length / 2, frameOutput[i].traffic.getCenTer().y - length / 2);
		p1 = Point(frameOutput[i].traffic.getCenTer().x + length / 2, frameOutput[i].traffic.getCenTer().y + length / 2);
		id = frameOutput[i].traffic.getId();
		file << i << "\t" << id << "\t" << p0.x << "\t" << p0.y << "\t" << p1.x << "\t" << p1.y << "\n";
	}
	file.close();
}

int main(int argc, wchar_t* argv[]){

	vector<TrafficSign> traffiSigns;

	/*Mat img = imread("test_images\\stop.jpg");
	MyDetecter::DetectTrafficSigns(img, traffiSigns);
	MyDetecter::DrawTrafficSigns(img, traffiSigns);
	imshow("square", img);
	waitKey(0);*/

	MyDetecter::LoadFileSignNames("./lenet/lenet/signnames.txt");

	// Init python
	Py_Initialize();
	PySys_SetArgv(argc, argv);

	//Import Module
	PyObject* pyModule = PyImport_ImportModule("TrafficReg");

	/*================================================*/
	
	
	VideoCapture cap; 
	while (1)
	{
		string videopath;
		cout << "Video path: ";
		cin >> videopath;
		cap.open(videopath);
		if (!cap.isOpened())
		{
			cout << "Video not found!\n";
			continue;
		}
		break;
	}

	vector<Frame> frameOutput;
	
	int idFrame = 0;
	while (1)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		if (!frame.data) break;

		Frame f; f.id = idFrame;
		bool isHasTraffic = false;
		MyDetecter::DetectTrafficSigns(frame, traffiSigns);
		for each (TrafficSign traffic in traffiSigns)
		{
			isHasTraffic = true;

			Mat imgTraffic = MyDetecter::CutTrafficSign(frame, traffic);
			//imshow("test", imgTraffic);
			imwrite("./images/imageDetected.jpg", imgTraffic);

			//======RECOGNIZE BY PYTHON==========
			PyObject*pyFunc = PyObject_GetAttrString(pyModule, "Recognize");
			PyErr_Occurred();
			PyErr_Print();
			PyObject*pyResult = PyObject_CallObject(pyFunc, NULL);
			int img_label = (int)PyFloat_AsDouble(pyResult);

			traffic.setId(img_label);
			MyDetecter::SetLabel(frame, traffic);

			/*Push output*/
			f.traffic = traffic;
			frameOutput.push_back(f);

			PyErr_Occurred();
			PyErr_Print();
		}

		if (!isHasTraffic)
		{
			f.traffic.setId(-1);
			frameOutput.push_back(f);
		}

		MyDetecter::DrawTrafficSigns(frame, traffiSigns);

		imshow("Traffic Sign", frame);
		if (waitKey(10) >= 0) break;

		idFrame++;
	}

	cout << "\nWrite output file!";
	SaveFileOutput("output.txt", frameOutput);
	cout << "\nSaved!";


	return 0;
}