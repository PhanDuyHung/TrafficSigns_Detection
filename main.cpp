#include <Python.h>
#include <iostream>
#include "MyDetecter.h"

/*====================NII=====================*/
/*============NOTHING IS IMPOSSIBLE===========*/
/*1 Jan 2018*/

struct Frame
{
	int id;
	//int label_traffic;
	TrafficSign traffic;
};

void PreSave(vector<Frame>& frameOutput)
{
	//int size = (int)frameOutput.size();
	//for (int i = 0; i < size; i++)
	//{
	//	int count = 0;
	//	int minindex = (i - 5 > 0) ? i - 5 : 0;
	//	int maxindex = (i + 5 < size - 1) ? i + 5 : size - 1;
	//	TrafficSign arr[11];
	//	int freq[11];
	//	for (int j = minindex, k = 0; j <= maxindex; j++)
	//	{
	//		arr[k++] = frameOutput[j].traffic;
	//		if (frameOutput[j].traffic.getId() == frameOutput[i].traffic.getId()) count++;
	//	}
	//	if (count == 1) //chi xuat hien 1 lan -> replace vs bien bao khac gan nhat
	//	{
	//		cout << frameOutput[i].id << "\n";
	//		//tìm biển báo có tần số xuất hiện nhiều nhất
	//		for (int j = 0; j < 11; j++)
	//		{
	//			int f = 0;
	//			for (int p = 1; p < 10; p++)
	//			{
	//				if (arr[p].getId() == arr[j].getId())
	//				{
	//					f++;
	//				}
	//			}
	//			freq[j] = f;
	//			if (f>5) break;
	//		}
	//		int maxf = 0; int labelmax = -1;
	//		for (int j = 0; j<11; j++)
	//		{
	//			if (freq[j] > maxf && freq[j] > 5)
	//			{
	//				maxf = freq[j];
	//				labelmax = arr[j].getId();
	//			}
	//		}
	//		if (labelmax == -1) continue;
	//		for (int j = i; j > 0 && i - j < 5; j--) //duyet tren
	//		{
	//			if (frameOutput[j].traffic.getId() == labelmax)
	//			{
	//				frameOutput[i].traffic = frameOutput[j].traffic;
	//				frameOutput[i].label_traffic = frameOutput[j].label_traffic;
	//				break;
	//			}
	//		}
	//		for (int j = i; j < size && j - i < 5; j++) //duyet duoi
	//		{
	//			if (frameOutput[j].traffic.getId() == labelmax)
	//			{
	//				frameOutput[i].traffic = frameOutput[j].traffic;
	//				frameOutput[i].label_traffic = frameOutput[j].label_traffic;
	//				break;
	//			}
	//		}
	//	}
	//}

	int size = (int)frameOutput.size();
	int stt = frameOutput[0].id;
	for (int i = 1; i < size; i++)
	{
		stt++;
		if (frameOutput[i].id == stt)
		{
			continue;
		}
		Frame f; f.id = stt;
		f.traffic = frameOutput[i - 1].traffic;
		frameOutput.insert(frameOutput.begin() + i, f);
	}
}

void SaveFileOutput(const string& filename, const vector<Frame>& frameOutput)
{
	fstream file;
	file.open(filename, ios::out);

	int n = (int)frameOutput.size();
	file << n << "\n";
	for (int i = 0; i < n; i++)
	{
		int id;
		Point p0, p1;
		if (frameOutput[i].traffic.getId() == -1) //frame hasn't traffic sign
		{
			file << frameOutput[i].id << "\t" << -1 << "\n";
			continue;
		}

		//length = frameOutput[i].traffic.getLength();
		Size size = frameOutput[i].traffic.getSize();
		p0 = Point(frameOutput[i].traffic.getCenTer().x - size.width / 2, frameOutput[i].traffic.getCenTer().y - size.height / 2);
		p1 = Point(frameOutput[i].traffic.getCenTer().x + size.width / 2, frameOutput[i].traffic.getCenTer().y + size.height / 2);
		id = frameOutput[i].traffic.getId();
		file << frameOutput[i].id << "\t" << "2" << "\t" << p0.x << "\t" << p0.y << "\t" << p1.x << "\t" << p1.y << "\n";
	}
	file.close();
}

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

	vector<Frame> frameOutput;
	int width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter videoOut("NII.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(width, height));

	int idFrame = 0;

	while (1)
	{
		Mat frame;
		cap >> frame; // get a new frame from video
		if (!frame.data) break;

		Frame f; f.id = idFrame;
		//bool isHasTraffic = false;
		MyDetecter::DetectTrafficSigns(frame, trafficSigns);
		for each (TrafficSign traffic in trafficSigns)
		{
			//isHasTraffic = true;

			Mat imgTraffic = MyDetecter::CutTrafficSign(frame, traffic);
			imwrite("imageDetected.jpg", imgTraffic);

			//======RECOGNIZE BY PYTHON==========
			PyObject*pyFunc = PyObject_GetAttrString(pyModule, "getPredict");
			PyObject*pyResult = PyObject_CallObject(pyFunc, NULL);
			int img_label = (int)PyFloat_AsDouble(pyResult);

			pyFunc = PyObject_GetAttrString(pyModule, "getAcc");
			pyResult = PyObject_CallObject(pyFunc, NULL);
			float acc = PyFloat_AsDouble(pyResult);

			
			//cout << "frame: " << idFrame << "\n";
			if (acc > MINIMUM_ACCURACY)
			{
				traffic.setId(img_label);
				MyDetecter::SetLabel(frame, traffic, acc);
				MyDetecter::DrawTrafficSigns(frame, traffic);

				/*Push output*/
				f.traffic = traffic;
				//f.label_traffic = img_label;
				frameOutput.push_back(f);
			}
		}
		trafficSigns.clear();

		/*if (!isHasTraffic)
		{
			f.traffic.setId(-1);
			frameOutput.push_back(f);
		}*/

		//MyDetecter::DrawTrafficSigns(frame, trafficSigns,label_require);

		imshow("Traffic Sign", frame);

		videoOut.write(frame);
		if (waitKey(1) >= 0) break;

		idFrame++;
	}

	cout << "\nWrite output file!";
	//PreSave(frameOutput);

	videoOut.release();

	//SaveFileOutput("Output.txt", frameOutput);
	
	cout << "\nSaved!";

	//system("pause");
	return 0;
}