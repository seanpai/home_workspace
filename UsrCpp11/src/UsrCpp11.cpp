
#include <iostream>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/move/unique_ptr.hpp>
#include "SmartPtr.h"

using namespace std;

#ifndef _CPP11__
#undef _CPP11__
#endif

typedef void (*IntDeletor)(int*);

void UserDeleter(int* pEntry)
{
	std::cout<<"UserDeleter called"<<std::endl;
	delete [] pEntry;
}

class UserDeletor
{
public:
	void operator()(int* pData)
	{
		std::cout<<"UserDeletor called: "<<pData<<std::endl;
		delete [] pData;
	}
};

int main()
{
//	auto Deleter = [](int* pValue)
//	{
//		delete [] pValue;
//		std::cout << "Deleter called" << std::endl;
//	};

//	class Deleter
//	{
//		void operator()(int* pPtrObj)
//		{
//			delete [] pPtrObj;
//		}
//		std::cout << "Deleter called" << std::endl;
//	};


	//UsrCpp11::ArrayDeleter<int> Deleter;

	UserDeletor tempDeletor;

	hhi::shared_ptr<int> smartptr_1(new int, tempDeletor);
//	hhi::shared_ptr<int> smartptr_2(new int[5], UserDeleter);
//	hhi::shared_ptr<int> smartptr_3(smartptr_2);
//	hhi::shared_ptr<int> smartptr_4;
//	smartptr_4 = smartptr_2;


	//boost::shared_ptr<int> smartptr_boost(new int, tempDeletor);



//	hhi::unique_ptr<int, UserDeletor> smartptr_5(new int[5], tempDeletor);
//	hhi::unique_ptr<int, IntDeletor> smartptr_6(new int[5], UserDeleter);

	return 0;
}
