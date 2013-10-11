#include "StdAfx.h"
#include "FormMain.h"
#include "FormAbout.h"
#include "FormCrypt.h"
#include "FormConnect.h"
#include "FormSet.h"
#include "FormRemoteFile.h"

FormMain::FormMain(void)
{
	resourceID_ = IDD_MAIN;
	ptr_ = this;
}


FormMain::~FormMain(void)
{
	ptr_ = nullptr;
}

void FormMain::OnInit()
{
	SetIcon(IDI_ICON1);

	buttonConnect_.Init(IDOK);
	buttonConnect_.onDraw_ = Button::GreenButton;
	buttonConnect_.onClick_ = [&](){
		NewWindow<FormConnect>()->OpenModal(this);
	};

	buttionFile_.Init(IDC_BUTTON1);
	buttionFile_.onClick_ = [&](){
		RemoteFile();
	};

	buttonDisconnect_.Init(IDC_BUTTON2);
	buttonCmd_.Init(IDC_BUTTON3);
	buttonCmd_.onClick_ = [&](){
		CommanLine();
	};

	buttonProcManag_.Init(IDC_BUTTON4);
	buttonProcManag_.onClick_ = [&](){
		RemoteProcess();
	};

	buttonDisconnect_.onDraw_ = Button::OrangeButton;
	buttonDisconnect_.onClick_ = [&](){


	};

	viewHost_.Init(IDC_LIST1);
	viewHost_.AddColumn(cct_("UID"));
	viewHost_.AddColumn(cct_("IP"), 150);
	viewHost_.AddColumn(cct_("计算机名"), 150);

	hostCount_.Init(IDC_EDIT1);
	hostCount_.SetText(cct_("已连接主机数:0"));

	buttonCancel_.Init(IDC_EXIT);
	buttonCancel_.onDraw_ = Button::RedButton;
	buttonCancel_.onClick_ = [&]{
		Close(true);
	};

	tary_.Init(cct_("Security System"), IDI_ICON1);
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
	int i = viewHost_.GetSelectIndex();
	if (i < 0)
	{
		PopHostErrMsg();
		return;
	}

	NewWindow<FormRemoteFile>()->Open();
}

void FormMain::CommanLine()
{
	int i = viewHost_.GetSelectIndex();
	if (i < 0)
	{
		PopHostErrMsg();
		return;
	}
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

FormMain * FormMain::ptr_;
