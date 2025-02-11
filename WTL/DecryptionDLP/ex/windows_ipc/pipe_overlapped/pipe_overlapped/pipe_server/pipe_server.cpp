/* 头文件 */
#include "stdafx.h"

#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
/* 常量 */
#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096
/* 结构定义 */
typedef struct 
{ 
	OVERLAPPED oOverlap; 
	HANDLE hPipeInst; 
	TCHAR chRequest[BUFSIZE]; 
	DWORD cbRead;
	TCHAR chReply[BUFSIZE]; 
	DWORD cbToWrite; 
} PIPEINST, *LPPIPEINST; 
/* 函数声明 */
VOID DisconnectAndClose(LPPIPEINST); 
BOOL CreateAndConnectInstance(LPOVERLAPPED); 
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED); 
VOID GetAnswerToRequest(LPPIPEINST); 
VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED); 
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED); 
/* 全局变量 */
HANDLE hPipe; 
/* ************************************
* int main(VOID) 
* 功能	pipe 通信服务端主函数
**************************************/
int main(VOID) 
{ 
	HANDLE hConnectEvent; 
	OVERLAPPED oConnect; 
	LPPIPEINST lpPipeInst; 
	DWORD dwWait, cbRet; 
	BOOL fSuccess, fPendingIO; 

	// 用于连接操作的事件对象 
	hConnectEvent = CreateEvent( 
		NULL,    // 默认属性
		TRUE,    // 手工reset
		TRUE,    // 初始状态 signaled 
		NULL);   // 未命名

	if (hConnectEvent == NULL) 
	{
		printf("CreateEvent failed with %d.\n", GetLastError()); 
		return 0;
	}
	// OVERLAPPED 事件
	oConnect.hEvent = hConnectEvent; 

	printf("创建连接实例，等待连接\r\n");
	// 创建连接实例，等待连接  
	fPendingIO = CreateAndConnectInstance(&oConnect); 

	while (1) 
	{
		printf("等待客户端连接或读写操作完成\r\n");
		// 等待客户端连接或读写操作完成 
		dwWait = WaitForSingleObjectEx( 
			hConnectEvent,  // 等待的事件 
			INFINITE,       // 无限等待
			TRUE);       

		
		switch (dwWait) 
		{ 
		case 0:		
			// pending 
			if (fPendingIO) 
			{ 
				printf("获取 Overlapped I/O 的结果\r\n");
				// 获取 Overlapped I/O 的结果
				fSuccess = GetOverlappedResult( 
					hPipe,     // pipe 句柄 
					&oConnect, // OVERLAPPED 结构 
					&cbRet,    // 已经传送的数据量
					FALSE);    // 不等待
				if (!fSuccess) 
				{
					printf("ConnectNamedPipe (%d)\n", GetLastError()); 
					return 0;
				}
			} 

			// 分配内存 
			lpPipeInst = (LPPIPEINST) HeapAlloc(GetProcessHeap(),0,sizeof(PIPEINST)); 
			if (lpPipeInst == NULL) 
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError()); 
				return 0;
			}
			lpPipeInst->hPipeInst = hPipe; 

			printf("读和写，注意CompletedWriteRoutine和CompletedReadRoutine的相互调用\r\n");
			// 读和写，注意CompletedWriteRoutine和CompletedReadRoutine的相互调用
			lpPipeInst->cbToWrite = 0; 
			printf("main CompletedWriteRoutine 写入pipe操作的完成函数\r\n"); 
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED) lpPipeInst); 

			printf("再创建一个连接实例，以响应下一个客户端的连接\r\n");
			// 再创建一个连接实例，以响应下一个客户端的连接
			fPendingIO = CreateAndConnectInstance( 
				&oConnect); 
			break; 

			// 读写完成 
		case WAIT_IO_COMPLETION: 
			break; 

		default: 
			{
				printf("WaitForSingleObjectEx (%d)\n", GetLastError()); 
				return 0;
			}
		} 
	} 
	return 0; 
} 

/* ************************************
* CompletedWriteRoutine 
* 	写入pipe操作的完成函数
*	接口参见FileIOCompletionRoutine回调函数定义
*
*	当写操作完成时被调用，开始读另外一个客户端的请求
**************************************/
VOID WINAPI CompletedWriteRoutine(
								  DWORD dwErr, 
								  DWORD cbWritten, 
								  LPOVERLAPPED lpOverLap) 
{ 
	LPPIPEINST lpPipeInst; 
	BOOL fRead = FALSE; 
	// 保存overlap实例
	lpPipeInst = (LPPIPEINST) lpOverLap; 

	// 如果没有错误
	if ((dwErr == 0) && (cbWritten == lpPipeInst->cbToWrite)) 
	{		
		printf("CompletedWriteRoutine  ReadFileEx 读取 client 消息\r\n"); 
		fRead = ReadFileEx( 
			lpPipeInst->hPipeInst, 
			lpPipeInst->chRequest, 
			BUFSIZE*sizeof(TCHAR), 
			(LPOVERLAPPED) lpPipeInst, 
			// 写读操作完成后，调用CompletedReadRoutine
			(LPOVERLAPPED_COMPLETION_ROUTINE) CompletedReadRoutine); 
	}	
	if (! fRead) 
		// 出错，断开连接
		DisconnectAndClose(lpPipeInst); 
} 

/* ************************************
* CompletedReadRoutine 
* 	读取pipe操作的完成函数
*	接口参见FileIOCompletionRoutine回调函数定义
*
*	当读操作完成时被调用，写入回复
**************************************/
VOID WINAPI CompletedReadRoutine(
								 DWORD dwErr, 
								 DWORD cbBytesRead, 
								 LPOVERLAPPED lpOverLap) 
{ 
	printf("CompletedReadRoutine 读取pipe操作的完成函数\r\n"); 
	LPPIPEINST lpPipeInst; 
	BOOL fWrite = FALSE; 

	// 保存overlap实例
	lpPipeInst = (LPPIPEINST) lpOverLap; 

	// 如果没有错误
	if ((dwErr == 0) && (cbBytesRead != 0)) 
	{ 
		// 根据客户端的请求，生成回复
		GetAnswerToRequest(lpPipeInst); 
		// 将回复写入到pipe
		fWrite = WriteFileEx( 
			lpPipeInst->hPipeInst, 
			lpPipeInst->chReply,	//将响应写入pipe
			lpPipeInst->cbToWrite, 
			(LPOVERLAPPED) lpPipeInst, 
			// 写入完成后，调用CompletedWriteRoutine
			(LPOVERLAPPED_COMPLETION_ROUTINE) CompletedWriteRoutine); 
	} 

	if (! fWrite) 
		// 出错，断开连接
		DisconnectAndClose(lpPipeInst); 
} 

/* ************************************
* VOID DisconnectAndClose(LPPIPEINST lpPipeInst) 
* 功能	断开一个连接的实例
* 参数	lpPipeInst，断开并关闭的实例句柄
**************************************/
VOID DisconnectAndClose(LPPIPEINST lpPipeInst) 
{ 
	// 关闭连接实例
	printf("关闭连接实例\r\n"); 
	if (! DisconnectNamedPipe(lpPipeInst->hPipeInst) ) 
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}
	// 关闭 pipe 实例的句柄 
	CloseHandle(lpPipeInst->hPipeInst); 
	// 释放
	if (lpPipeInst != NULL) 
		HeapFree(GetProcessHeap(),0, lpPipeInst); 
} 

/* ************************************
* BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap)
* 功能	建立连接实例
* 参数	lpoOverlap，用于overlapped IO的结构
* 返回值	是否成功
**************************************/
BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap) 
{ 
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\samplenamedpipe"); 
	// 创建named pipe	 
	hPipe = CreateNamedPipe( 
		lpszPipename,             // pipe 名 
		PIPE_ACCESS_DUPLEX |      // 可读可写
		FILE_FLAG_OVERLAPPED,     // overlapped 模式 
		// pipe模式
		PIPE_TYPE_MESSAGE |       // 消息类型 pipe 
		PIPE_READMODE_MESSAGE |   // 消息读模式 
		PIPE_WAIT,                // 阻塞模式
		PIPE_UNLIMITED_INSTANCES, // 无限制实例
		BUFSIZE*sizeof(TCHAR),    // 输出缓存大小
		BUFSIZE*sizeof(TCHAR),    // 输入缓存大小
		PIPE_TIMEOUT,             // 客户端超时
		NULL);                    // 默认安全属性
	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		printf("CreateNamedPipe failed with %d.\n", GetLastError()); 
		return 0;
	}

	// 连接到新的客户端
	return ConnectToNewClient(hPipe, lpoOverlap); 
}

/* ************************************
* BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
* 功能	建立连接实例  这里相当于socket中的投递一个等待连接
* 参数	lpoOverlap，用于overlapped IO的结构
* 返回值	是否成功
**************************************/
BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo) 
{ 
	BOOL fConnected, fPendingIO = FALSE; 

	// 开始一个 overlapped 连接 
	printf("开始一个 overlapped 连接 相当于socket中的投递一个等待连接\r\n"); 
	fConnected = ConnectNamedPipe(hPipe, lpo); 

	if (fConnected) 
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError()); 
		return 0;
	}
	switch (GetLastError()) 
	{ 
		// overlapped连接进行中. 		
	case ERROR_IO_PENDING: 
		printf("overlapped连接进行中\r\n"); 
		fPendingIO = TRUE; 
		break; 
		// 已经连接，因此Event未置位 
	case ERROR_PIPE_CONNECTED: 
		printf("已经连接，因此Event未置位\r\n"); 
		if (SetEvent(lpo->hEvent)) 
			break; 
		// error
	default: 
		{
			printf("ConnectNamedPipe failed with %d.\n", GetLastError());
			return 0;
		}
	} 
	return fPendingIO; 
}

// TODO根据客户端的请求，给出响应
VOID GetAnswerToRequest(LPPIPEINST pipe)
{
	_tprintf( TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
	lstrcpyn( pipe->chReply,  TEXT("******服务器默认回复 Default answer from server") ,BUFSIZE);
	pipe->cbToWrite = (lstrlen(pipe->chReply)+1)*sizeof(TCHAR);
}


