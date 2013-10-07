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
	textHostName_.Init(IDC_COMBO1);
	textPSW_.Init(IDC_EDIT2);

	butOK_.onClick_ = [&]{

		if (textHostName_.GetText() == tcc_(""))
		{
			PopMessage(tcc_("请输入主机地址!"));
			textHostName_.SetFocus();
			return;
		}

		if (textPSW_.GetText() == tcc_(""))
		{
			PopMessage(tcc_("请输入密码!"));
			textPSW_.SetFocus();
			return;
		}

		FormLoad::RunAsync(tcc_("正在连接中..."), this, [=](FormLoad & form){
			df::ThreadSleep(3000);
		});
	};
}
