#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain(HINSTANCE hInst     /* Library instance handle. */,
	DWORD reason        /* Reason this function is being called. */,
	LPVOID reserved     /* Not used. */)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(0, TEXT("From DLL\n"), TEXT("Process Attach"), MB_ICONINFORMATION);
		break;

	case DLL_PROCESS_DETACH:
		MessageBox(0, TEXT("From DLL\n"), TEXT("Process Detach"), MB_ICONINFORMATION);
		break;

	case DLL_THREAD_ATTACH:
		MessageBox(0, TEXT("From DLL\n"), TEXT("Thread Attach"), MB_ICONINFORMATION);
		break;

	case DLL_THREAD_DETACH:
		MessageBox(0, TEXT("From DLL\n"), TEXT("Thread Detach"), MB_ICONINFORMATION);
		break;
	}

	return TRUE;
}