#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

using namespace std;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	/*switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL,TEXT("关机代码"),TEXT("关机代码"),0);
		//system("shutdown -s -t 10");
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	break;
	}*/
	MessageBox(NULL, "123", "123", 0);
	return TRUE;
}

int main()
{
	return 0;
}