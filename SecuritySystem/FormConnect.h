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

	//����ָ������
	void ConnectHost(FormLoad & formLoading);
};

