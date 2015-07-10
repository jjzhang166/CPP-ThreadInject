#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <tchar.h>

using namespace std;

int EnableDebugPriv(const char * name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	//获得进程本地唯一ID
	LookupPrivilegeValue(NULL, name, &luid);

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
	// 提升权限(必须管理员身份)
	EnableDebugPriv(SE_DEBUG_NAME);

	//打开远程线程
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId);
	if (hRemoteProcess == NULL)
	{
		cout << "Error: OpenProcess failed!\n" << endl;
		return FALSE;
	}

	//使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名空间
	LPVOID pszLibFileRemote = VirtualAllocEx(hRemoteProcess, NULL, lstrlen(DllFullPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (pszLibFileRemote == 0)
	{
		cout << "Error: VirtualAllocEx failed!\n" << endl;
		return FALSE;
	}

	//使用WriteProcessMemory函数将DLL的路径名写入到远程进程的内存空间
	if (!WriteProcessMemory(hRemoteProcess, pszLibFileRemote, DllFullPath, lstrlen(DllFullPath) + 1, NULL))
	{
		cout << "Error: WriteProcessMemory failed!\n" << endl;
		return FALSE;
	}

	//启动远程线程LoadLibraryA，通过远程线程调用创建新的线程
	HANDLE hRemoteThread;
	if ((hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, 0, NULL)) == NULL)
	{
		cout << "Error: the remote thread could not be created.\n" << endl;
		return FALSE;
	}
	else
	{
		// 等待线程退出 要设置超时 以免远程线程挂起导致程序无响应
		//WaitForSingleObject(hRemoteThread, 10000);
		// 如果等待线程 DLL中的DllMain不要写MessageBox
		cout << "Success: the remote thread was successfully created.\n" << endl;
	}

	// 释放句柄
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);

	return TRUE;
}

// 根据进程名称获取进程ID
DWORD processNameToId(LPCTSTR lpszProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 pe = {};
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hSnapshot, &pe))
		{
			CloseHandle(hSnapshot);
			return 0;
		}
		while (Process32Next(hSnapshot, &pe)) {
			if (!strcmp(lpszProcessName, pe.szExeFile)) {
				CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
		}
	}
	return 0;
}

//*****************************************************************************************************************************

int main()
{
	
	DWORD id = processNameToId("notepad++.exe");
	cout << id << endl;

	// 获取可执行文件所在目录
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	*(_tcsrchr(szFilePath, '\\')) = 0;

	_tcscat_s(szFilePath, "\\dll.dll");
	InjectDll(szFilePath, id);//这个数字是你想注入的进程的ID号
	return 0;
}