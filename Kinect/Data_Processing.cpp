#include "Data_Acquisition.h"
#include "Data_Processing.h"
#include <iostream>
std::string The_Position[25];//����ؽ�λ��
void NumberOfFrames::GetTimes(const int Data)
{
	Times = Data;
}

void ini_OutputToFile()
{
	char a[][30] = {
		"1      Spine_Base: ",//�����ײ�
		"2       Spine_Mid: ",//�����в�
		"3            Neck: ",//����
		"4            Head: ",//ͷ
		"5   Shoulder_Left: ",//���
		"6      Elbow_Left: ",//����
		"7      Wrist_Left: ",//������
		"8       Hand_Left: ",//����
		"9  Shoulder_Right: ",//�Ҽ��
		"10    Elbow_Right: ",//����
		"11    Wrist_Right: ",//������
		"12     Hand_Right: ",//����
		"13       Hip_Left: ",//����
		"14      Knee_Left: ",//��ϥ
		"15     Ankle_Left: ",//����
		"16      Foot_Left: ",//����
		"17      Hip_Right: ",//����
		"18     Knee_Right: ",//��ϥ
		"19    Ankle_Right: ",//����
		"20     Foot_Right: ",//�ҽ�
		"21 Spine_Shoulder: ",//������
		"22  Hand_Tip_Left: ",//���ֲ�����
		"23     Thumb_Left: ",//��Ĵָ
		"24 Hand_Tip_Right: ",//���ֲ�����
		"25    Thumb_Right: "//��Ĵָ
	};//λ������
	for (int i = 0; i < 25; i++)
		The_Position[i] = a[i];
}

//void DataToVector(float X, float Y)
//{
//	OutputToFile tmp;
//	tmp.X = X;
//	tmp.Y = Y;
//	Data.push_back(tmp);
//}