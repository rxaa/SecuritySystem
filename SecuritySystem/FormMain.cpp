#include "StdAfx.h"
#include "FormMain.h"
#include "FormAbout.h"
#include "FormCrypt.h"
#include "FormConnect.h"
#include "FormSet.h"
#include "FormRemoteFile.h"
#include "FormCMD.h"

FormMain::FormMain(void)
{
	resourceID_ = IDD_MAIN;
	ptr_ = this;
}


FormMain::~FormMain(void)
{
	LOCKED(G::listLock_);

	ptr_ = nullptr;
}

void FormMain::OnInit()
{
	SetIcon(IDI_ICON1);

	buttonConnect_.Init(IDOK);
	buttionFile_.Init(IDC_BUTTON1);
	buttonDisconnect_.Init(IDC_BUTTON2);
	buttonCmd_.Init(IDC_BUTTON3);
	buttonProcManag_.Init(IDC_BUTTON4);
	viewHost_.Init(IDC_LIST1);
	hostCount_.Init(IDC_EDIT1);
	buttonCancel_.Init(IDC_EXIT);
	tary_.Init(cct_("Security System"), IDI_ICON1);


	buttonConnect_.onDraw_ = Button::GreenButton;
	buttonConnect_.onClick_ = [&](){
		NewWindow<FormConnect>()->OpenModal(this);
	};


	buttionFile_.onClick_ = [&](){
		RemoteFile();
	};


	buttonCmd_.onClick_ = [&](){
		CommanLine();
	};

	buttonProcManag_.onClick_ = [&](){
		RemoteProcess();
	};

	buttonDisconnect_.onDraw_ = Button::OrangeButton;
	buttonDisconnect_.onClick_ = [&](){


	};


	viewHost_.AddColumn(cct_("UID"));
	viewHost_.AddColumn(cct_("IP"), 150);
	viewHost_.AddColumn(cct_("计算机名"), 150);

	
	hostCount_.SetText(cct_("已连接主机数:0"));

	
	buttonCancel_.onDraw_ = Button::RedButton;
	buttonCancel_.onClick_ = [&]{
		Close(true);
	};

	
	Tray::OnLeftClick() = [&]{
		Show();
	};
	Tray::OnRightClick() = [&]{
		PopMenu(IDR_TRAY_MENU, this);
	};


	onClose_ = [&]{
		Hide();
	};

	AddEvent(IDM_EXIT, [&]{
		Close(true);
	});
	AddEvent(IDM_ABOUT, [&]{
		FormAbout().OpenModal(this);
	});
	AddEvent(ID_32772, [&]{
		Show();
	});

	AddEvent(ID_32771, [&]{
		NewWindow<FormCrypt>()->Open();
	});

	AddEvent(ID_32778, [&]{
		NewWindow<FormSet>()->OpenModal(this);
	});
	AddEvent(ID_32775, [&]{
		RemoteFile();
	});

	AddEvent(ID_32780, [&]{
		NewWindow<FormConnect>()->OpenModal(this);
	});

}

void FormMain::RemoteFile()
{
	LOCKED(G::listLock_);

	int i = viewHost_.GetSelectIndex();
	if (i < 0 || i >= G::serverList_.Count())
	{
		PopHostErrMsg();
		return;
	}
	auto & server = G::serverList_[i];
	if (server->formFile_ != nullptr)
		server->formFile_->SetActive();
	else
		NewWindow<FormRemoteFile>(server)->Open();
}

void FormMain::CommanLine()
{
	LOCKED(G::listLock_);

	int i = viewHost_.GetSelectIndex();
	if (i < 0 || i >= G::serverList_.Count())
	{
		PopHostErrMsg();
		return;
	}

	auto & server = G::serverList_[i];
	if (server->formCmdPtr_ != nullptr)
		server->formCmdPtr_->SetActive();
	else
		NewWindow < FormCMD>(server)->Open();
}

void FormMain::RemoteProcess()
{
	int i = viewHost_.GetSelectIndex();
	if (i < 0)
	{
		PopHostErrMsg();
		return;
	}
}

FormMain * FormMain::ptr_ = nullptr;
