#include "stdafx.h"
#include "FormRemoteFile.h"


FormRemoteFile::FormRemoteFile()
{
	resourceID_ = IDD_MFILE;
}


FormRemoteFile::~FormRemoteFile()
{
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
}

void FormRemoteFile::OnLayout()
{
	//窗口边距
	const int16_t border = 5;
	//控件间距
	const int16_t space = 1;

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
	viewLocal_.marginLeft_ = border;
	viewLocal_.marginRight_ = buttonLocalRefresh_.marginRight_;

	buttonRemoteRet_.marginLeft_ = buttonDownload_.marginLeft_ + buttonDownload_.GetWidth() + space;
	buttonRemoteRet_.marginTop_ = border;

	viewRemote_.marginTop_ = viewLocal_.marginTop_;
	viewRemote_.marginLeft_ = buttonRemoteRet_.marginLeft_;
	viewRemote_.marginRight_ = border;

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
	textInfo_.marginBottom_ = border + space + progress_.GetHeight();

	progress_.marginLeft_ = textInfo_.marginLeft_;
	progress_.marginRight_ = border;
	progress_.marginBottom_ = border;
}
