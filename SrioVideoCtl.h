#pragma once

#ifdef SRIO_EXPORT

#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif


/***********************************************
*�ļ�����SrioVideoCtl.h
*
*�汾�ţ�version 1.00.01
*
*����˵����
*    ���ӿ�����������������ۺ�ͼ��������Խ�
* *����˵����
*      ���ļ������V1.00.01
*      (1)�����ϱ���չ�ӿ�
*      (3)����ָ���·���չ�ӿ�
*      (4)������·״̬�ϱ��ӿ�linkStatusProc
*      (5)ָ���Խӿ�������ӳ�ʱ��ʧ�ܷ���ֵ
*      (6)�޸�ƴ�ӽӿڡ��ںϽӿ�
*
*���Լ����
*      ����
*
*�������ڣ�2019-12-4
*
*���ߣ�����
*************************************************/
#ifndef SRIO_COMMUNICATE_PARSER_H_
#define SRIO_COMMUNICATE_PARSER_H_

#include "Prototyse.h"
#include "Define.h"
class CapBoardListner
{
public:
	/***********************************************
	*��������linkStatusReportProc
	*�������ܣ�SRIO��·״̬�ϱ� ��Ƶ�ź�״̬�ϱ� ����״̬�ϱ�
	*        �ϱ�ʱ����(1)SRIO��·״̬�ϱ� start�� ��Ƶ�źŵ���֮ǰ �����ϱ�SRIO��·״̬   ֮���б仯���ϱ�
	*                (2)��Ƶ�ź�״̬�ϱ�  start�� ��Ƶ�������ͻ���ʾ֮ǰ Ӧ�����ϱ��ź�״̬  ֮���б仯���ϱ�
	*                (3)����״̬�ϱ� �ɷ������ͻ���1s��һ�Σ����������ϱ�
	*
	*������
	*    ���룺
	*         destId Ŀ���豸���
	*         channelID ͨ�����
	*         state
	*               -1: SRIO��·�쳣
	*                1: SRIO��·ͨ��
	*               -2: ����Ƶ�ź�
	*                2������Ƶ�ź�
	*                3������ͻ���1S�ϱ�һ�Σ���������
	*    �������������0 �������ش�����
	*************************************************/
	virtual int linkStatusReportProc(unsigned int destId, unsigned int channelID, int state) = 0;

	/***********************************************
	*��������videoFormatDetectedProc
	*�������ܣ���Ƶ��ʽ�ϱ��ӿ�
	*        �ϱ�ʱ������Ƶ�ɼ���������Ƶ��������/��ʾ֮ǰ�����ϱ���Ƶ��ʽ��Ϣ
	*������
	*    ���룺
	*         destID Ŀ���豸���
	*         channelID ͨ�����
	*         nVideoWidth ��Ƶ��
	*         nVideoHeight ��Ƶ��
	*         frameStoreType YUV���ݴ洢��ʽ
	*                        1 YUV422 YUYV
	*                        2 YUV420 YV12)
	*         duration �����ֶ�
	*         frameRate  ��Ƶ����֡�� ʵʱ��Ƶ�ɲ���ط�ͨ��ʱ���ʵʱͨ���ɲ���
	*
	*    �������������0 �������ش�����
	*************************************************/
	virtual int VideoFormatDetectedProc(unsigned int destID, unsigned int channelID, ULONG nVideoWidth, ULONG nVideoHeight, ULONG frameStoreType,
		ULLONG duration, ULONG frameRate) = 0;

	/***********************************************
	*��������frameReachedProc
	*�������ܣ�֡�����ϱ�
	*������
	*    ���룺
	*         destID Ŀ���豸���
	*         channelID ͨ�����
	*         pFrameBuffer ֡����
	*         nFrameBufferLen���ݳ���
	*    �������������0 �������ش�����
	*************************************************/
	virtual int frameReachedProc(unsigned int destID, unsigned int channelID, char*pFrameBuffer, ULONG nFrameBufferLen) = 0;

	/***********************************************
	*��������stopCapFinishedProc
	*�������ܣ�ֹͣ��ɻص�
	*������
	*    ���룺
	*         destID Ŀ���豸���
	*         channelID ͨ�����
	*    �������������0 �������ش�����
	*************************************************/
	virtual int stopCapFinishedProc(unsigned int destID, unsigned int channelID) = 0;

	/***********************************************
	*��������recvTargetRecognizationResultProc
	*�������ܣ�Ŀ��ʶ�����ϱ�
	*������
	*    ���룺
	*         destId  Ŀ���豸���
	*         channelID ͨ�����
	*         targetNum Ŀ��ʶ������
	*         targetInfo Ŀ����Ϣ
	*    �������������0 �������ش�����
	*************************************************/
	virtual int recvTargetRecognizationResultProc(unsigned int destId, unsigned int channelID, BYTE targetNum, TargetInfo *targetInfo) = 0;

	/***********************************************
	*��������freshTrackStatusProc
	*�������ܣ�Ŀ�����״̬ˢ���ϱ�
	*������
	*    ���룺
	*         destId  Ŀ���豸���
	*         channelID ͨ�����
	*         x,y ����
	*         trackStatus ����״̬
	*                     1 ����
	*                     2 ������
	*                     3 ��ʧ
	*    �������������0 �������ش�����
	*************************************************/
	virtual int freshTrackStatusProc(unsigned int destId, unsigned int channelID, unsigned int x, unsigned int y, unsigned int trackStatus) = 0;

	/***********************************************
	*��������callBackMsg
	*�������ܣ��ϱ���չ�ӿ�
	*         ��Ϣ�ϱ�����չ�ӿڣ�������Ҫʹ�ã���ͼ�����ṩЭ��
	*������
	*    ���룺
	*         destID Ŀ���豸���
	*         channelID ͨ�����
	*         messageType ��Ϣ����
	*         messageLegnth ��Ϣ����
	*         messageData  ��Ϣ����
	*    �������������0 �������ش�����
	*************************************************/
	virtual int callBackMsg(unsigned int destID, unsigned int channelID, unsigned int messageType, unsigned int messageLegnth, char *messageData) = 0;

};


class BoardCapture
{
public:
	/***********************************************
	*��������getLocalDevice
	*�������ܣ���ȡ�����豸��źͶ˿ں�
	*������
	*    ���룺
	*    �����0xXXXXXXXX
	*         ǰ�����ֽڱ����豸���localID���������ֽڱ����豸�˿ں� localPort
	*************************************************/
	virtual unsigned int getLocalDevice() = 0;

	/***********************************************
	*��������getDestDevice
	*�������ܣ���ȡĿ���豸���
	*������
	*    ���룺
	*    �����Ŀ���豸���
	*************************************************/
	virtual unsigned int getDestDevice() = 0;

	/***********************************************
	*��������init
	*�������ܣ���ʼ������
	*������
	*    ���룺
	*    �����  0 �ɹ�
	*          -1 ʧ��
	*          -2 ��ʱ
	*
	*************************************************/
	virtual int init() = 0;
	/***********************************************
	*��������uninit
	*�������ܣ�ȥ��ʼ������������״̬��λ
	*������
	*    ���룺
	*
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int uninit() = 0;

	/***********************************************
	*��������swtichVideoType
	*�������ܣ��л�Ҫ��ʾ����Ƶ��������
	*������
	*    ���룺
	*          videoType ��ʾ����
	*                   0 ԭʼ��Ƶ
	*                   1 ����֮�����Ƶ
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int swtichVideoType(BYTE videoType) = 0;

	/***********************************************
	*��������setOverlapCharacters
	*�������ܣ����õ����ַ���
	*������
	*    ���룺
	*         index ���������� 1-4
	*         bOn  ���أ�
	*              true ��
	*              false ��
	*         x��y  ���Ͻ���ʼ������ ��λ����
	*         characters �ַ���
	*         length �ַ�������
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int setOverlapCharacters(ULONG index, bool bOn, ULONG x, ULONG y, char* characters, ULONG length) = 0;

	/***********************************************
	*��������setImageEnhancementSwitch
	*�������ܣ�����ͼ����ǿ����
	*������
	*    ���룺
	*         on ���أ�
	*            true ��
	*            false ��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int setImageEnhancementSwitch(bool on) = 0;

	/***********************************************
	*��������start
	*�������ܣ��������У���ʼ�ɼ���ʾ��Ƶ
	*������
	*    ���룺
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int start() = 0;

	/***********************************************
	*��������stop
	*�������ܣ�ֹͣ�ɼ�����Ƶ��������
	*������
	*    ���룺
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int stop() = 0;

	/***********************************************
	*��������clone
	*�������ܣ�ʵ�ֶര����ʾͬһͨ����Ƶ
	*������
	*    ���룺
	*         winId ����ʾ�Ĵ���ID ��ʾ��֧�ֿ�ƽ̨(linux��windows)
	*         cloneId ��¡��� [1-3] ��¡�������֧��3��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*
	*************************************************/
	virtual int clone(HWND winId, BYTE cloneId) = 0;

	/***********************************************
	*��������unClone
	*�������ܣ�ȡ��ͨ���Ķര����ʾ
	*������
	*    ���룺
	*         cloneId ��¡ID [1-3]
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int unClone(BYTE cloneId) = 0;

	/***********************************************
	*��������setMagnification
	*�������ܣ����õ��ӷŴ���
	*������
	*    ���룺
	*         magnification �Ŵ�����
	*                       0 x1
	*                       1 x2;
	*                       2 x4
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setMagnification(BYTE magnification) = 0;

	/***********************************************
	*��������setFogProcSwitch
	*�������ܣ����õ���͸����
	*������
	*    ���룺
	*         on ���أ�
	*            true ��
	*            false ��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setFogProcSwitch(bool on) = 0;

	/***********************************************
	*��������setViedoStabilizationSwitch
	*�������ܣ����õ������񿪹�
	*������
	*    ���룺
	*         on ���أ�
	*           true ��
	*           false ��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setViedoStabilizationSwitch(bool on) = 0;

	/***********************************************
	*��������setImageStitchingSwitch
	*�������ܣ�����ͼ��ƴ�ӿ��� ֻ��ͨ��1��Ӧ
	*������
	*    ���룺
	*         on ���أ�
	*           true ��
	*           false ��
	* 		  isc 0 ���ӳ�
	*             1 ���ӳ�
	*             2 С�ӳ�
	*         ext ����
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setImageStitchingSwitch(bool on, int isc, unsigned int ext) = 0;

	/***********************************************
	*��������setTargetDetectSwitch
	*�������ܣ�����Ŀ���⿪��
	*������
	*    ���룺
	*        on ���أ�
	*          true ��
	*          false ��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setTargetDetectSwitch(bool on) = 0;

	/***********************************************
	*��������setTargetRecognizeSwitch
	*�������ܣ�����Ŀ��ʶ�𿪹�
	*������
	*    ���룺
	*         on ���أ�
	*           true ��
	*           false ��
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setTargetRecognizeSwitch(bool on) = 0;

	/***********************************************
	*��������setTargetTrackMode
	*�������ܣ�����Ŀ�����ģʽ
	*������
	*    ���룺
	*         mode ����ģʽ��0�������  1�Զ�����
	*               0 ������x,y
	*               1 ��Ŀ����
	*          on ���ٿ��� true ���� false �ر�
	*          x,y Ŀ�����ͼ������ʼ�����Ͻǵ���������
	*          targetID Ŀ����[0-9]
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setTargetTrackMode(BYTE mode, bool on, unsigned int x, unsigned int y, unsigned int targetID) = 0;

	/***********************************************
	*��������setImageFusionSwitch
	*�������ܣ�����ͼ���ںϿ���
	*������
	*    ���룺
	*         on ���أ�
	*           true ��
	*           false ��
	*         KJGFocalDistance �ɼ���ͨ������ ��λ��mm
	*         HWFocalDistance  ����ͨ������  ��λ: mm
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setImageFusionSwitch(bool on, float KJGFocalDistance, float HWFocalDistance) = 0;

	/***********************************************
	*��������setRecordSettings
	*�������ܣ�¼������
	*������
	*    ���룺
	*         videoType
	*                  0 ������Ƶ
	*                  1 ԭʼ��Ƶ
	*         MByteRate ���� ��λMbps
	*         timeDuration ʱ�� ��λ����
	*         WScale  �洢��Ƶ��width ��λ����
	*         HScale  �洢��Ƶ��height ��λ����
	*         codecType: 0 H264  1 H265 Ĭ��H264
	*		  fileType : �ļ���ʽ
	*                           0 Ts
	*                           1 ����
	*                           2 MP4
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setRecordSettings(BYTE videoType, ULLONG MBitRate, ULONG timeDuration, int WScale, int HScale, int codecType = 0, int fileType = 0) = 0;


	/***********************************************
	*��������startRecord
	*�������ܣ���ʼ¼��
	*               ��ʽ֧�� ���� ts mp4(��֧��seek)
	*������
	*    ���룺
	*          path   �洢·�� �����ļ����ļ���[150�ֽ�] ["·��"+"/"+"�ļ�����������׺��"]
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int startRecord(char* path) = 0;

	/***********************************************
	*��������stopRecord
	*�������ܣ�ֹͣ��¼
	*������
	*    ���룺
	*
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int stopRecord() = 0;

	/***********************************************
	*��������snapshot
	*�������ܣ���ͼ��֧��.JPG .BMP .JP2 .PNG�ȳ��ø�ʽ���Ժ�׺������
	*������
	*    ���룺
	*         path    �洢·�� [��󳤶ȴ���]
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	************************************************/
	virtual int snapshot(char* path) = 0;

	/***********************************************
	*��������setCross
	*�������ܣ�����ʮ��˿
	*������
	*    ���룺
	*         on  ���أ�
	*            true ��
	*            false ��
	*         centerX ʮ��˿����ˮƽ����
	*         centerY ʮ��˿���Ĵ�ֱ����
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int setCross(bool on, unsigned int centerX, unsigned int centerY) = 0;

	/***********************************************
	*��������selectEth
	*�������ܣ��������̫������
	*������
	*    ���룺
	*         srcIp  ��ƵԴIP
	*         srcPort ��ƵԴPort
	*         destIp Ŀ��IP
	*         destPort:Ŀ�Ķ˿ں�
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int selectEth(unsigned int srcIp, unsigned int srcPort, unsigned int destIp, unsigned int destPort) = 0;

	/***********************************************
	*��������extInterface
	*�������ܣ���չָ���·���������ýӿ�
	*������
	*    ���룺
	*         extPara  ��չָ������
	*    �����
	*         0 �ɹ�
	*        -1 ʧ��
	*        -2 ��ʱ
	*************************************************/
	virtual int extInterface(ExtStruct extPara) = 0;
	virtual ~BoardCapture() {}
};

extern "C"
{
	/***********************************************
	*��������createBoardCapture
	*�������ܣ�����BoardCapture����
	*������
	*    ���룺
	*          localId  �����豸���
	*          localPort ���ض˿ں�
	*          destId Ŀ���豸���
	*          channelID ͨ����ţ�0�ɼ��⣬1 ����, 4 ���˻�
	*          listner ��Ϣ������
	*          videoType ������չ
	*          winId    ��ʾ����ID ��ʾ��֧�ֿ�ƽ̨(linux,windows)
	*    �����
	*           BoardCapture����
	*           ʧ�ܷ��� nullptr
	*************************************************/
	EXPORT_API BoardCapture *createBoardCapture(int localId, unsigned int localPort, unsigned int destId, unsigned int channelID, unsigned int videoType, CapBoardListner *listner, PVOID winId);

	/***********************************************
	*��������destroyBoardCapture
	*�������ܣ�����BoardCapture����
	*������
	*    ���룺
	*        �����ͼ��ɼ���
	*    �����
	*        0 �ɹ�
	*        -1 ʧ��
	*************************************************/
	EXPORT_API int destroyBoardCapture(BoardCapture *boardCap);
}
#endif /* SRIO_COMMUNICATE_PARSER_H_ */
