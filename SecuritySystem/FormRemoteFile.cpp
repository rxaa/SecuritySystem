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
	buttonRemoteRet_.Init(IDC_BUTTON3);
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




	buttonLocalRet_.onClick_ = [&](){
		comLocal_.SetText(viewLocal_.Back());
		viewLocal_.SetFocus();
	};

	buttonLocalRefresh_.onClick_ = [&](){
		comLocal_.GetText(viewLocal_.currentMenu_);
		viewLocal_.Refresh();
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

}

void FormRemoteFile::GetRemoteFileList(CC menu)
{
	if (!con_->Send(Direct::GetMenu, menu))
		MessageERR(tcc_("网络错误!"));
}
