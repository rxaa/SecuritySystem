#include "stdafx.h"
#include "FormSet.h"


const static CC regVal = tcc_("SecuritySystem");

FormSet::FormSet()
{
	resourceID_ = IDD_SET;
}


FormSet::~FormSet()
{

}

void FormSet::OnInit()
{
	buttonOK_.Init(IDOK);
	buttonOK_.onClick_ = [&](){
		OnButtonOkClick();
	};

	buttonCancel_.Init(IDCANCEL);
	buttonCancel_.onDraw_ = Button::RedButton;

	TextPort_.Init(IDC_EDIT1);
	TextPSW_.Init(IDC_EDIT2);
	choice_.AddItem(IDC_CHECK1);
	choice_.AddItem(IDC_CHECK2);
	choice_.onClick_ = [&](int i){
		if (i == 1)
			OnAutoStartChoice();
	};


	TextPort_.SetText(tcc_("") + G::main.listen_port);

	choice_[0].SetCheck(G::main.hide_window);

	
	reg_.EachKeyValue([&](df::RegKeyValue & key){

		if (key.type == df::Reg::KeyString && key.key == regVal)
		{
			choice_[1].SetCheck(TRUE);
			return false;
		}
		return true;
	});

}

void FormSet::OnButtonOkClick()
{
	SS port = TextPort_.GetText();
	if (port.Length() == 0)
	{
		PopMessage(tcc_("请输入端口号"),800);
		TextPort_.SetFocus();
		return;
	}

	df::StringToInteger(port, G::main.listen_port);

	G::main.hide_window = choice_[0].GetCheck();
	G::WriteMainIni();
	Close();
}

void FormSet::OnAutoStartChoice()
{
	if (choice_[1].GetCheck())
	{
		if (!reg_.SetValue(regVal, df::GetExeMenu()))
			PopMessage(tcc_("开机自启失败!"));
	}
	else
	{
		reg_.DeleteValue(regVal);
	}
}
