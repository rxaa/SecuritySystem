#include "stdafx.h"
#include "FormRemoteFile.h"


FormRemoteFile::FormRemoteFile(ConnPtr & con)
:con_(con)
{
	resourceID_ = IDD_MFILE;
	con_->formFile_ = this;
}


FormRemoteFile::~FormRemoteFile()
{

	con_->formFile_ = nullptr;
}

void FormRemoteFile::OnInit()
{
	buttonLocalRet_.Init(IDC_BUTTON2);
	buttonLocalRet_.onDraw_ = Button::GreenButton;
	buttonRemoteRet_.Init(IDC_BUTTON3);
	buttonRemoteRet_.onDraw_ = Button::GreenButton;
	buttonLocalRefresh_.Init(IDC_BUTTON5);
	buttonRemoteRefresh_.Init(IDC_BUTTON6);
	buttonDownload_.Init(IDOK);
	buttonUpload_.Init(IDC_BUTTON1);
	buttonProperty_.Init(IDC_BUTTON4);
	buttonDelete_.Init(IDC_BUTTON7);
	viewLocal_.Init(IDC_LIST1);
	viewRemote_.Init(IDC_LIST2);
	viewQueue_.Init(IDC_LIST4);
	textInfo_.Init(IDC_EDIT3);
	progress_.Init(IDC_PROGRESS1);
	comLocal_.Init(IDC_COMBO3);
	comRemote_.Init(IDC_COMBO4);

	buttonDownload_.onClick_ = [&](){
		DownloadFile();
	};


	buttonLocalRet_.onClick_ = [&](){
		comLocal_.SetText(viewLocal_.Back());
		viewLocal_.SetFocus();
	};


	buttonLocalRefresh_.onClick_ = [&](){
		comLocal_.GetText(viewLocal_.currentMenu_);
		viewLocal_.Refresh();
		viewLocal_.SetFocus();
	};

	buttonRemoteRet_.onClick_ = [&](){
		RemoteMenuBack();
	};

	buttonRemoteRefresh_.onClick_ = [&](){
		GetRemoteFileList(comRemote_.GetText());
		viewRemote_.SetFocus();
	};

	viewLocal_.AddColumn(tcc_("本地文件"), 300);
	viewLocal_.AddColumn(tcc_("大小"));
	viewLocal_.InitImage(IDB_BITMAP3, IDB_BITMAP2, IDB_BITMAP4);

	viewLocal_.onDoubleClick_ = [&](int i){
		comLocal_.SetText(viewLocal_.NaviMenu(i));
	};


	viewRemote_.AddColumn(tcc_("远程文件"), 300);
	viewRemote_.AddColumn(tcc_("大小"));
	viewRemote_.SetImageList(viewLocal_.GetImageList());
	viewRemote_.onDoubleClick_ = [&](int i){
		RemoteDoubleClick(i);
	};


	viewQueue_.AddColumn(tcc_("文件队列"), 350);
	viewQueue_.AddColumn(tcc_("大小"), 100);

	onClose_ = [&](){
		if (con_ && con_->IsTransfering())
		{
			if (MessageOK(tcc_("文件正在传输中,确认中断?")))
			{
				con_->fileConnect_->SendTrandsferError(TRANS_ERRCODE_USER_CLOSED);
				Close();
			}
			return;
		}

		Close();
	};

	viewLocal_.Refresh();
	GetRemoteFileList(tcc_(""));
}

void FormRemoteFile::OnLayout()
{
	//窗口边距
	const int16_t border = 5;
	//控件间距
	const int16_t space = 2;

	//中轴
	int16_t mid = GetWidth() / 2;
	int16_t halfw = buttonDownload_.GetWidth() / 2;

	buttonDownload_.marginLeft_ = mid - halfw;
	buttonUpload_.marginLeft_ = buttonDownload_.marginLeft_;
	buttonProperty_.marginLeft_ = buttonDownload_.marginLeft_;
	buttonDelete_.marginLeft_ = buttonDownload_.marginLeft_;

	buttonLocalRet_.marginLeft_ = border;
	buttonLocalRet_.marginTop_ = border;

	buttonLocalRefresh_.marginRight_ = buttonDownload_.marginLeft_ + space + buttonDownload_.GetWidth();
	buttonLocalRefresh_.marginTop_ = border;

	comLocal_.marginLeft_ = border + space + buttonLocalRet_.GetWidth();
	comLocal_.marginRight_ = buttonLocalRefresh_.GetWidth() + buttonLocalRefresh_.marginRight_ + space;
	comLocal_.marginTop_ = border;

	viewLocal_.marginTop_ = border + buttonLocalRet_.GetHeight() + space;
	viewLocal_.marginBottom_ = border + space + viewQueue_.GetHeight();
	viewLocal_.marginLeft_ = border;
	viewLocal_.marginRight_ = buttonLocalRefresh_.marginRight_;

	buttonRemoteRet_.marginLeft_ = buttonDownload_.marginLeft_ + buttonDownload_.GetWidth() + space;
	buttonRemoteRet_.marginTop_ = border;

	viewRemote_.marginTop_ = viewLocal_.marginTop_;
	viewRemote_.marginLeft_ = buttonRemoteRet_.marginLeft_;
	viewRemote_.marginRight_ = border;
	viewRemote_.marginBottom_ = border + space + viewQueue_.GetHeight();

	buttonRemoteRefresh_.marginTop_ = border;
	buttonRemoteRefresh_.marginRight_ = border;

	comRemote_.marginTop_ = border;
	comRemote_.marginLeft_ = buttonRemoteRet_.marginLeft_ + buttonRemoteRet_.GetWidth() + space;
	comRemote_.marginRight_ = buttonRemoteRefresh_.GetWidth() + border + space;


	viewQueue_.marginLeft_ = border;
	viewQueue_.marginBottom_ = border;
	viewQueue_.marginRight_ = mid - halfw;

	textInfo_.marginLeft_ = viewRemote_.marginLeft_;
	textInfo_.marginRight_ = border;
	textInfo_.marginBottom_ = border + progress_.GetHeight();

	progress_.marginLeft_ = textInfo_.marginLeft_;
	progress_.marginRight_ = border;
	progress_.marginBottom_ = border;
}


void FormRemoteFile::RemoteDoubleClick(int i)
{
	if (i < 0 || i >= remoteDirCount_)
		return;

	SS menu = comRemote_.GetText();

	menu << viewRemote_.GetText(i, 0) << cct_("\\");
	comRemote_.SetText(menu);
	GetRemoteFileList(menu);
}

void FormRemoteFile::GetRemoteFileList(CC menu)
{
	if (!con_->Send(Direct::GetMenu, menu))
		MessageERR(tcc_("网络错误!"));
}

void FormRemoteFile::RemoteMenuBack()
{
	SS menu = comRemote_.GetText();
	auto pos = menu.Find(tt_('\\'), menu.Length() - 2, false);
	if (pos > 0)
		menu.SetSize(pos + 1);
	else
		menu.ClearString();

	comRemote_.SetText(menu);
	GetRemoteFileList(menu);

	viewRemote_.SetFocus();
}

void FormRemoteFile::DownloadFile()
{
	if (con_ && con_->IsTransfering())
	{
		PopMessage(tcc_("已有文件正在传输中!"),600);
		return;
	}

	int i = viewRemote_.GetSelectIndex();
	if (i < remoteDirCount_)
	{
		PopMessage(tcc_("请选择要下载的文件!"));
		return;
	}

	SS menuLocal = comLocal_.GetText();
	if (menuLocal.Length() == 0)
	{
		PopMessage(tcc_("请选择本地储存目录!"));
		return;
	}

	SS menuRemote = comRemote_.GetText();
	SS fileName = viewRemote_.GetText(i, 0);

	menuRemote << tcc_("\\") << fileName;
	menuLocal << tcc_("\\") << fileName;



	auto file = con_->DownloadFileInit();
	file->transferedSize_ = 0;
	file->fileNameFrom_ = std::move(menuRemote);
	file->fileNameTo_ = std::move(menuLocal);

	df::IntoPtr<FormRemoteFile> fPtr(this);

	SS info;
	file->onTransfer_ = [=]()mutable{
		int rate = int(file->transferedSize_ * 100 / file->file_.GetFileSize());

		info = tcc_("进度:");
		info << rate << tcc_("%");
		fPtr->textInfo_.SetText(info);
		fPtr->progress_.SetProgressPos(rate);
	};
	file->onCompleted_ = [=]()mutable{
		fPtr->textInfo_.SetText(tcc_("传输完成!"));
	};

	file->onError_ = [=](CC code, CC msg)mutable{
		fPtr->MessageERR(tcc_("错误码:") + code + tcc_("\r\n") + msg);
	};
	if (!con_->DownloadFileStart())
		MessageERR(tcc_("网络错误!"));
}
