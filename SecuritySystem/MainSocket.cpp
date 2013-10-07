#include "stdafx.h"
#include "MainSocket.h"


MainConnecter::DirectProcFunc MainConnecter::FuncList[Direct::DirectEnd];



template<unsigned DirectI>
struct DirectProc
{
	static_assert(DirectI < Direct::DirectEnd, "Index out of range");

	static void Func(MainConnecter *, char *, uint)
	{
		static_assert(DirectI >= Direct::DirectEnd, "no implementation direct process function");
	}
};


template<>
void DirectProc<Direct::GetHost>::Func(MainConnecter *, char *, uint size)
{

};



void MainConnecter::OnConnect()
{

}

void MainConnecter::OnRecv(char *, uint)
{

}


template<unsigned I>
struct InitProc
{
	inline static void Init()
	{
		MainConnecter::FuncList[I] = DirectProc<I>::Func;

		InitProc<I + 1>::Init();
	}
}; 

template<>
struct InitProc< Direct::DirectEnd>
{
	static void Init()
	{
	}
};



void MainConnecter::InitFunc()
{
	InitProc<0>::Init();
}
