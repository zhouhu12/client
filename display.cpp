#include "display.h"
#include <unordered_map>
#include <mutex>
#include "videoEncoderManager.h"
#include "srio.h"
#include "draw.h"
#include "size.h"
#include "spdlog/spdlog.h"
#include "SrioVideoCtl.h"


extern CapBoardListner* pCapBoardListner;


#define DEBUG(x, id) spdlog::get("srio")->debug("[{}] {} channelId={}", "display", x, id);
#define INFO(x, id) spdlog::get("srio")->info("[{}] {} channelId={}", "display", x, id);
#define SPDERROR(x, id) spdlog::get("srio")->error("[{}] {} channelId={}", "display", x, id);


display::display(int channelId, HWND winId)
	: channelId(channelId), winId(winId)
{

	buffer.resize(1920 * 1080 * 4);
	srioBuffer[0].resize(1920 * 1080 * 4);
	srioBuffer[1].resize(1920 * 1080 * 4);

	recordOn = false;
	displayOn = false;
	quitDisplay = true;
	//quitSnapshot = true;

	//if (channelId == 0 || channelId == 4) {
	//	imgSize = cv::Size(1920, 1080);
	//}
	//else if (channelId == 1) {
	//	imgSize640 = cv::Size(640, 512);
	//}
	imgSize = cv::Size(1920, 1080);
	imgSize910 = cv::Size(910, 512);
	imgSize1440 = cv::Size(1440, 810);

	img640 = cv::Mat(cv::Size(640, 512), CV_8UC1);
	if (channelId == 1)
		img = cv::Mat(imgSize910, CV_8UC1, buffer.data());
	else 
		img = cv::Mat(imgSize, CV_8UC3, buffer.data());
		//img = cv::Mat(imgSize1440, CV_8UC3, buffer.data() + 16);

}

display::~display()
{
	stop();
}

int display::start()
{
	displayOn = true;
	quitDisplay = false;
	displayThread = std::thread(&display::showPix, this, channelId);

	//quitSnapshot = false;
	//snapshotThread = std::thread(&display::snapshotLoop, this);

	return 0;
}

int display::stop()
{
	quitDisplay = true;
	if (recordOn)
	{
		recordOn = false;
		videoEncoderManager.stopRecord();
	}
	if (displayThread.joinable())
		displayThread.join();

	//quitSnapshot = true;
	//snapshotCv.notify_all();
	//if (snapshotThread.joinable()) {
	//	snapshotThread.join();
	//}

	return 0;
}

void display::showPix(int channelId)
{
	if (channelId != 0 && channelId != 1 && channelId != 4) {
		SPDERROR(fmt::format("showPix is called with invalid channelId {}", channelId), channelId);
		return;
	}
	int cap = channel2Srio.at(channelId);

	auto &srio = SRIO::getSrio();
	//int tmpBufSize = 1920 * 1080 * 3;
	int tmpBufSize = SIZE_HEIGHT * SIZE_WIDTH * 3;;
	unsigned char* tmpBuf = new unsigned char[tmpBufSize];

	while (!quitDisplay) {
		//static auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//static int frameCnt = 0;
		//static long long pre = 0;
		//std::cout<<"interval:"<< start - pre<<std::endl;
		//pre = start;
		if (channelId == 1) {
			
			auto sz = srio.acceptImage(cap, &mImgPack, MAXORISIZE) - 4;
			if (mImgPack.type == 0) {
				if (img.channels() != 1 && img.cols != 910) {
					img = cv::Mat(imgSize910, CV_8UC1, buffer.data());
					mInfType = InfType::COMMON;
				}
				memcpy(img640.data, mImgPack.imgDate, 640*512);
				cv::copyMakeBorder(img640, img, 0, 0, 135, 135, cv::BORDER_CONSTANT, cv::Scalar(0));
				cv::resize(img, img, cv::Size(1920, 1080));
			}
			else if (mImgPack.type == 1) {
				if (img.channels() != 3) {
					img = cv::Mat(imgSize, CV_8UC3, buffer.data());
					mInfType = InfType::STITCH;
				}
				memcpy(img.data, mImgPack.imgDate, sz);
				cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
				//DEBUG(fmt::format("horiAngle:{}", mImgPack.horiAngle), channelId);
				if (mImgPack.horiAngle != -1) {
					mTargetInfo.guanDaoAttiangle.orientation = mImgPack.horiAngle;
					mTargetInfo.guanDaoAttiangle.picht = mImgPack.vertAngle;
					INFO(fmt::format("Tracking Point: hori_angle{}, vert_angle{}", mImgPack.horiAngle, mImgPack.vertAngle), channelId);
					pCapBoardListner->recvTargetRecognizationResultProc(0, 1, 0, &mTargetInfo);
				}
			}else
				SPDERROR(fmt::format("mImgPack.type error {}", channelId), channelId);
				
		}
		else {
			//auto offset = srio.acceptImage(cap, buffer.data(), MAXORISIZE) - 16;

			//auto offset = srio.acceptImage(cap, srioBuffer[0].data(), MAXORISIZE);
			//auto rem = srio.acceptImage(cap, srioBuffer[1].data(), MAXORISIZE);

			int r1 = srio.acceptImage(cap, tmpBuf, tmpBufSize);
			//int r2 = srio.acceptImage(cap, tmpBuf + r1, tmpBufSize - r1);


			//

			//cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
			{
				std::lock_guard<std::mutex> lck(imgMtx);
				//memcpy(buffer.data(), srioBuffer[0].data(), offset);
				//memcpy(buffer.data() + offset, srioBuffer[1].data(), rem);

				//img = cv::Mat(1080, 1920, CV_8UC3, tmpBuf);
				img = cv::Mat(SIZE_HEIGHT, SIZE_WIDTH, CV_8UC3, tmpBuf);
				cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

				//auto yuv = cv::Mat(1080, 1920, CV_8UC3,tmpBuf);
				////cv::Mat rgb;
				////cv::cvtColor(yuv, rgb, cv::COLOR_YUV2RGB_YUYV);
				//cv::imshow("1", img);
				//cv::waitKey(1);
			}
			//frameCnt++;
			//if (frameCnt == 500) {
			//	auto stop = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			//	double fps = 500 * 1000.0 / (stop - start);
			//	std::cout << "FRAMES PER SECOND: " << fps << '\n';
			//	INFO(fmt::format("FRAMES PER SECOND: {}", fps), channelId);
			//	frameCnt = 0;
			//	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			//}
		}

		//displayOn = false;
		//auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		if (recordOn)
		{
			videoEncoderManager.push(img.data);
		}
		if (displayOn) {
			DrawCVImage(img, winId);
			for (auto &p : clonedWin) {
				DrawCVImage(img, p.second);
			}
		}
		//auto t2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//spdlog::info("delay:{}", t2 - t1);
	}
	delete tmpBuf;
}

//void display::snapshotLoop()
//{
//	std::unique_lock<std::mutex> lck(snapshotMtx);
//	while (!quitSnapshot) {
//		snapshotCv.wait(lck);
//
//		cv::imwrite(filename, copy);
//	}
//}

int display::clone(HWND winId, BYTE channelId)
{
	INFO(fmt::format("clone channel {}", channelId), channelId);

	if (clonedWin.count(channelId)) {
		SPDERROR(fmt::format("cannot clone duplicated channel {}", channelId), channelId);
		return -1;
	}
	clonedWin[channelId] = winId;
}

int display::unClone(BYTE channelId)
{
	INFO(fmt::format("unClone channel {}", channelId), channelId);
	if (!clonedWin.count(channelId))
	{
		SPDERROR(fmt::format("cannot unclone unexisting channel {}", channelId), channelId);
		return -1;
	}
	clonedWin.erase(channelId);
}


int display::extInterface(ExtStruct extPara)
{
	auto tp = channel2Source.at(channelId);
	auto p = extPara.para;
	switch (extPara.type)
	{
	//	// Mode switching
	//case MODE_RAW_VIDEO:
	//	INFO("extInterface MODE_RAW_VIDEO", channelId);
	//	break;
	//case MODE_PANORAMA_STCHING_VIDEO:
	//	INFO("extInterface MODE_PANORAMA_STCHING_VIDEO", channelId);
	//	if (p.recordSetting.sourceType != tp)
	//	{
	//		SPDERROR(fmt::format("extInterface sourceType mismatch! from para: {}, should be {}", p.recordSetting.sourceType, tp), channelId);
	//		return -1;
	//	}

	//	break;
	//case MODE_SECTOR_SCAN_STCHING_VIDEO:
	//	INFO("extInterface MODE_SECTOR_SCAN_STCHING_VIDEO", channelId);
	//	break;
	//case MODE_DUAL_SOURCE_FUSION_VIDEO:
	//	INFO("extInterface MODE_DUAL_SOURCE_FUSION_VIDEO", channelId);
	//	break;

		// Record start / stop
	case RECORD_START:
		INFO("extInterface RECORD_START", channelId);
		if (p.recordSetting.sourceType != tp)
		{
			SPDERROR(fmt::format("extInterface sourceType mismatch! from para: {}, should be {}", p.recordSetting.sourceType, tp), channelId);
			return -1;
		}
		startRecord(p.recordSetting);
		break;
	case RECORD_STOP:
		INFO("extInterface RECORD_STOP", channelId);
		if (p.recordSetting.sourceType != tp)
		{
			SPDERROR(fmt::format("extInterface sourceType mismatch! from para: {}, should be {}", p.recordSetting.sourceType, tp), channelId);
			return -1;
		}
		stopRecord();
		break;
	default:
		//if(extPara.type == MODE_PANORAMA_STCHING_VIDEO)

		//SPDERROR(fmt::format("invalid type enum: {}", extPara.type), channelId);
		INFO(fmt::format("extInterface Sending para with type {} directly to server...", extPara.type), channelId);
		
		auto ret = send2Server(extPara);
		if (ret < 0) {
			SPDERROR(fmt::format("extInterface Failed to send para with type {}", extPara.type), channelId);
		}
		break;
	}
}

int display::snapshot(char * path)
{
	INFO(fmt::format("snapshot to {}", std::string(path)), channelId);
	static cv::Mat copy;
	{
		std::lock_guard<std::mutex> lck(imgMtx);
		if(img.cols == 1920)
			img.copyTo(copy);
		else
			cv::resize(img, copy, cv::Size(1920, 1080));
	}
	cv::imwrite(path, copy);
	return 0;
}

int display::send2Server(ExtStruct s)
{
	auto &srio = SRIO::getSrio();
	//return srio.writeCommand(&s, sizeof(ExtStruct), false);
	return srio.writeCommand(&s, sizeof(ExtStruct));
}

void display::startRecord(RecordPara p)
{
	int rawWid = SIZE_WIDTH, rawHei = SIZE_HEIGHT;
	info.time = p.timeDuration * 60;
	info.path = p.path;
	info.FPS = p.encodePara.fRate;
	info.rate = p.encodePara.MBitRate / 1024 / 1024;
	info.targetWidth = p.encodePara.moiveScale.wScale;
	info.targetHeight = p.encodePara.moiveScale.hScale;
	if (channelId == 1) {
		if (mInfType == InfType::COMMON) {
			rawWid = 1920;
			rawHei = 1080;
			info.picType = "GRAY";
		}
		else {
			rawWid = 1920;
			rawHei = 1080;
			info.picType = "BGR";
		}
	}
	videoEncoderManager.paraSet(rawWid, rawHei, info.targetWidth, info.targetHeight, info.FPS, info.time, info.rate, info.picType, info.path);
	videoEncoderManager.startRecord();
	recordOn = true;
}

void display::stopRecord()
{
	recordOn = false;
	videoEncoderManager.stopRecord();
}
