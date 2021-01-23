#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <windows.h>
#include "prototyse.h"
#include "videoEncoderManager.h"
#include "opencv2/opencv.hpp"
#include "Define.h"

struct RecordInfo {
	int videoWidth = 1920;
	int videoHeight = 1080;
	int targetWidth = 1920;
	int targetHeight = 1080;
	int FPS = 30;
	int time = 30;
	int rate = 8;
	std::string picType = "BGR";
	std::string path = "D:/pano";
};

struct ImgPackage {
	int type;//common 0, stitch 1
	uchar imgDate[1920 * 1080 * 3];
	float horiAngle = -1;
	float vertAngle = -1;
};



class display
{
public:
	display(int channelId, HWND winId);

	display::~display();

	int start();

	int stop();

	int clone(HWND winId, BYTE channelId);

	int unClone(BYTE channelId);

	int extInterface(ExtStruct extPara);

	int snapshot(char *path);

	float mAngle = -1;

private:

	TargetInfo mTargetInfo;

	int send2Server(ExtStruct s);

	void startRecord(RecordPara p);

	void stopRecord();

	videoEncoderManager videoEncoderManager;

	std::string name;

	int channelId;

	HWND winId;

	std::unordered_map<int, HWND> clonedWin;

	void display::showPix(int category);

	RecordInfo info;

	std::vector<unsigned char> buffer;
	std::vector<unsigned char> srioBuffer[2];

	cv::Mat img, img640;
	cv::Mat copy;
	std::string filename;
	std::mutex imgMtx;

	const int MAXORISIZE = 1024 * 1024 * 8;

	std::atomic_bool recordOn;

	std::atomic_bool displayOn;

	std::thread displayThread;
	std::atomic_bool quitDisplay;

	const std::unordered_map<int, int> channel2Srio = {
		{ 0, 1 },{ 1, 2 },{ 4, 3 }
	};

	const std::unordered_map<int, SourcePara> channel2Source = {
		{0, VISIBLE}, {1, INFRARED}, {4, UAV}
	};

	cv::Size imgSize, imgSize640, imgSize910, imgSize1440;

	ImgPackage mImgPack;

	enum class InfType {COMMON, STITCH};

	InfType mInfType = InfType::COMMON;
	//std::atomic_bool quitSnapshot;
	//std::thread snapshotThread;
	//void snapshotLoop();
	//std::condition_variable snapshotCv;
	//std::mutex snapshotMtx;

};

#endif // DISPLAY_H
