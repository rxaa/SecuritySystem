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
	//调用API来获得主机名;
	int nRetCode = gethostname(szHostName, sizeof(szHostName));
	if (nRetCode != 0)
	{
		BREAK_POINT_MSG("gethostname failed");
		ERR(tcc_("gethostname failed"));
		return;
	}

	con->Send(Direct::ResponseHost, szHostName);


};
template<>
void DirectProc<Direct::ResponseHost>::Func(MainConnecter * con, char * msg, uint size)
{
	//连接成功,更新服务器列表
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

template<>
void DirectProc<Direct::GetCmd>::Func(MainConnecter * con, char * msg, uint)
{
	if (!con->cmd_ || con->cmd_->IsClosed())
	{
		con->cmd_.reset(new df::cmd());

		ConnPtr ptr(con);

		df::AsyncStart([=]{

			ptr->cmd_->Read([&](CC str){
				ptr->Send(Direct::ResponseCmd, str);
			});

		});
	}

	con->cmd_->Exec(msg);
}

template<>
void DirectProc<Direct::ResponseCmd>::Func(MainConnecter * con, char * msg, uint)
{
	if (con->formCmdPtr_)
	{
		df::IntoPtr < FormCMD> formPtr(con->formCmdPtr_);
		formPtr->textRes_.AddText(msg);
	}
}

template<>
void DirectProc<Direct::CloseCmd>::Func(MainConnecter * con, char *, uint)
{
	if (con->cmd_)
	{
		con->cmd_->Close();
	}
}

template<>
void DirectProc<Direct::GetMenu>::Func(MainConnecter * con, char *msg, uint)
{
	CC menu(msg);
	SS res(256);

	//数据结构:目录名\n类型\n信息
	//目录类型:0磁盘,1目录,2文件
	if (menu.Length() == 0)
	{
		df::EachDisk([&](DiskInfo & disk){
			res << disk.name_ << tcc_("\n0\n");
			res << DiskInfo::typeInfo[disk.type_];
			res.AddByte(disk.freeSize_) << cct_(" / ");
			res.AddByte(disk.totalSize_) << '\n';
		});
	}
	else
	{
		df::EachDir(menu, [&](FileInfo & file){
			if (file.IsDir())
			{
				res << file.name_ << tcc_("\n1\n\n");
			}
			else
			{
				res << file.name_ << tcc_("\n2\n");
				res.AddByte(file.size_) << '\n';
			}
		});
	}

	res << tcc_("abc");

	con->Send(Direct::ResponseMenu, res);

}

template<>
void DirectProc<Direct::ResponseMenu>::Func(MainConnecter * con, char * msg, uint len)
{
	if (con->formFile_ == nullptr)
		return;

	df::IntoPtr < FormRemoteFile> formPtr(con->formFile_);

	int i = 0;
	CC res[3];
	CC::Split(msg, len, [&](CC c){
		res[i] = c;
		i++;
		if (i >= 3)
		{
			if (res[1].Length() != 1)//数据出错
				return;
			formPtr->viewRemote_.AddImageRow(res[1][0] - '0', res[0], res[2]);
			i = 0;
		}
	});
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

