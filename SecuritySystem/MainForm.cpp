#include "StdAfx.h"
#include "MainForm.h"
#include "AboutForm.h"
#include "CryptForm.h"

MainForm::MainForm(void)
{
	resourceID_ = IDD_MAIN;
}


MainForm::~MainForm(void)
{
}

void MainForm::OnInit()
{
	SetIcon(IDI_ICON1);

	buttonConnect_.Init(IDOK);
	buttonConnect_.onDraw_ = Button::GreenButton;
	buttonConnect_.onClick_ = [&](){
	};

	buttionFile_.Init(IDC_BUTTON1);
	buttonDisconnect_.Init(IDC_BUTTON2);
	buttonCmd_.Init(IDC_BUTTON3);
	buttonProcManag_.Init(IDC_BUTTON4);

	buttonDisconnect_.onDraw_ = Button::OrangeButton;

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
		AboutForm().OpenModal(this);
	});
	AddEvent(ID_32772, [&]{
		Show();
	});

	AddEvent(ID_32771, [&]{
		NewWindow<CryptForm>()->Open();
	});
}
