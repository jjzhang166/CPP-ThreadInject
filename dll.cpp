#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

using namespace std;

int main()
{
	return 0;
}

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	ofstream ofresult( "result.txt", ios::app); 
	cout<<"这个在写文件"<<endl;
	ofresult<<"123"<<"你是好孩子"<<endl;
	cout<<"第二次写文件"<<endl;
	ofresult<<"第二次写文件"<<endl;
	return 0;
}