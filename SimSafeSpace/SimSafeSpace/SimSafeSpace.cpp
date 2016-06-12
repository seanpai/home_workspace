// SimSafeSpace.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include <future>
#include <istream>
#include <ostream>
#include <condition_variable>

using namespace std;

void ThreadFunc(int& Aug)
{
	do
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		Aug++;
		cout << "[ThreadFunc] count: " << Aug <<endl;
	} 
	while (10 >= Aug);
}

using FUNC_TYPE = int(const std::string&);

int ThreadFunc2(const std::string& strAug)
{
	std::cout << strAug.c_str() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "starting getting the input from user..." << std::endl;

	std::string strInput;
	int nValue = 0;
	do
	{
		std::getline(cin, strInput);
		nValue = strInput.compare("end");
		std::cout << "echo: " << strInput.c_str() << std::endl;
		std::cout << "compare result: " << nValue << std::endl;
	} while (0 != nValue);

	//do
	//{
	//	
	//} while (std::cin.fail());

	return 1;
}

int main()
{
	int a = 0;
	//std::thread ThreadInstance(ThreadFunc, std::ref(a));
	std::string strMessage("thread is running");

	std::packaged_task<FUNC_TYPE> task(ThreadFunc2);
	//std::packaged_task<FUNC_TYPE> task(std::bind(ThreadFunc2, std::ref(strMessage)));
	std::future<int> RetValue = task.get_future();


	std::thread ThreadInstance2(std::move(task), std::ref(strMessage));
	ThreadInstance2.detach();
	int nValue = RetValue.get();
	std::cout << "[main] thread2 terminated" << std::endl;

	//std::thread ThreadInstance([&a]() -> void {
	//	do
	//	{
	//		std::this_thread::sleep_for(std::chrono::seconds(1));
	//		a++;
	//		cout << "[ThreadFunc] count: " << a << endl;
	//	} while (10 >= a); });

	//ThreadInstance.join();
	cout<<"[main] Process terminated, a: " <<a<<endl;
	
    return 0;
}

