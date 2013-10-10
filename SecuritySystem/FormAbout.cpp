#include "StdAfx.h"
#include "FormAbout.h"


FormAbout::FormAbout(void)
{
	resourceID_ = IDD_ABOUTBOX;
}


FormAbout::~FormAbout(void)
{
}

void FormAbout::OnInit()
{
	butOK_.Init(IDOK);
	butOK_.onClick_ = [&]{
		Close();
	};
}
