#include "stdafx.h"
#include "FormConnect.h"
#include "FormLoad.h"

FormConnect::FormConnect()
{
	resourceID_ = IDD_CONNECT;
}


FormConnect::~FormConnect()
{
}

void FormConnect::OnInit()
{
	butOK_.Init(IDOK);
	textHostName_.Init(IDC_EDIT1);

	butOK_.onClick_ = [&]{

		if (textHostName_.GetText() == tcc_(""))
		{
			PopMessage(tcc_("������������ַ!"));
			textHostName_.SetFocus();
			return;
		}

		FormLoad::RunAsync(tcc_("��������..."), this, [=](FormLoad & form){
			Sleep(2000);
		});
	};
}
