#include "Data_Acquisition.h"
#include "Data_Processing.h"
#include <iostream>
extern std::vector<float> Data_X;
extern std::vector<float> Data_Y;
extern std::vector<float> Data_Z;
using namespace std;
//const char* path;
int frame_num = 0;

/// ��ʼ��kinect����
HRESULT CBodyBasics::InitializeDefaultSensor()
{

	//�����ж�ÿ�ζ�ȡ�����ĳɹ����
	HRESULT hr;

	//����kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)){
		return hr;
	}

	//�ҵ�kinect�豸
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//��ȡ�Ǽ�
		IDepthFrameSource* pDepthFrameSource = NULL;//��ȡ�����Ϣ
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//��ȡ������ֵͼ

		//��kinect
		hr = m_pKinectSensor->Open();
		 
		//����ӳ��
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//����Ǽ�
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}
		//��ȡ����Ǽ�
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		//���frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}
		//��ȡ���
		if (SUCCEEDED(hr)){
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//�����ֵframe
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pBodyIndexFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "Kinect initialization failed!" << std::endl;
		return E_FAIL;
	}

	//skeletonImg,���ڻ��Ǽܡ�������ֵͼ��MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,���ڻ������Ϣ��MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	return hr;
}


/// ��Ҫ������
//double* CBodyBasics::Update(double *array)
//void CBodyBasics::Update()
double* CBodyBasics::Update(double *array)
{
	
	//ÿ�������skeletonImg
	skeletonImg.setTo(0);

	//�����ʧ��kinect���򲻼�������
	if (!m_pBodyFrameReader)
	{
		exit(1);
	}
	IBodyFrame* pBodyFrame = NULL;//�Ǽ���Ϣ
	IDepthFrame* pDepthFrame = NULL;//�����Ϣ
	IBodyIndexFrame* pBodyIndexFrame = NULL;//������ֵͼ

	//��¼ÿ�β����ĳɹ����
	HRESULT hr = S_OK;

	//---------------------------------------��ȡ������ֵͼ����ʾ---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//��ñ�����ֵͼ��Ϣ
	}
	if (SUCCEEDED(hr)){
		BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//������ֵͼ��8Ϊuchar
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//�ѱ�����ֵͼ����MAT��
		uchar* skeletonData = (uchar*)skeletonImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
		}
		delete[] bodyIndexArray;
	}
	SafeRelease(pBodyIndexFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

	//-----------------------��ȡ������ݲ���ʾ--------------------------
	if (SUCCEEDED(hr)){
		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//����������
	}
	if (SUCCEEDED(hr)){
		UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//���������16λunsigned int
		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

		//��������ݻ���MAT��
		uchar* depthData = (uchar*)depthImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
			*depthData = depthArray[j];
			++depthData;
		}
		delete[] depthArray;
	}
	SafeRelease(pDepthFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	imshow("���", depthImg);
	cv::waitKey(5);
	//-----------------------------��ȡ�Ǽܲ���ʾ----------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//��ȡ�Ǽ���Ϣ
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//ÿһ��IBody����׷��һ���ˣ��ܹ�����׷��������

		if (SUCCEEDED(hr))
		{
			//��kinect׷�ٵ����˵���Ϣ���ֱ�浽ÿһ��IBody��
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);//_countof(ppBodies)�������
		}

		if (SUCCEEDED(hr))
		{
			//��ÿһ��IBody�������ҵ����ĹǼ���Ϣ�����һ�����
			//ProcessBody(BODY_COUNT, ppBodies);
			//��¼��������Ƿ�ɹ�
			char image_name[30];
			HRESULT hr;
			//path = "E:\\Data\\right\\right.bmp";

			//����ÿһ��IBody
			for (int i = 0; i < BODY_COUNT; ++i)
			{
				IBody* pBody = ppBodies[i];
				if (pBody)
				{
					BOOLEAN bTracked = false;
					hr = pBody->get_IsTracked(&bTracked);

					if (SUCCEEDED(hr) && bTracked)
					{
						Joint joints[JointType_Count];//�洢�ؽڵ���
						HandState leftHandState = HandState_Unknown;//����״̬
						HandState rightHandState = HandState_Unknown;//����״̬

						//��ȡ������״̬
						pBody->get_HandLeftState(&leftHandState);
						pBody->get_HandRightState(&rightHandState);

						//�洢�������ϵ�еĹؽڵ�λ��
						DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

						//��ùؽڵ���
						hr = pBody->GetJoints(_countof(joints), joints);
						if (SUCCEEDED(hr))
						{
							for (int j = 0; j < _countof(joints); ++j)
							{
								//���ؽڵ���������������ϵ ת�� �������ϵ��424*512��
								m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
							}
						/*	static int h = 1;
							static double a, b, d, c, e, f;
							for (int i = 0; i < 25; ++i)
							{
								if (i % 25 == 7 || i % 25 == 11)
								{
									if (h ==1 && i % 25 == 7)
									{
										a = joints[i].Position.X;
										b = joints[i].Position.Y;
										c = joints[i].Position.Z;
										h++;
									}
									else if (h > 1 && i % 25 == 11) 
									{
										d = joints[i].Position.X;
										e = joints[i].Position.Y;
										f = joints[i].Position.Z;
										//cout << d - a << " ";
										//cout << e - b << " ";
										//cout << f - c << endl;
										array[0] = (d - a) * 10;
										array[1] = (e - b) * 10;
										array[2] = (f - c) * 10;
										//cout << array[0] << " ";
										//cout << array[1] << " ";
										//cout << array[2] << endl;
									}
								}
							}
							*/
							static int h = 1;
							static double a, b, c, d, e, f;
							for (int i = 0; i < 25; ++i)
							{
								if (i % 25 == 11)
								{
									if (h == 1)
									{
										a = joints[i].Position.X;
										b = joints[i].Position.Y;
										c = joints[i].Position.Z;
										if (a == NULL && b == NULL && c == NULL) break;
										h++;
									}
									else if (h > 1)
									{
										d = joints[i].Position.X;
										e = joints[i].Position.Y;
										f = joints[i].Position.Z;
								//		array[0] = (d - a);
								//		array[1] = (e - b);
								//		array[2] = (f - c);
									}
								}
							}
							array[0] = (d - a) * 30;
							array[1] = (e - b) * 28;
							array[2] = (f - c) * -50;
							/*
							static int h = 1;
							static double a, b, c, d, e, f, g, j, l;
							for (int i = 0; i < 25; ++i)
							{
								if (h == 1)
								{
									if (i % 25 == 7)
									{
										a = joints[i].Position.X;
										b = joints[i].Position.Y;
										c = joints[i].Position.Z;
									}
									else if (i % 25 == 11)
									{
										d = joints[i].Position.X - a;
										e = joints[i].Position.Y - b;
										f = joints[i].Position.Z - c;
									}
									h++;
								}
								else if (h > 1 && i % 25 == 11)
								{
									g = joints[i].Position.X - a - d;
									j = joints[i].Position.Y - b - e;
									l = joints[i].Position.Z - c - f;
									array[0] = g;
									array[1] = h;
									array[2] = l;
								}
							}
							*/

							for (int k = 0; k < 25; k++)//�����ݱ��浽vector��
							{
								//Data_X.push_back(depthSpacePosition[k].X);
								//Data_Y.push_back(depthSpacePosition[k].Y);
								Data_X.push_back(joints[k].Position.X);
								Data_Y.push_back(joints[k].Position.Y);
								Data_Z.push_back(joints[k].Position.Z);

							}

							//------------------------hand state left-------------------------------
							DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
							DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);
							array[3] = rightHandState;
							array[4] = leftHandState;

							//---------------------------body-------------------------------
							DrawBone(joints, depthSpacePosition, JointType_Head, JointType_Neck);
							DrawBone(joints, depthSpacePosition, JointType_Neck, JointType_SpineShoulder);
							DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_SpineMid);
							DrawBone(joints, depthSpacePosition, JointType_SpineMid, JointType_SpineBase);
							DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderRight);
							DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderLeft);
							DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipRight);
							DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipLeft);

							// -----------------------Right Arm ------------------------------------ 
							DrawBone(joints, depthSpacePosition, JointType_ShoulderRight, JointType_ElbowRight);
							DrawBone(joints, depthSpacePosition, JointType_ElbowRight, JointType_WristRight);
							DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_HandRight);
							DrawBone(joints, depthSpacePosition, JointType_HandRight, JointType_HandTipRight);
							DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_ThumbRight);

							//----------------------------------- Left Arm--------------------------
							DrawBone(joints, depthSpacePosition, JointType_ShoulderLeft, JointType_ElbowLeft);
							DrawBone(joints, depthSpacePosition, JointType_ElbowLeft, JointType_WristLeft);
							DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_HandLeft);
							DrawBone(joints, depthSpacePosition, JointType_HandLeft, JointType_HandTipLeft);
							DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_ThumbLeft);

							// ----------------------------------Right Leg--------------------------------
							DrawBone(joints, depthSpacePosition, JointType_HipRight, JointType_KneeRight);
							DrawBone(joints, depthSpacePosition, JointType_KneeRight, JointType_AnkleRight);
							DrawBone(joints, depthSpacePosition, JointType_AnkleRight, JointType_FootRight);

							// -----------------------------------Left Leg---------------------------------
							DrawBone(joints, depthSpacePosition, JointType_HipLeft, JointType_KneeLeft);
							DrawBone(joints, depthSpacePosition, JointType_KneeLeft, JointType_AnkleLeft);
							DrawBone(joints, depthSpacePosition, JointType_AnkleLeft, JointType_FootLeft);
						}
						delete[] depthSpacePosition;
					}
				}
			}
			cv::imshow("�ؽ�", skeletonImg);
			sprintf_s(image_name, "%s%d%s", "e:\\result\\image", ++frame_num, ".jpg");

			cv::imwrite(image_name, skeletonImg);
			cv::waitKey(5);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//�ͷ�����
		}
	}
	SafeRelease(pBodyFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	return array;
}

/// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	//��¼��������Ƿ�ɹ�
	char image_name[30];
	HRESULT hr;
	//path = "E:\\Data\\right\\right.bmp";

	//����ÿһ��IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];//�洢�ؽڵ���
				HandState leftHandState = HandState_Unknown;//����״̬
				HandState rightHandState = HandState_Unknown;//����״̬

				//��ȡ������״̬
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//�洢�������ϵ�еĹؽڵ�λ��
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//��ùؽڵ���
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//���ؽڵ���������������ϵ ת�� �������ϵ��424*512��
						m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
					}					
					for (int k = 0; k < 25; k++)//�����ݱ��浽vector��
					{
						//Data_X.push_back(depthSpacePosition[k].X);
						//Data_Y.push_back(depthSpacePosition[k].Y);
						Data_X.push_back(joints[k].Position.X);
						Data_Y.push_back(joints[k].Position.Y);
						Data_Z.push_back(joints[k].Position.Z);
						
					}

					//------------------------hand state left-------------------------------
					//DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
					//DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);

					//---------------------------body-------------------------------
					DrawBone(joints, depthSpacePosition, JointType_Head, JointType_Neck);
					DrawBone(joints, depthSpacePosition, JointType_Neck, JointType_SpineShoulder);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_SpineMid);
					DrawBone(joints, depthSpacePosition, JointType_SpineMid, JointType_SpineBase);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderLeft);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipLeft);

					// -----------------------Right Arm ------------------------------------ 
					DrawBone(joints, depthSpacePosition, JointType_ShoulderRight, JointType_ElbowRight);
					DrawBone(joints, depthSpacePosition, JointType_ElbowRight, JointType_WristRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_HandRight);
					DrawBone(joints, depthSpacePosition, JointType_HandRight, JointType_HandTipRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_ThumbRight);

					//----------------------------------- Left Arm--------------------------
					DrawBone(joints, depthSpacePosition, JointType_ShoulderLeft, JointType_ElbowLeft);
					DrawBone(joints, depthSpacePosition, JointType_ElbowLeft, JointType_WristLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_HandLeft);
					DrawBone(joints, depthSpacePosition, JointType_HandLeft, JointType_HandTipLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_ThumbLeft);

					// ----------------------------------Right Leg--------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipRight, JointType_KneeRight);
					DrawBone(joints, depthSpacePosition, JointType_KneeRight, JointType_AnkleRight);
					DrawBone(joints, depthSpacePosition, JointType_AnkleRight, JointType_FootRight);

					// -----------------------------------Left Leg---------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipLeft, JointType_KneeLeft);
					DrawBone(joints, depthSpacePosition, JointType_KneeLeft, JointType_AnkleLeft);
					DrawBone(joints, depthSpacePosition, JointType_AnkleLeft, JointType_FootLeft);
				}
				delete[] depthSpacePosition;
			}
		}
	}
	cv::imshow("�ؽ�", skeletonImg);
	sprintf_s(image_name, "%s%d%s", "e:\\result\\image", ++frame_num, ".jpg");

	cv::imwrite(image_name, skeletonImg);
	cv::waitKey(5);
}

//���ֵ�״̬
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//����ͬ�����Ʒ��䲻ͬ��ɫ
	CvScalar color;
	switch (handState){
	case HandState_Open:
		color = cvScalar(255, 0, 0);
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);
		break;
	default://û��ȷ�������ƣ��Ͳ���
		return;
	}

	circle(skeletonImg,
		cvPoint(depthSpacePosition.X, depthSpacePosition.Y),
		20, color, -1);
}


/// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	CvPoint p1 = cvPoint(depthSpacePosition[joint0].X, depthSpacePosition[joint0].Y),
		p2 = cvPoint(depthSpacePosition[joint1].X, depthSpacePosition[joint1].Y);

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		//�ǳ�ȷ���ĹǼܣ��ð�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(255, 255, 255));
	}
	else
	{
		//��ȷ���ĹǼܣ��ú�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(0, 0, 255));
	}
}


/// Constructor
CBodyBasics::CBodyBasics() :
m_pKinectSensor(NULL),
m_pCoordinateMapper(NULL),
m_pBodyFrameReader(NULL){}

/// Destructor
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}