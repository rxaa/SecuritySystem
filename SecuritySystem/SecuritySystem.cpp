// SecuritySystem.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "SecuritySystem.h"
#include "../../df/IocpSocket.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	int)
{
	if (::FindWindow(0, t_t("Security System")) != 0)
	{
		df::msg(t_t("������������!"));
		return 0;
	}
	df::Init(hInstance);

	COUT(sizeof(df::IocpConnecter));



	G::ReadMain();
	if (G::main.hide_window)
	{
	}
	else
	{
		Window::InitImage(IDB_BITMAP1);
		//MainForm().Run();
		NewWindow<MainForm>()->Run();
	}

	return 1;
}
