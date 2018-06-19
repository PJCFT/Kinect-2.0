#include <cmath>
#include <iostream>
#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>  /* ERROR Number Definitions          */
#include <cstdlib>
#include <windows.h>
using namespace std;

#ifndef SERIAL_H_
#define SERIAL_H_

#define BAUDRATE B57600
#define MODEMDEVICE "/dev/ttyUSB0"

#define START_SYSEX                 (0XF0)
#define END_SYSEX                   (0XF7)
#define UARM_CODE                   (0XAA)

#define READ_ANGLE                  (0X10)
#define WRITE_ANGLE                 (0X11)
#define READ_COORDS                 (0X12)
#define WRITE_COORDS                (0X13)
#define READ_DIGITAL                (0X14)
#define WRITE_DIGITAL               (0X15)
#define READ_ANALOG                 (0X16)
#define WRITE_ANALOG                (0X17)
#define READ_EEPROM                 (0X1A)
#define WRITE_EEPROM                (0X1B)
#define DETACH_SERVO                (0X1C)
#define PUMP_STATUS                 (0X1D)
#define WRITE_STRETCH               (0X1E)
#define WRITE_LEFT_RIGHT_ANGLE      (0X1F)
#define GRIPPER_STATUS              (0X20)
#define READ_SERIAL_NUMBER          (0x21)
#define WRITE_SERIAL_NUMBER         (0x22)
#define REPORT_LIBRARY_VERSION      (0x23)
#define BUZZER_ALERT                (0x24)

#define ON 1
#define OFF 0



HANDLE hComm;
OVERLAPPED m_ov;




#endif


/********************************************************************/
/*				 statement about class of Uarm						*/
/*	���캯����														*/
/*				Uarm()		Ĭ�ϴ򿪶˿�"com3",��������dcb			*/
/*				Uarm(char port)		�򿪶˿�port����������Dcb		*/
/*	����������														*/
/*				~Uarm()     �Ͽ����ӣ��رն˿�						*/
/*	public������													*/
/*				bool setupdcb(int rate_arg)		����DCB				*/
/*				void writeServoAngele(int x)	���õ�һ�������Ƕ�	*/
/*				void writeStretchHeight(int  y,int z)	����y,z		*/
/*				void move(int x,int y,int z)	�ƶ���(x,y,z)		*/
/*				void Pump_control(int control)	��������״̬		*/
/*				void writeh(float angle)		���̽Ƕ�			*/
/*	private����														*/
/*				void write(unsigned char msg[],int size)			*/
/*												д���ַ�����		*/
/*				void sendInteger(int val)		д�����ͺ���		*/
/*				void getValueAsThree7bitBytes(float val)			*/
/*												д�����ݺ���		*/
/*				void sendFloatAsFour7bitBytes(int val)				*/
/*												д�����ݺ���		*/
/********************************************************************/




class Uarm
{
private:
	//д�뺯��

	void write(unsigned char msg[], int size)  //д���ַ���
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;
		DWORD BytesSent = 0;

		WriteFile(hComm, // Handle to COMM Port
			msg, // Pointer to message buffer in calling finction
			size,      // Length of message to send
			&BytesSent,     // Where to store the number of bytes sent
			&m_ov);
	}

	void sendInteger(int val)   //д��int ����
	{
		unsigned char msg[1];
		msg[0] = val;
		write(msg, 1);
	}

	//д�����ݺ���
	void getValueAsThree7bitBytes(float val)
	{
		int int_val = int(val);
		int decimal_val = (int)((val - int_val) * 100);
		unsigned char msg[3] = { 0 };
		msg[0] = abs(int_val) & 0x7F;
		msg[1] = abs(int_val) >> 7 & 0x7F;
		msg[2] = abs(decimal_val) & 0x7F;
		write(msg, 3);
	}


	void sendFloatAsFour7bitBytes(int val)
	{                                             //����
		int int_val = val;
		int decimal_val = (int)((val - int_val) * 100);
		unsigned char msg[4] = { 0 };
		msg[0] = val > 0 ? 0 : 1;
		msg[1] = abs(int_val) & 0x7F;
		msg[2] = abs(int_val) >> 7 & 0x7F;
		msg[3] = abs(decimal_val) & 0x7F;
		write(msg, 4);
	}

public:
	//�򿪴��ڣ�Ĭ��com3
	Uarm()
	{
		hComm = CreateFile(L"com3", //���ں�
			GENERIC_READ | GENERIC_WRITE, //�����д
			0, //ͨѶ�豸�����Զ�ռ��ʽ��
			0, //�ް�ȫ����
			OPEN_EXISTING, //ͨѶ�豸�Ѵ���
			0, //ͬ��I/O
			0); //ͨѶ�豸������ģ���
		if (hComm == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
		}
		else
		{
			cout << "open comport success" << endl;
		}
		setupdcb(57600);
	}


	//��ָ������
	Uarm(char *portname)
	{
		hComm = CreateFile(L"portname", //���ں�
			GENERIC_READ | GENERIC_WRITE, //�����д
			0, //ͨѶ�豸�����Զ�ռ��ʽ��
			0, //�ް�ȫ����
			OPEN_EXISTING, //ͨѶ�豸�Ѵ���
			0, //ͬ��I/O
			0); //ͨѶ�豸������ģ���
		if (hComm == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
		}
		else
		{
			cout << "open comport success" << endl;
		}

		setupdcb(57600);
	}


	//�رմ���
	~Uarm()
	{
		cout << "�رմ��ڣ��Ͽ�����!" << endl;
		move(90, 0, 0);
		CloseHandle(hComm);
	}


	//����Uarm
	bool setupdcb(int rate_arg)//����DCB
	{
		DCB dcb;
		int rate = rate_arg;
		memset(&dcb, 0, sizeof(dcb));
		if (!GetCommState(hComm, &dcb))//��ȡ��ǰDCB����
			return FALSE;
		printf("������:%d\n", dcb.BaudRate);  //��ȡ��ǰ������

		// set DCB to configure the serial port
		dcb.DCBlength = sizeof(dcb);
		/* ---------- Serial Port Config ------- */			//��������

		dcb.BaudRate = rate;			//���ò�����  UarmΪ57600
		dcb.Parity = NOPARITY;           //��У��
		dcb.fParity = 0;					//��żУ����
		dcb.StopBits = ONESTOPBIT;      //ֹͣλ
		dcb.ByteSize = 8;				//8bit
		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow = 0;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fDsrSensitivity = 0;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutX = 0;
		dcb.fInX = 0;
		/* ----------------- misc parameters ----- */   //����������
		dcb.fErrorChar = 0;
		dcb.fBinary = 1;
		dcb.fNull = 0;
		dcb.fAbortOnError = 0;
		dcb.wReserved = 0;
		dcb.XonLim = 2;
		dcb.XoffLim = 4;
		dcb.XonChar = 0x13;
		dcb.XoffChar = 0x19;
		dcb.EvtChar = 0;
		// set DCB
		if (!SetCommState(hComm, &dcb))
			return false;
		else
			return true;
	}


	//x��(0~��)
	void writeServoAngele(int x)  //�Ƕ�
	{
		sendInteger(START_SYSEX);
		sendInteger(UARM_CODE);
		sendInteger(WRITE_ANGLE);
		sendInteger(0);
		//	sendInteger(x-128);
		getValueAsThree7bitBytes(x);
		sendInteger(-1);
		sendInteger(END_SYSEX);
	}
	//y,z��

	void writeStretchHeight(int  y, int z)
	{
		sendInteger(START_SYSEX);
		sendInteger(UARM_CODE);
		sendInteger(WRITE_STRETCH);
		sendFloatAsFour7bitBytes(y);
		sendFloatAsFour7bitBytes(z);
		sendInteger(END_SYSEX);
	}
	//x,y,z

	void move(int x, int y, int z)
	{
		writeServoAngele(x);
		writeStretchHeight(y, z);
	}
	//����

	void Pump_control(int control)  //  1.��ȡ 0.����
	{
		sendInteger(START_SYSEX);
		sendInteger(UARM_CODE);
		sendInteger(PUMP_STATUS);
		sendInteger(control);
		sendInteger(END_SYSEX);
		cout << endl;
	}
	void writeh(float angle)      //����ת��
	{
		sendInteger(START_SYSEX);
		sendInteger(UARM_CODE);
		sendInteger(WRITE_ANGLE);
		sendInteger(3);
		getValueAsThree7bitBytes(angle);
		sendInteger(0);
		sendInteger(END_SYSEX);
		printf("/n");
	}


};


/*void main()
{
	Uarm A;
	A.move(10, 20, 30);
	Sleep(10000);

}*/

#include "Data_Acquisition.h"
#include "Data_Processing.h"
#include "math.h"
#include <iostream>
#include <fstream>

using namespace std;

#define TheNumberOfFlip 30*40 //ÿ��30֡
#define M_PI 3.14159265358979323846//����PI

std::vector<float> Data_X;	//�����������X������
std::vector<float> Data_Y;	//�����������Y������
std::vector<float> Data_Z;	//��������������������

extern std::string The_Position[25];//����ؽ�λ��

int main()
{
	int ab = 0;
	Uarm A;
	A.move(90, 0, 0);
	Sleep(2 * 1000);
	NumberOfFrames Flip;//����֡
	ofstream outfile;//�������
	CBodyBasics myKinect;//����kinect��ʼ������
	Flip.GetTimes(TheNumberOfFlip);//��ȡʱ��
	ini_OutputToFile();//��ʼ��
	double * array1 = new double[5];
	HRESULT hr = myKinect.InitializeDefaultSensor();//��ʼ���豸

	if (SUCCEEDED(hr))
	{
		while (1)
		{
			//myKinect.Update(array1);
			array1 = myKinect.Update(array1);

			if (array1[0] < 0)
			{
				array1[0] = sqrt(array1[0] * array1[0] + array1[2] * array1[2]);
				array1[2] = atan(abs(-array1[2] / array1[0]));
				array1[2] = 90 - array1[2] * (180 / M_PI);
				array1[0] = array1[0] * 500;
	/*			if (array1[0] < 0 || array1[0] > 210)
				{
					if (array1[0] < 0)
					{
						array1[0] = 0;
					}
					else
					{
						array1[0] = 210;
					}
				}
				if (array1[2] < 0 || array1[2] > 180)
				{
					if (array1[2] < 0)
					{
						array1[2] = 0;
					}
					else
					{
						array1[2] = 180;
					}
				}
	*/
			}
			else
			{
				array1[0] = sqrt(array1[0] * array1[0] + array1[2] * array1[2]);
				array1[2] = atan(-array1[2] / array1[0]);
				array1[2] = array1[2] * (180 / M_PI);
				array1[0] = array1[0] * 500;
	/*			if (array1[0] < 0 || array1[0] > 210)
				{
					if (array1[0] < 0)
					{
						array1[0] = 0;
					}
					else
					{
						array1[0] = 210;
					}
				}
				if (array1[2] < 0 || array1[2] > 180)
				{
					if (array1[2] < 0)
					{
						array1[2] = 0;
					}
					else
					{
						array1[2] = 180;
					}
				}
	*/
			}
			array1[1] = array1[1] * 500;
	/*		if (array1[1] > 90 || array1[1] < -150)
			{
				if (array1[1] < -150)
				{
					array1[1] = -150;
				}
				else
				{
					array1[1] = 90;
				}
			}
			cout << array1[0] << " ";
			cout << array1[1] << " ";
			cout << array1[2] * 2 << " ";
			cout << array1[3] << endl;
	*/
			if (array1[2] >= 0 && array1[2] <= 180)
			{
				if (array1[0] >= 0 && array1[0] <= 210)
				{
					if (array1[1] >= -180 && array1[1] <= 120)
					{
						cout << (int)array1[0] << " ";
						cout << (int)array1[1] << " ";
						cout << (int)array1[2] * 2 << endl;
						Sleep(10);
						//A.move((int)(array1[2] * 2), (int)array1[0], (int)array1[1]);
						//A.move(0, 0, (int)array1[1]);
					}
				}
			}
			//A.move(array1[2],array1[0] ,array1[1] );  //��е���˶�����;

			switch ((int)array1[3])    //�ж��Ƿ������ǲ���;�����ſ���ʾ�������ϱ�ʾ����;
			{
			case 1:
				if (ab == 1)
				{
					A.Pump_control(0);
					ab = 0;
					break;
				}
			case 2:
				if (ab == 0)
				{
					A.Pump_control(1);
					ab = 1;
					break;
				}
			}
			A.Pump_control(array1[3] == 2);  //����ѭ���������ſ����������������ſ�������ѭ��;
			if (array1[4] == 2)
			{
				break;
			}
		}
		free(array1);
		//system("pause");
		A.move(90, 0, 0);
/*		cout << "�������£�" << endl;
		static int h = 1;
		static double a, b, d, c, e, f;
		for (int i = 0; i < Data_X.size(); ++i)
		{
			if (i % 25 == 7 || i % 25 == 11)
			{
				if (h == 1 && i % 25 == 7)
				{
					a = Data_X[i];
					b = Data_Y[i];
					c = Data_Z[i];
					h++;
				}
				else if (h > 1 && i % 25 == 11)
				{
					d = Data_X[i];
					e = Data_Y[i];
					f = Data_Z[i];
					cout << d - a << " ";
					cout << e - b << " ";
					cout << f - c << endl;
				}
			}
		}
		system("pause");*/
//		int64 Size = Data_X.size();//��ȡ������С
		/*****************************��ʼд�ļ�*************************************/
/*		outfile.open("1.txt");
		for (int i = 0; i < Size; ++i)
		{
			if (!(i % 25) && i != 0)
				outfile << "\n";
			outfile << The_Position[i % 25] << " ";
			outfile << "<" << Data_X[i] << " , ";
			outfile << Data_Y[i] << " , ";
			outfile << Data_Z[i] << ">";
			outfile << "\n";

		}
		outfile.close();
		/******************************д�ļ����**********************************/
	}
	else{
		cout << "kinect initialization failed!" << endl;
		system("pause");
	}
}

















