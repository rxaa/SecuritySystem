#include "StdAfx.h"
#include "FormMain.h"
#include "FormAbout.h"
#include "FormCrypt.h"
#include "FormConnect.h"

FormMain::FormMain(void)
{
	resourceID_ = IDD_MAIN;
}


FormMain::~FormMain(void)
{
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
	buttonDisconnect_.Init(IDC_BUTTON2);
	buttonCmd_.Init(IDC_BUTTON3);
	buttonProcManag_.Init(IDC_BUTTON4);

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
}
