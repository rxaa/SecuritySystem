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

	viewProc_.AddColumn(tcc_("PID"));
	viewProc_.AddColumn(tcc_("进程名:"), 160);

	butOK_.marginRight_ = 10;
	butOK_.marginBottom_ = 10;

	viewProc_.marginLeft_ = 10;
	viewProc_.marginTop_ = 10;
	viewProc_.marginRight_ = 10;
	viewProc_.marginBottom_ = butOK_.GetHeight() + 15;

	SS info;
	df::EachProcess([&](ProcInfo & proc){
		viewProc_.AddRow(info.ClearString() << proc.pid_, proc.name_);
	});
	butOK_.onClick_ = [&]{
	};
}
