#pragma once

class FormProc :
	public Window
{
public:
	FormProc(ConnPtr & con);
	~FormProc(void);

	Button butOK_;
	Button buttonRsfresh_;
	ListView viewProc_;
	Control textCount_;

	ConnPtr con_;
	
	void OnInit() override;
};

