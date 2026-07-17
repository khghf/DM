#pragma once
#include"MultiDelegate.h"
#define DECLARE_DELEGATE(Name,Ret,...)\
using Name=DM::BaseDelegate<Ret(__VA_ARGS__)>
#define DECLARE_MULTI_DELEGATE(Name,Ret,...)\
using Name=DM::MultiDelegate<Ret(__VA_ARGS__)>

//DECLARE_DELEGATE(DTEST, int, int, int);
//DECLARE_MULTI_DELEGATE(MDTEST, int, int, int);
//void MacroDelegateTest()
//{
//	DTEST t;
//	t.BindLamFun([](int a,int b)->int {
//		LOG_Core_INFO(a+b);
//		return a;
//		});
//	t.Execute(2, 3);
//
//	MDTEST md;
//	md.Add([](int a, int b)->int {
//		LOG_Core_INFO(a + b);
//		return a;
//		});
//	md.Add([](int a, int b)->int {
//		LOG_Core_INFO(a + b);
//		return a;
//		});
//	auto a = [](int a, int b)->int {
//		LOG_Core_INFO(a + b);
//		return a;
//		};
//	md.Add(a);
//	md.Add(a);
//	md.BroadCast(4, 4);
//}
