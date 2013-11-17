#include "stdafx.h"
#include "DirectFunc.h"
#include "../../df/Proc.h"

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

		df::AsyncStart([=] {

			ptr->cmd_->Read([&](CC str) {
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
		df::EachDisk([&](DiskInfo & disk) {
			res << disk.name_ << tcc_("\n0\n");
			res << DiskInfo::typeInfo[disk.type_];
			res.AddByte(disk.freeSize_) << cct_(" / ");
			res.AddByte(disk.totalSize_) << tt_('\n');
		});
	}
	else
	{
		df::EachDir(menu, [&](FileInfo & file) {
			if (file.IsDir())
			{
				res << file.name_ << tcc_("\n1\n\n");
			}
			else
			{
				res << file.name_ << tcc_("\n2\n");
				res.AddByte(file.size_) << tt_('\n');
			}
		});
	}

	con->Send(Direct::ResponseMenu, res);

}

template<>
void DirectProc<Direct::ResponseMenu>::Func(MainConnecter * con, char * msg, uint len)
{
	List<CC> fileList_;
	List<CC> fileInfoList_;

	if (con->formFile_ == nullptr)
		return;
	df::IntoPtr < FormRemoteFile> formPtr(con->formFile_);

	formPtr->remoteDirCount_ = 0;
	formPtr->viewRemote_.Clear();
	int i = 0;
	CC res[3];
	CC::Split(msg, len, [&](CC c) {
		res[i] = c;
		i++;
		if (i >= 3)
		{
			if (res[1].Length() != 1)//数据出错
				return;

			if (res[1][0] != '2')
			{
				formPtr->viewRemote_.AddImageRow(res[1][0] - '0', res[0], res[2]);
				formPtr->remoteDirCount_++;
			}
			else
			{
				fileList_.Add(res[0]);
				fileInfoList_.Add(res[2]);
			}
			i = 0;
		}
	});

	for (int i = 0; i < fileList_.Count(); i++)
	{
		formPtr->viewRemote_.AddImageRow(2, fileList_[i], fileInfoList_[i]);
	}
}


////服务器响应文件下载请求(8字节断点+文件名),返回8字节文件长度(小端)
template<>
void DirectProc<Direct::DownloadFile>::Func(MainConnecter * con, char * msg, uint len)
{
	if (len < 9)
	{
		BREAK_POINT_MSG("长度不对");
		con->Close();
		return;
	}

	int64_t pos = *(int64_t*)msg;
	msg += sizeof(int64_t);
	CC menu(msg);
	if (menu.Length() == 0)
	{
		con->SendTrandsferError(TRANS_ERRCODE_INVALID_FILE);
		return;
	}

	if (!con->file_)
	{
		con->file_.reset(new FileConnect);
	}
	con->file_->fileNameFrom_ = menu;
	if (!con->file_->file_.Open(menu, false, false, true))
	{
		con->SendTrandsferError(TRANS_ERRCODE_OPEN_FAILED);
		return;
	}
	auto fSize = con->file_->file_.GetFileSize();
	con->file_->transferedSize_ = pos;
	con->file_->file_.SeekStart(pos);
	con->file_->state = FileConnect::StateUpload;
	con->Send(Direct::ResponseDownload, &fSize, sizeof(int64_t));
}

//客户端接收8字节文件长度
template<>
void DirectProc<Direct::ResponseDownload>::Func(MainConnecter * con, char * msg, uint len)
{
	if (len < 8 || !con->file_)
	{
		BREAK_POINT_MSG("长度或参数不对");
		con->Close();
		return;
	}
	auto f = con->file_.get();
	int64_t fileSize = *(int64_t*)msg;
	if (fileSize < 0)
		fileSize = 0;


	if (!f->file_.Open(f->fileNameTo_, true, false, false))
	{
		con->SendTrandsferError(TRANS_ERRCODE_WRITE);
		return;
	}
	f->file_.SeekStart(f->transferedSize_);


	f->file_.SetFileSizeVar(fileSize);

}

template<>
void DirectProc<Direct::Message>::Func(MainConnecter *, char * msg, uint)
{
	SS s(msg);
	df::AsyncStart([=] {
		df::msg(s);
	});
}

template<>
void DirectProc<Direct::RecvFileData>::Func(MainConnecter * con, char * msg, uint len)
{
	if (!con->file_)//发生错误
	{
		BREAK_POINT_MSG("con->file_不存在");
		con->SendTrandsferError(TRANS_ERRCODE_UNKNOWN);
		return;
	}

	auto & f = con->file_->file_;
	if (f.IsClosed())
	{
		con->SendTrandsferError(TRANS_ERRCODE_HANDLE);
		return;
	}
	if (!f.Write(msg, len))
	{
		ERR(con->file_->fileNameTo_ + tcc_(" 文件写入失败!"));
		con->SendTrandsferError(TRANS_ERRCODE_WRITE);
		return;
	}

	con->file_->transferedSize_ += len;

	if (con->file_->state != FileConnect::StateNone)
		con->file_->onTransfer_();

}

template<>
void DirectProc<Direct::TransferComplete>::Func(MainConnecter * con, char *, uint)
{
	if (!con->file_)//发生错误
	{
		BREAK_POINT_MSG("con->file_不存在");
		con->SendTrandsferError(TRANS_ERRCODE_UNKNOWN);
		return;
	}

	auto f = con->file_.get();
	ON_EXIT({
		f->Clear();
		f->Release();
	});
	f->onCompleted_();
}

template<>
void DirectProc<Direct::TransferError>::Func(MainConnecter * con, char * msg, uint len)
{
	CC res[2];

	int i = 0;
	CC::Split(msg, len, [&](CC c) {
		if (i > 1)
		{
			BREAK_POINT_MSG("数据错误!");
			return false;
		}
		res[i] = c;
		i++;
		return true;
	});
	COUT("错误码:" << res[0] << " 消息:" << res[1]);

	if (con->file_)
	{
		ON_EXIT({
			con->file_->Clear();
		});
		con->file_->onError_(res[0], res[1]);
	}
}

template<>
void DirectProc<Direct::UploadFile>::Func(MainConnecter * con, char * msg, uint len)
{
	if (len < 16)
	{
		BREAK_POINT_MSG("长度不对");
		con->Close();
		return;
	}
	int64_t size = *(int64_t*)msg;
	msg += sizeof(int64_t);
	int64_t pos = *(int64_t*)msg;
	msg += sizeof(int64_t);
	CC menu(msg);
	if (menu.Length() == 0)
	{
		con->SendTrandsferError(TRANS_ERRCODE_INVALID_FILE);
		return;
	}

	if (!con->file_)
	{
		con->file_.reset(new FileConnect);
	}
	con->file_->fileNameTo_ = menu;
	if (!con->file_->file_.Open(menu, true, false, false))
	{
		con->SendTrandsferError(TRANS_ERRCODE_OPEN_FAILED);
		return;
	}
	con->file_->file_.SetFileSizeVar(size);
	con->file_->transferedSize_ = pos;
	con->file_->file_.SeekStart(pos);
	con->file_->state = FileConnect::StateDownload;
}

template<>
void DirectProc<Direct::DeleteFile>::Func(MainConnecter * con, char * msg, uint len)
{
	if (len < 1)
	{
		con->Send(Direct::Message, tcc_("文件名不能为空!"));
		return;
	}

	CC name(msg);
	if (!df::FileBin::DeleteFile(name))
	{
		ERR(name + tcc_("\r\n文件删除失败!"));
		con->Send(Direct::Message, tcc_("文件删除失败!"));
	}
}

template<>
void DirectProc<Direct::GetProcList>::Func(MainConnecter * con, char *, uint)
{
	SS li(256);
	df::EachProcess([&](ProcInfo & info) {
		li << info.pid_ << tcc_("\n") << info.name_ << tcc_("\n");
	});

	con->Send(Direct::ResponseProc, li);
}

template<>
void DirectProc<Direct::ResponseProc>::Func(MainConnecter * con, char *msg, uint len)
{

	if (con->formProc_ == nullptr)
		return;
	df::IntoPtr < FormProc> formPtr(con->formProc_);

	formPtr->viewProc_.Clear();
	CC res[2];
	int i = 0;
	int count = 0;
	CC::Split(msg, len, [&](CC c) {
		res[i++] = c;

		if (i >= 2)
		{
			count++;
			formPtr->viewProc_.AddRow(res[0], res[1]);
			i = 0;
		}
	});

	formPtr->textCount_.SetText(tcc_("进程数:") + count);

}


template<>
void DirectProc<Direct::KillProc>::Func(MainConnecter * con, char * msg, uint)
{
	uint pid = 0;
	df::StrToVar(msg, pid);
	df::Proc pro;
	if (!pro.Open(pid) || !pro.Terminate())
	{
		con->Send(Direct::Message, tcc_("进程结束失败!"));
		return;
	}

}


template<>
void DirectProc<Direct::GetAttr>::Func(MainConnecter * con, char * msg, uint)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	if (!::GetFileAttributesEx(msg, GetFileExInfoStandard, &wfad))
	{
		ERR(tcc_("GetFileAttributesEx failed"));
		con->Send(Direct::Message, tcc_("属性获取失败!"));
		return;
	}

	SS str(64);
	df::Time::FileTimeToStr(wfad.ftCreationTime, str) << tcc_("\n");
	df::Time::FileTimeToStr(wfad.ftLastWriteTime, str) << tcc_("\n")
		<< wfad.dwFileAttributes << tcc_("\n");

	con->Send(Direct::ResponseAttr, str);
}

template<>
void DirectProc<Direct::ResponseAttr>::Func(MainConnecter * con, char * msg, uint len)
{
	if (con->formFile_ == nullptr)
		return;

	df::IntoPtr < FormRemoteFile> formFilePtr(con->formFile_);

	if (!formFilePtr->attrPtr_)
		return;

	auto & formPtr = formFilePtr->attrPtr_;


	CC res[3];
	int i = 0;
	CC::Split(msg, len, [&](CC c) {
		res[i++] = c;
		if (i >= 3)
		{
			SS text = tcc_("创建时间：");
			text << res[0] << tcc_("\r\n最后修改：") << res[1];
			formPtr->textTime_.SetText(text);
			formPtr->ShowAttr(res[2]);
			return false;
		}
		return true;
	});


}

template<>
void DirectProc<Direct::ReName>::Func(MainConnecter * con, char * msg, uint len)
{
	CC res[2];
	int i = 0;
	CC::Split(msg, len, [&](CC c) {
		res[i++] = c;
		if (i >= 2)
		{
			if (df::FileBin::Rename(res[0], res[1]))
				con->Send(Direct::Message, tcc_("修改成功!"));
			else
				con->Send(Direct::Message, tcc_("修改失败!"));
		}
	});
}

template<>
void DirectProc<Direct::ReAttr>::Func(MainConnecter * con, char * msg, uint len)
{
	CC res[2];
	int i = 0;
	CC::Split(msg, len, [&](CC c) {
		res[i++] = c;
		if (i >= 2)
		{
			DWORD at = 0;
			df::StrToVar(res[1], at);
			if (SetFileAttributes(res[0].GetBuffer(), at))
				con->Send(Direct::Message, tcc_("修改成功!"));
			else
				con->Send(Direct::Message, tcc_("修改失败!"));
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

