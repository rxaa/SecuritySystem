#include "stdafx.h"
#include "FormConnect.h"

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

	textHostName_.SetText(tcc_("localhost"));
	textPSW_.SetText(tcc_("rxaa"));

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

		COUT(tcc_("--"));
		df::IntoPtr<FormConnect> formPtr(this);

		//formPtr->ConnectHost(*NewWindow<FormLoad>().Get());

		FormLoad::RunAsync(tcc_("正在连接中..."), this, [=](FormLoad & formLoading){
			formPtr->ConnectHost(formLoading);
		});
	};
}

void FormConnect::ConnectHost(FormLoad & formLoading)
{
	try
	{
		auto con = df::IocpSocket::Connect<MainConnecter, false>(textHostName_.GetText(), G::main.listen_port);
		if (formLoading.IsClosed())
		{
			con->Close();
			return;
		}
		::srand(GetTickCount());
		SS psw = textPSW_.GetText();
		UCHAR key[32];
		Sha2PasswordBuf(psw, key);
		memcpy(con->connectKey, key, 16);
		con->SessionCrypt_.InitByteKey(key);
		con->isClient_ = true;
		con->StartRecvIo();
		Close();
	}
	catch (df::WinException & ex)
	{
		if (formLoading.IsClosed())
			return;

		formLoading.Close();

		MessageERR(cct_("连接失败!\r\n") + ex.message_);
	}

}
