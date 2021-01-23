#pragma once

#ifndef INT64_C 

#define INT64_C(c) (c ## LL) 

#define UINT64_C(c) (c ## ULL) 

#endif 



#ifdef __cplusplus 

extern "C"

{

#endif 

	/*Include ffmpeg header file*/
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 
#include <libavutil/imgutils.h>  
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>

#include <windows.h>

#ifdef __cplusplus 

}

#endif 

#include<iostream>
#include<string>
#include<time.h>
using namespace std;

class ffmpegEncoder
{
public:
	ffmpegEncoder();
	~ffmpegEncoder();

	int openEncoder(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara,int bitratePara,std::string _frameType, std::string _savePath,int number);
	/*
	�ɹ�����0�����󷵻ش����
	1����ʼ��AVCodecContext�������ݽṹʧ��
	2����ʼ��AVPacketʧ��
	3���򿪱�����ʧ��
	4�����ñ���֡ʧ��
	5���������֡�ռ�ʧ�ܡ�
	6�����ļ�ʱ��
	7��д�ļ�ͷʧ��
	*/
	int EncoderPerFrame(uint8_t* data);
	//��ȷ����0��
	//1��Frame���ɶ�
	//2��Frame���ɱ���
	//3��Frame�������
	//4�������֡ˢ��ʱ����
	int EncodeEndFrame();
	int closeEncoder();
	int openAnotherFile(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int bitratePara, std::string _savePath);


private:

	//���������
	AVCodec* codec;//������
	AVCodecContext* c = NULL;//�������ݽṹ����
	AVFrame* frame;//֡
	AVPacket* pkt;//���ݰ�
	AVPacket* outPkt;//���ݰ�
	AVFormatContext* ofmt_ctx; //������ݽṹ
	int ret;//����ֵ
	struct SwsContext* sws_ctx;//��ʽת����
	enum AVPixelFormat src_pix_fmt;//��ʽ����s
	enum AVPixelFormat dst_pix_fmt;//��ʽ����d

    bool encoderState;//�������Ƿ�����
	int pts = 0;//ʱ�������
	int savepts = 0;

	int count = 0;

	int srcCols;
	int srcRows;
	int dstCols;
	int dstRows;
	int FPS;
	int srcColsSwsScale;//����������ڳߴ�ת��ʱ��buffer sizeȷ�ϣ������RGB����ΪsrcColsPara*3.
	int bitrate;

	double saveStartFrameCount;
};
