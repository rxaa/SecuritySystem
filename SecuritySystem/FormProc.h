#pragma once

class FormProc :
	public Window
{
public:
	FormProc(ConnPtr & con);
	~FormProc(void);

	Button butOK_;
	ListView viewProc_;

	ConnPtr con_;
	
	void OnInit() override;
};

