#include "stdafx.h"
#include "FormCMD.h"


FormCMD::FormCMD(ConnPtr & con)
:con_(con)
{
	resourceID_ = IDD_CMD;
	con_->formCmdPtr_ = this;
}


FormCMD::~FormCMD()
{
	if (con_)
	{
		con_->Send(Direct::CloseCmd, tcc_(""));
		con_->formCmdPtr_ = nullptr;
	}

}

void FormCMD::OnInit()
{

	textRes_.Init(IDC_EDIT1);
	//textRes_.SetReadOnly(true);
	textCom_.Init(IDC_EDIT2);
	buttonOk_.Init(IDOK);
	buttonOk_.onClick_ = [&](){
		SendCmd();
	};

	textRes_.marginLeft_ = 5;
	textRes_.marginRight_ = 5;
	textRes_.marginTop_ = 5;
	textRes_.marginBottom_ = buttonOk_.GetHeight() + 10;

	textCom_.marginLeft_ = 5;
	textCom_.marginRight_ = buttonOk_.GetWidth() + 10;
	textCom_.marginBottom_ = 6;

	buttonOk_.marginRight_ = 5;
	buttonOk_.marginBottom_ = 5;

}

void FormCMD::OnResize()
{

}

void FormCMD::SendCmd()
{
	if (!con_ || con_->IsClosed())
	{
		PopMessage(tcc_("连接已断开!"));
		return;
	}

	SS msg = textCom_.GetText();
	if (msg.Length() < 1)
	{
		PopMessage(tcc_("请输入命令!"));
		textCom_.SetFocus();
		return;
	}

	msg += tcc_("\r\n");
	//textRes_.AddText(msg);
	con_->Send(Direct::GetCmd, msg);
	textCom_.SetText(tcc_(""));
	textCom_.SetFocus();
}

void FormCMD::OnKeyUp(int key)
{
	COUT(key);
}
