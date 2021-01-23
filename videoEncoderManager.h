#pragma once
#include<string>
#include<vector>
#include<queue>
#include <ctime>

#include <sys/stat.h>
#include <sys/types.h>

#include<iostream>
#include<thread>
#include<mutex>

#include<opencv2/opencv.hpp>

#include <windows.h>
#include<direct.h>
#include<io.h>

#include"videoEncoder.h"

using namespace std;

class  videoEncoderManager
//class videoEncoderManager
{
public:
	videoEncoderManager();
	~videoEncoderManager();

	mutex *getEncoderMutex();

    /*
    �ɹ�����0�����󷵻ش����
    1��δ����������
    2���򿪱�����ʧ��
    3�������֡ʧ��
    */
    int push(unsigned char *buf);
	int paraSet(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int segmentationTimePara, int bitratye,std::string _frameType, std::string _savePath);
	int startRecord();
	int stopRecord();

private:
	int deleteEarlyVideo();
	void getFiles(std::string path, std::string exd, std::vector<std::string>& files);
    bool createSavePath(std::string savePath);//����������Ƶ���õ��ļ���

	std::queue<cv::Mat> compressQueue;
	mutex encoderQueueLock;


	bool ifstart;
	int srcCols;
	int srcRows;
	int dstCols;
	int dstRows;
	int srcColsEven;
	int srcRowsEven;
	int dstColsEven;
	int dstRowsEven;
	int FPS;
	int segmentationTime;//�ֶ�ʱ��/s
	int bitrate;
	std::string savePath;//����MP4�ļ���Ŀ¼��
	char videoSaveName[200];//����MP4�ļ�ȫ��

	bool srcIsGRAY;//�����ж�����ͼ������͡�
	bool flagRunEncoder = false;
	int encoderCount;
    bool isEncoderBeenOpen = false;//�������д�������һ�ű������ͼ��

    mutex mutex_stop_push;
	std::thread th;

	ffmpegEncoder Encoder1;

	//¼�����
	int num=0;
};

