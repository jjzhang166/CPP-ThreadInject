#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <stdio.h>

using namespace std;

BOOL APIENTRY DllMain(HINSTANCE hInst     /* Library instance handle. */,
	DWORD reason        /* Reason this function is being called. */,
	LPVOID reserved     /* Not used. */)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: //当这个DLL被映射到了进程的地址空间时
		MessageBox(0, TEXT("From DLL\n"), TEXT("Process Attach"), MB_ICONINFORMATION);
		cout<<"Process Attach"<<endl;
		break;

	case DLL_PROCESS_DETACH: //这个DLL从进程的地址空间中解除映射
		MessageBox(0, TEXT("From DLL\n"), TEXT("Process Detach"), MB_ICONINFORMATION);
	cout<<"Process Detach"<<endl;
		break;

	case DLL_THREAD_ATTACH: //一个线程正在被创建
		MessageBox(0, TEXT("From DLL\n"), TEXT("Thread Attach"), MB_ICONINFORMATION);
		cout<<"Thread Attach"<<endl;
		break;

	case DLL_THREAD_DETACH: //线程终结
		MessageBox(0, TEXT("From DLL\n"), TEXT("Thread Detach"), MB_ICONINFORMATION);
		cout<<"Thread Detach"<<endl;
		break;
	}

	return TRUE;
}