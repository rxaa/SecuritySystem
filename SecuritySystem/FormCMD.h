#pragma once
#include "MainSocket.h"

class FormCMD :
	public Window
{
public:
	FormCMD(ConnPtr & con);
	~FormCMD();

	TextBox textRes_;
	TextBox textCom_;
	Button buttonOk_;

	ConnPtr con_;

	void OnInit() override;

	void SendCmd();
};

