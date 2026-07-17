#pragma once
#include<DMPCH.h>
//#include"BaseDelegate.h"
//#include"MultiDelegate.h"
//#include"DelegateHash.h"
//using namespace DM;
//void TestFun()
//{
//	std::cout << "TestFun()" << std::endl;
//}
//static int TestStaticFun(int a)
//{
//	std::cout << "TestStaticFun()" << std::endl;
//	std::cout << a << std::endl;
//	return a;
//}
//void BindFun_Test()
//{
//	BaseDelegate<void()>Test;
//	Test.BindFun(TestFun);
//	Test.Execute();
//}
//void BindStaticFun_Test()
//{
//	BaseDelegate<int(int)>Test;
//	Test.BindFun(TestStaticFun);
//	Test.Execute(1);
//}
//class MebFunTest
//{
//public:
//	MebFunTest() = default;
//	int a = 0;
//	float TestMebFun(float val)
//	{
//		std::cout << "Member fun Test,Param:"<< this->a << std::endl;
//		this->a;
//		return val;
//	}
//	void MultiDelegateTest()
//	{
//		std::cout << "MultiDelegate MemeberFun Test" << std::endl;
//	}
//};
//void BindMebFun_Test()
//{
//	BaseDelegate<float(float)>test;
//	{
//		MebFunTest*a=new MebFunTest();
//		a->a = 4;
//		std::shared_ptr<MebFunTest>tes(a);
//		test.BindMebFun(tes, &MebFunTest::TestMebFun);
//		test.Execute(1.f);
//	}
//}
//void BindLamFun_Test()
//{
//	BaseDelegate<float(char ch)>test;
//	char ch = 'a';
//	{
//		const auto lam = [&](char c)->float {
//			ch = 'b';
//			std::cout << c << std::endl;
//			return 1;
//		};
//		test.BindLamFun(lam);
//	}
//	std::cout << test.Execute('c') << std::endl;
//	std::cout << ch << std::endl;
//}
//
//void MultiDelegateFun_Test1()
//{
//	std::cout << "MultiDelegateFun_Test1" << std::endl;
//}
//void MultiDelegateFun_Test2()
//{
//	std::cout << "MultiDelegateFun_Test2" << std::endl;
//}
//static void MultiDelegateFun_Test3()
//{
//	std::cout << "Static Fun MultiDelegateFun_Test3" << std::endl;
//}
//void MultiDelegate_Test()
//{
//	MultiDelegate<void(void)>md;
//	md.Add(MultiDelegateFun_Test1);
//	md.Add(MultiDelegateFun_Test1);
//	md.Add(MultiDelegateFun_Test2);
//	md.Add(MultiDelegateFun_Test3);
//	auto a = []()
//	{
//		std::cout << "Lambda表达式" << std::endl;
//	};
//	md.Add(a);
//	md.Add(a);
//	/*md.Add([]() {std::cout << "Lambda表达式" << std::endl;});
//	md.Add([]() {std::cout << "Lambda表达式" << std::endl; });*/
//	std::shared_ptr<MebFunTest>mebtest=std::make_shared<MebFunTest>();
//	md.Add(mebtest, &MebFunTest::MultiDelegateTest);
//	//md.Add(mebtest, &MebFunTest::MultiDelegateTest);
//	md.BroadCast();
//	//size_t a = FTypeHash<MultiDelegate<int(void)>>()();
//	//std::cout<< a << std::endl;
//	//std::cout<< FTypeHash<MultiDelegate<void(void)>>() << std::endl;
//}
//void DelegateTest()
//{
//	BindFun_Test();
//	BindStaticFun_Test();
//	BindMebFun_Test();
//	BindLamFun_Test();
//	MultiDelegate_Test();
//}
