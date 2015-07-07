#include <windows.h>
#include <iostream>
#include "tlhelp32.h"

using namespace std;

int EnableDebugPriv(const char * name)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    //打开进程令牌环
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
    //获得进程本地唯一ID
    LookupPrivilegeValue(NULL, name, &luid) ;
     
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid = luid;
    //调整权限
    AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    return 0;
}

//*****************************************************************************************************************************

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
    HANDLE hRemoteProcess;
    EnableDebugPriv(SE_DEBUG_NAME);
    //打开远程线程
    hRemoteProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId );

    char *pszLibFileRemote;

    //使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名空间
    pszLibFileRemote = (char *) VirtualAllocEx( hRemoteProcess, NULL, lstrlen(DllFullPath)+1, MEM_COMMIT, PAGE_READWRITE);


    //使用WriteProcessMemory函数将DLL的路径名写入到远程进程的内存空间
    WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (void *) DllFullPath, lstrlen(DllFullPath)+1, NULL);

	//##############################################################################
    //计算LoadLibraryA的入口地址
    PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)
            GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");
    //(关于GetModuleHandle函数和GetProcAddress函数)

    //启动远程线程LoadLibraryA，通过远程线程调用创建新的线程
    HANDLE hRemoteThread;
    if( (hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, pfnStartAddr, pszLibFileRemote, 0, NULL) ) == NULL)
    {
        cout<<"Error: the remote thread could not be created.\n"<<endl;
        return FALSE;
    } else {
        cout<<"Success: the remote thread was successfully created.\n"<<endl;
    }
	//##############################################################################

    /*
    // 在//###.....//###里的语句也可以用如下的语句代替:
     DWORD dwID;
     LPVOID pFunc = LoadLibraryA;
     HANDLE hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, pszLibFileRemote, 0, &dwID );
     //是不是感觉简单了很多
    */

    // 释放句柄

    CloseHandle(hRemoteProcess);
    CloseHandle(hRemoteThread);

    return TRUE;
}

// 根据进程名称获取进程ID
DWORD processNameToId(LPCTSTR lpszProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
	{
		return 0;
	}
	while (Process32Next(hSnapshot, &pe)) {
		if (!strcmp(lpszProcessName, pe.szExeFile)) {
			return pe.th32ProcessID;
		}
	}
	return 0;
}

//*****************************************************************************************************************************

int main()
{
    DWORD id = processNameToId("calc.exe");
    cout<<id<<endl;
    InjectDll("E:\\Env\\code\\test\\dll.dll", id);//这个数字是你想注入的进程的ID号
    return 0;
}