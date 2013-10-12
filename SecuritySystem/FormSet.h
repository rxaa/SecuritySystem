#pragma once
#include "../../window/Reg.h"

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
	ChoiceBox choice_;

	df::Reg reg_;

	void OnInit() override;

	void OnButtonOkClick();
	void OnAutoStartChoice();
};

