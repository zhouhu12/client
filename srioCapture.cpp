#include "srioCaptrue.h"
#include <csignal>

SRIOCapture::SRIOCapture(): m_hDev(INVALID_HANDLE_VALUE), m_channelId(0xff), m_buffer(NULL), m_bufferSize(0), m_dbThreadRunning(false) {
}

SRIOCapture::SRIOCapture(HANDLE device, DWORD channel, DWORD bufSize) : m_hDev(device), m_channelId(channel), m_buffer(NULL), m_bufferSize(bufSize), m_dbThreadRunning(false) {
	try {
		m_buffer = (PVOID)malloc(m_bufferSize);
	}
	catch (...) {
		printf_s("ERROR(SRIOCapture) malloc error\n");
		raise(SIGTERM);
	}
	m_dbThreadRunning = true;
	m_dbThread = std::thread(&SRIOCapture::_dbThread, this);
	if (!m_dbThread.joinable()) {
		printf_s("ERROR(SRIOCapture) start doorbell failed\n");
		raise(SIGTERM);
	}
}

SRIOCapture::~SRIOCapture() {
	if (m_dbThread.joinable()) {
		m_dbThread.join();
	}
	m_dbThreadRunning = false;
	if (NULL != m_buffer) {
		free(m_buffer);
	}
}

DWORD SRIOCapture::channel() {
	return m_channelId;
}

void SRIOCapture::updateDoorbell(SRIODoorbell doorbell) {
	SRIODBInfo dbInfo = getDoorbellInfo(doorbell._info);
	dbInfo._srcId = doorbell._srcId;
	dbInfo._dstId = doorbell._dstId;
	if (dbInfo._channel != m_channelId) {
		return;
	}
	{
		std::unique_lock<std::mutex> lk(m_dbMutex);
		m_dbInfo = dbInfo;
	}
	m_dbCV.notify_one();
}

DWORD SRIOCapture::getData(PVOID buf, UINT32 bufSize) {
	DWORD iRetSize = 0;
	{
		std::unique_lock<std::mutex> lk(m_bufferMutex);
		m_bufferCV.wait(lk);
		PVOID tmpBuf = m_buffer;
		SRIOData* ibData = NULL;
		uint32_t  ibSize = 0;
		if (*(uint32_t*)tmpBuf != 0xffeebbaa) {
			ibData = (SRIOData*)((unsigned char*)tmpBuf + 236);
			ibSize = ibData->_dataSize;
		}
		else {
			ibData = (SRIOData*)((unsigned char*)tmpBuf);
			ibSize = ibData->_dataSize;
		}
		unsigned char* srcAddr = (unsigned char*)ibData + 20;
		if (bufSize < ibSize) {
			memcpy((unsigned char*)buf, srcAddr, bufSize);
			iRetSize = bufSize;
			printf("WARNING: The size of buffer you provided is not adequate for inbound data.");
			printf(" Buffer is truncated to suit yours\n");
		}
		else {
			memcpy((unsigned char*)buf, srcAddr, ibSize);
			iRetSize = ibSize;
		}
	}
	return iRetSize;
}

void SRIOCapture::_dbThread() {
	DWORD dwRetSize = 0;
	while (m_dbThreadRunning) {
		{
			std::unique_lock<std::mutex> lk(m_dbMutex);
			m_dbCV.wait(lk);
			{
				std::unique_lock<std::mutex> lk2(m_bufferMutex);
				_srioRead(m_dbInfo._r2pWinNum, m_dbInfo._r2pWinOffset, m_buffer, m_bufferSize);
				switch (m_dbInfo._r2pWinNum) {
				case 1:
				{
					if (*(PUINT32)m_buffer != 0xffeebbaa) {
						PUCHAR head = (PUCHAR)m_buffer + 236;
						PUINT32 ptr = (PUINT32)head;
						*ptr = 0xffeebbaa; ptr++;
						*ptr = 1; ptr++;
						*ptr = 0; ptr++;
						*ptr = 1920 * 1080 * 2; ptr++;
						*ptr = 1920 * 1080 * 2 + 20; ptr++;
					}
					//if (*(UINT32*)m_buffer != 0xffeebbaa) {
					//	SRIOData srioData(1, 0, (PUCHAR)m_buffer + 256, 1920 * 1080 * 2);
					//	//memset(m_buffer, 0, m_bufferSize);
					//	memcpy(m_buffer, srioData._buf, srioData._totalSize);
					//}
				}
				break;
				case 2:
				{
					if (*(PUINT32)m_buffer != 0xffeebbaa) {
						PUCHAR head = (PUCHAR)m_buffer + 236;
						PUINT32 ptr = (PUINT32)head;
						*ptr = 0xffeebbaa; ptr++;
						*ptr = 2; ptr++;
						*ptr = 0; ptr++;
						*ptr = 1280 * 720 * 2; ptr++;
						*ptr = 1280 * 720 * 2 + 20; ptr++;
					}
					//if (*(UINT32*)m_buffer != 0xffeebbaa) {
					//	SRIOData srioData(2, 0, (PUCHAR)m_buffer + 256, 1280 * 720 * 2);
					//	//memset(m_buffer, 0, m_bufferSize);
					//	memcpy(m_buffer, srioData._buf, srioData._totalSize);
					//}
				}
				break;
				case 3:
				{
					if (*(PUINT32)m_buffer != 0xffeebbaa) {
						PUCHAR head = (PUCHAR)m_buffer + 236;
						PUINT32 ptr = (PUINT32)head;
						*ptr = 0xffeebbaa; ptr++;
						*ptr = 3; ptr++;
						*ptr = 0; ptr++;
						*ptr = 1920 * 1080 * 2; ptr++;
						*ptr = 1920 * 1080 * 2 + 20; ptr++;
					}
					//if (*(UINT32*)m_buffer != 0xffeebbaa) {
					//	SRIOData srioData(3, 0, (PUCHAR)m_buffer + 256, 1920 * 1080 * 2);
					//	//memset(m_buffer, 0, m_bufferSize);
					//	memcpy(m_buffer, srioData._buf, srioData._totalSize);
					//}
				}
				break;
				default:
					break;
				}
			}
			m_bufferCV.notify_one();
		}
	}
}

DWORD SRIOCapture::_srioRead(DWORD r2pWinNum, DWORD offset, PVOID buf, DWORD bufSize) {
	DWORD dwErr;
	DWORD dwBufSize;

	//TODO: bufSize should not be larger than 1920 * 1080 * 2 + 256
	dwBufSize = bufSize;
	dwErr = TSI721IbwBufferGet(m_hDev, r2pWinNum, offset, buf, &dwBufSize);
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) IbwBufferGet error, code: 0x%x\n", dwErr);
		return -1;
	}
	return 0;
}

SRIODBInfo getDoorbellInfo(DWORD doorbell) {
	SRIODBInfo dbInfo;
	memset(&dbInfo, 0, sizeof(SRIODBInfo));
	dbInfo._doorbell = doorbell;
	switch (doorbell) {
	case SRIOCOM_COMMAND_DOORBELL_1:
	case SRIOCOM_COMMAND_DOORBELL_2:
		dbInfo._dstAddr = (doorbell == SRIOCOM_COMMAND_DOORBELL_1)
			? SRIOCOM_COMMAND_ADDRESS_1 : SRIOCOM_COMMAND_ADDRESS_2;
		dbInfo._r2pWinOffset = (doorbell == SRIOCOM_COMMAND_DOORBELL_1)
			? SRIOCOM_COMMAND_ADDRESS_1 : SRIOCOM_COMMAND_ADDRESS_2;
		dbInfo._channel = 0;
		dbInfo._r2pWinNum = 0;
		break;
	case CARDCAPTURE_CONTROL_DOORBELL:
		dbInfo._dstAddr = CARDCAPTURE_CONTROL_ADDRESS;
		dbInfo._r2pWinOffset = CARDCAPTURE_CONTROL_ADDRESS;
		break;
	case CARDCAPTURE_VISION_DOORBELL_1:/*break;*/
	case CARDCAPTURE_VISION_DOORBELL_2:
		dbInfo._dstAddr = (doorbell == CARDCAPTURE_VISION_DOORBELL_1)
			? CARDCAPTURE_VISION_ADDRESS_1 : CARDCAPTURE_VISION_ADDRESS_2;
		dbInfo._r2pWinOffset = (doorbell == CARDCAPTURE_VISION_DOORBELL_1) ? 0 : SRIO_DMA_BUF_SIZE_HALF;
		dbInfo._channel = 1;
		dbInfo._r2pWinNum = 1;
		break;
	case CARDCAPTURE_INFRARE_DOORBELL_1:
	case CARDCAPTURE_INFRARE_DOORBELL_2:
		dbInfo._dstAddr = (doorbell == CARDCAPTURE_INFRARE_DOORBELL_1)
			? CARDCAPTURE_INFRARE_ADDRESS_1 : CARDCAPTURE_INFRARE_ADDRESS_2;
		dbInfo._r2pWinOffset = (doorbell == CARDCAPTURE_INFRARE_DOORBELL_1) ? 0 : SRIO_DMA_BUF_SIZE_HALF;
		dbInfo._channel = 2;
		dbInfo._r2pWinNum = 2;
		break;
	case CARDCAPTURE_DRONE_DOORBELL_1:
	case CARDCAPTURE_DRONE_DOORBELL_2:
		dbInfo._dstAddr = (doorbell == CARDCAPTURE_DRONE_DOORBELL_1)
			? CARDCAPTURE_DRONE_ADDRESS_1 : CARDCAPTURE_DRONE_ADDRESS_2;
		dbInfo._r2pWinOffset = (doorbell == CARDCAPTURE_DRONE_DOORBELL_1) ? 0 : SRIO_DMA_BUF_SIZE_HALF;
		dbInfo._channel = 3;
		dbInfo._r2pWinNum = 3;
		break;
	default:
		break;
	}
	return dbInfo;
}