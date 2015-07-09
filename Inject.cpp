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
	//�򿪽������ƻ�
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	//��ý��̱���ΨһID
	LookupPrivilegeValue(NULL, name, &luid);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//����Ȩ��
	AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	return 0;
}

//*****************************************************************************************************************************

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
	// ����Ȩ��(�������Ա���)
	EnableDebugPriv(SE_DEBUG_NAME);

	//��Զ���߳�
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId);
	if (hRemoteProcess == NULL)
	{
		cout << "Error: OpenProcess failed!\n" << endl;
		return FALSE;
	}

	//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
	LPVOID pszLibFileRemote = VirtualAllocEx(hRemoteProcess, NULL, lstrlen(DllFullPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (pszLibFileRemote == 0)
	{
		cout << "Error: VirtualAllocEx failed!\n" << endl;
		return FALSE;
	}

	//ʹ��WriteProcessMemory������DLL��·����д�뵽Զ�̽��̵��ڴ�ռ�
	if (!WriteProcessMemory(hRemoteProcess, pszLibFileRemote, DllFullPath, lstrlen(DllFullPath) + 1, NULL))
	{
		cout << "Error: WriteProcessMemory failed!\n" << endl;
		return FALSE;
	}

	//����Զ���߳�LoadLibraryA��ͨ��Զ���̵߳��ô����µ��߳�
	HANDLE hRemoteThread;
	if ((hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, 0, NULL)) == NULL)
	{
		cout << "Error: the remote thread could not be created.\n" << endl;
		return FALSE;
	}
	else
	{
		// �ȴ��߳��˳� Ҫ���ó�ʱ ����Զ���̹߳����³�������Ӧ
		//WaitForSingleObject(hRemoteThread, 10000);
		// ����ȴ��߳� DLL�е�DllMain��ҪдMessageBox
		cout << "Success: the remote thread was successfully created.\n" << endl;
	}

	// �ͷž��
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);

	return TRUE;
}

// ���ݽ������ƻ�ȡ����ID
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
	DWORD id = processNameToId("calc.exe");
	cout << id << endl;

	// ��ȡ��ִ���ļ�����Ŀ¼
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	*(_tcsrchr(szFilePath, '\\')) = 0;

	_tcscat_s(szFilePath, "\\TestDLL.dll");
	InjectDll(szFilePath, id);//�������������ע��Ľ��̵�ID��
	return 0;
}
