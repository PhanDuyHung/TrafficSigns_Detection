#include <Python.h>
#include <iostream>
#include "MyDetecter.h"

/*====================NII=====================*/
/*============NOTHING IS IMPOSSIBLE===========*/
/*1 Jan 2018*/

int main(int argc, wchar_t* argv[]){

	vector<TrafficSign> trafficSigns;

	MyDetecter::LoadFileSignNames("./traffic-sign-model/signnames.txt");

	// Init python
	Py_Initialize();
	PySys_SetArgv(argc, argv);

	//Import Module
	PyObject* pyModule = PyImport_ImportModule("Predict");
	PyErr_Occurred();
	PyErr_Print();

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

	/*int width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter videoOut("NII.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(width, height));
*/
	while (1)
	{
		Mat frame;
		cap >> frame; // get a new frame from video
		if (!frame.data) break;

		MyDetecter::DetectTrafficSigns(frame, trafficSigns);
		for each (TrafficSign traffic in trafficSigns)
		{
			Mat imgTraffic = MyDetecter::CutTrafficSign(frame, traffic);
			imwrite("imageDetected.jpg", imgTraffic);

			//======RECOGNIZE BY PYTHON==========
			PyObject*pyFunc = PyObject_GetAttrString(pyModule, "getPredict");
			PyObject*pyResult = PyObject_CallObject(pyFunc, NULL);
			int img_label = (int)PyFloat_AsDouble(pyResult);

			pyFunc = PyObject_GetAttrString(pyModule, "getAcc");
			pyResult = PyObject_CallObject(pyFunc, NULL);
			float acc = PyFloat_AsDouble(pyResult);

			if (acc > MINIMUM_ACCURACY)
			{
				traffic.setId(img_label);
				MyDetecter::SetLabel(frame, traffic, acc);
				MyDetecter::DrawTrafficSigns(frame, traffic);
		
			}
		}
		trafficSigns.clear();

		imshow("Traffic Sign", frame);

		//videoOut.write(frame);
		if (waitKey(1) >= 0) break;
	}
	return 0;
}