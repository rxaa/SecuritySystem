#include "StdAfx.h"
#include "AboutForm.h"


AboutForm::AboutForm(void)
{
	resourceID_ = IDD_ABOUTBOX;
}


AboutForm::~AboutForm(void)
{
}

void AboutForm::OnInit()
{
	butOK_.Init(IDOK);
	butOK_.onClick_ = [&]{
		Close();
	};
}
