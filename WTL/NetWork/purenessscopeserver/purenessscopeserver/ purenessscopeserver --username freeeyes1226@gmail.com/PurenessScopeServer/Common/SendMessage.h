#ifndef _SENDMESSAGE_H
#define _SENDMESSAGE_H

#include "define.h"
#include "IBuffPacket.h"
#include <map>

using namespace std;

//定义一个发送数据容器，用于异步发送队列
struct _SendMessage
{
	uint32          m_u4ConnectID;    //要发送的远程ID
	uint16          m_u2CommandID;    //要发送的命令ID，用于统计功能
	bool            m_blSendState;    //要发送的状态，0是立即发送，1是先缓存不发送
	IBuffPacket*    m_pBuffPacket;    //数据包内容
	uint8           m_nEvents;        //发送类型，0：正常数据包发送，1：发送阻塞数据
	ACE_hrtime_t    m_tvSend;         //数据包发送的时间戳

	_SendMessage()
	{
		m_u4ConnectID = 0;
		m_nEvents     = 0;
		m_u2CommandID = 0;
		m_blSendState = 0;
	}

	~_SendMessage()
	{
	}
};

class CSendMessagePool
{
public:
	CSendMessagePool(void);
	~CSendMessagePool(void);

	void Init(int nObjcetCount = MAX_MSG_THREADQUEUE);
	void Close();

	_SendMessage* Create();
	bool Delete(_SendMessage* pObject);

	int GetUsedCount();
	int GetFreeCount();

private:
	typedef map<_SendMessage*, _SendMessage*> mapSendMessage;
	mapSendMessage              m_mapMessageUsed;                      //已使用的
	mapSendMessage              m_mapMessageFree;                      //没有使用的
	ACE_Recursive_Thread_Mutex  m_ThreadWriteLock;                     //控制多线程锁
	uint32                      m_u4CurrMaxCount;
};
#endif

