#pragma once
#include <string>
#include <Kinect.h>
#include <vector>
#include <opencv2\opencv.hpp>
void ini_OutputToFile();
void OutputToFile();
//extern vector<OutputToFile> Data;
//����֡����
class NumberOfFrames
{
public:
	void GetTimes(const int Data);
	int Times;
};

//�����ļ������
//class OutputToFile
//{
//public:
//	char *Position[25];
//	float X, Y;
//	void ini_OutputToFile();//��ʼ��
//	void DataToVector(float X, float Y);//�����ݷŵ�vector��
//};