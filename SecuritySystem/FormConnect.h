#pragma once
#include "FormLoad.h"

class FormConnect :
	public Window
{
public:
	FormConnect();
	~FormConnect();

	Button butOK_;
	Control textHostName_;
	Control textPSW_;

	void OnInit() override;

	//连接指定主机
	void ConnectHost(FormLoad & formLoading);
};

