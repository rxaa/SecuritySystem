#include "StdAfx.h"
#include "FormProc.h"
#include "../../df/Proc.h"

FormProc::FormProc(ConnPtr & con)
:con_(con)
{
	resourceID_ = IDD_PROC;
	con_->formProc_ = this;
}


FormProc::~FormProc(void)
{
	con_->formProc_ = nullptr;
}

void FormProc::OnInit()
{
	butOK_.Init(IDOK);
	butOK_.onDraw_ = Button::RedButton;
	viewProc_.Init(IDC_LIST1);
	textCount_.Init(IDC_EDIT1);
	buttonRsfresh_.Init(IDC_BUTTON1);
	buttonRsfresh_.onClick_ = [&](){
		con_->Send(Direct::GetProcList);
		viewProc_.SetFocus();
	};

	viewProc_.AddColumn(tcc_("PID"));
	viewProc_.AddColumn(tcc_("进程名:"), 200);

	textCount_.marginTop_ = 15;
	buttonRsfresh_.marginTop_ = 10;
	buttonRsfresh_.marginRight_ = 10;
	butOK_.marginRight_ = 10;
	butOK_.marginBottom_ = 10;

	viewProc_.marginLeft_ = 10;
	viewProc_.marginTop_ = 15 + buttonRsfresh_.GetHeight();
	viewProc_.marginRight_ = 10;
	viewProc_.marginBottom_ = butOK_.GetHeight() + 15;


	butOK_.onClick_ = [&]{
	};

	con_->Send(Direct::GetProcList);
}
