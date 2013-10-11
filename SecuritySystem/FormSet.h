#pragma once

class FormSet :
	public Window
{
public:
	FormSet();
	~FormSet();

	Button buttonOK_;
	Button buttonCancel_;
	Control TextPort_;
	Control TextPSW_;
	Control CheckHide_;

	void OnInit() override;

	void OnButtonOkClick();
};

