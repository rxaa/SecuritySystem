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

		df::IntoPtr<FormConnect> formPtr(this);
		FormLoad::RunAsync(tcc_("正在连接中..."), this, [=](FormLoad & formLoading){
			try
			{
				auto con = df::IocpSocket::Connect<MainConnecter, false>(textHostName_.GetText(), G::main.listen_port);
				SS psw = formPtr->textPSW_.GetText();
				UCHAR key[32];
				Sha2PasswordBuf(psw, key);
				con->SessionCrypt_.InitByteKey(key);
				con->StartRecvIo();
			}
			catch (df::WinException & ex)
			{
				formLoading.Close();
				formPtr->Message(cct_("连接失败!\r\n") + ex.message_);
			}
		

		});
	};
}
