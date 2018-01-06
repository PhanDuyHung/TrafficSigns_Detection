#include <iostream>
#include <opencv2\opencv.hpp>
#include <sstream>
#include "dirent.h"

void ResizeMul(std::string inputDirectory, std::string outputDirectory, int min, int max)
{

	DIR *directory = opendir(inputDirectory.c_str());
	struct dirent *_dirent = NULL;
	if (directory == NULL)
	{
		printf("Cannot open Input Folder\n");
		return;
	}

	while ((_dirent = readdir(directory)) != NULL)
	{
		int i = 0;
		int size = min;
		std::string fileName = inputDirectory + "\\" + std::string(_dirent->d_name);
		cv::Mat rawImage = cv::imread(fileName.c_str());
		if (rawImage.data == NULL)
		{
			printf("Cannot Open Image\n");
			continue;
		}
		// Add your any image filter here
		cv::Mat imageOut;
		while (size <= max)
		{
			cv::resize(rawImage, imageOut, cv::Size(size, size));

			std::string sIndex;
			std::stringstream out;
			out << i;
			sIndex = out.str();
			i++;

			std::string name = _dirent->d_name;
			name = name.substr(0, name.length() - 4) + "-" + sIndex;
			fileName = outputDirectory + "\\" + name + ".jpg";
			cv::imwrite(fileName.c_str(), imageOut);

			size++;
		}
	}
	closedir(directory);
}

void Resize32(std::string inputDirectory, std::string outputDirectory)
{
	DIR *directory = opendir(inputDirectory.c_str());
	struct dirent *_dirent = NULL;
	if (directory == NULL)
	{
		printf("Cannot open Input Folder\n");
		return;
	}

	while ((_dirent = readdir(directory)) != NULL)
	{
		std::string fileName = inputDirectory + "\\" + std::string(_dirent->d_name);
		cv::Mat rawImage = cv::imread(fileName.c_str());
		if (rawImage.data == NULL)
		{
			printf("Cannot Open Image\n");
			continue;
		}
		// Add your any image filter here
		cv::Mat imageOut;

		cv::resize(rawImage, imageOut, cv::Size(32, 32));


		fileName = outputDirectory + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), imageOut);

	}
	closedir(directory);
}

int main()
{
	std::string inputDirectory = "C:\\Users\\LAPTOP64\\Desktop\\trafiic-signs-data\\images_data\\dataset\\new";
	std::string outputDirectory = "C:\\Users\\LAPTOP64\\Desktop\\trafiic-signs-data\\images_data\\dataset\\out";
	/*resize multiple size 18-32*/
	ResizeMul(inputDirectory, outputDirectory,18,38);
		
	
	inputDirectory = outputDirectory;
	/*resize 32*/
	Resize32(inputDirectory, outputDirectory);
		
	
	system("pause");
	return 0;
}