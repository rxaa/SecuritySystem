#include "stdafx.h"
#include "DirectFunc.h"

DirectFunc::DirectProcFunc DirectFunc::FuncList[Direct::_DirectEnd];


template<unsigned DirectI>
struct DirectProc
{
	static_assert(DirectI < Direct::_DirectEnd, "Index out of range");

	static void Func(MainConnecter *, char *, uint)
	{
		static_assert(DirectI >= Direct::_DirectEnd, "no implementation direct process function");
	}
};


template<>
void DirectProc<Direct::GetHost>::Func(MainConnecter * con, char *, uint)
{
	char szHostName[256] = { 0 };
	//����API�����������;
	int nRetCode = gethostname(szHostName, sizeof(szHostName));
	if (nRetCode != 0)
	{
		BREAK_POINT_MSG("gethostname failed");
		ERR("gethostname failed");
		return;
	}

	con->Send(Direct::ResponseHost, szHostName);


};
template<>
void DirectProc<Direct::ResponseHost>::Func(MainConnecter * con, char * msg, uint size)
{
	//���ӳɹ�,���·������б�
	LOCKED(G::listLock_);
	G::serverList_.Add(con);

	if (FormMain::ptr_)
	{
		if (size < 1)
			size = 1;
		FormMain::ptr_->viewHost_.AddRow(tcc_("") + con->GetSocket(), con->GetRemoteIpStr(), CC(msg, size - 1));
		FormMain::ptr_->UpdateHostCount();
	}

}



template<unsigned I>
struct InitProc
{
	inline static void Init()
	{
		DirectFunc::FuncList[I] = DirectProc<I>::Func;

		InitProc<I + 1>::Init();
	}
};

template<>
struct InitProc< Direct::_DirectEnd>
{
	static void Init()
	{
	}
};



void DirectFunc::InitFunc()
{
	InitProc<0>::Init();
}

