// ipc_client.cpp : 定义控制台应用程序的入口点。

/*
*  author: mikezhang
*
*  演示方法：两个程序编译后，分别启动ipc_client.exe和ipc_server.exe。然后在ipc_client.exe的控制台中随意输入
*         某些字符串，在ipc_server.exe的控制台上就可以显示出来。注意两个程序的数据交换和同步机制。
*
*  酷尚网 时尚的社区，www.coolshang.com  欢迎你的加入！
*
*  有意交流技术或互联网创意的，请联系mike19840321@tom.com
*  进程通信，进程同步，共享内存
*  本程序演示了两个知识点 1，共享内存的使用 2 同步机制，使用自动事件
*
*   注：本程序只是具有演示的作用，不是完善的代码，不具有健壮行，不具有性能的优化，
*      ，不对函数的返回值做处理，不顾及异常等等。
*      感兴趣的朋友可以一起探讨。
*/
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>


using namespace std;
#define BUF_SIZE 256
char szName[] = "Global\\MyFileMappingObject";

int main()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;


    ////也可以用openEvent()
    HANDLE h_event_a = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)"Global\\IPC_event_a");
    HANDLE h_event_b = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)"Global\\IPC_event_b");


    ////也可以用openFileMapping()
    hMapFile = CreateFileMapping(
                   INVALID_HANDLE_VALUE,    // use paging file
                   NULL,                    // default security
                   PAGE_READWRITE,          // read/write access
                   0,                       // max. object size
                   BUF_SIZE,                // buffer size
                   (LPCWSTR)szName);        // name of mapping object



    if (hMapFile == NULL)
    {
        printf("Could not open file mapping object (%d).\n",
               GetLastError());
        return 1;
    }

    pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
                                  FILE_MAP_ALL_ACCESS,  // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);

    if (pBuf == NULL)
    {
        printf("Could not map view of file (%d).\n",
               GetLastError());
        return 2;
    }


    char ss[100];
    memset(ss, 0, sizeof(ss));
    cout << "client is starting........." << endl;
    while(1)
    {

        /*
        *  author: mikezhang
        *
        *  酷尚网 时尚的社区，www.coolshang.com  欢迎你的加入！
        *
        *  进程通信，进程同步，共享内存
        */

        memset(ss, 0, sizeof(ss));
        printf("\nplease input :\n");
        cin.getline(ss, sizeof(ss));
        printf("your message is sending to server:%s\n", ss);

        CopyMemory((PVOID)pBuf, ss, sizeof(ss));
        //注意，先setEvent再wait，与server端的程序相反
        SetEvent(h_event_a);
        int w = WaitForSingleObject(h_event_b, INFINITE);




    }



    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    return 0;
}