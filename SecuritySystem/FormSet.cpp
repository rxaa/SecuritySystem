#include "stdafx.h"
#include "FormSet.h"


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
	CheckHide_.Init(IDC_CHECK1);


	TextPort_.SetText(tcc_("") + G::main.listen_port);

	CheckHide_.SetCheck(G::main.hide_window);

}

void FormSet::OnButtonOkClick()
{
	SS port = TextPort_.GetText();
	if (port.Length() == 0)
	{
		PopMessage(tcc_("«Î ‰»Î∂Àø⁄∫≈"),800);
		TextPort_.SetFocus();
		return;
	}

	df::StringToInteger(port, G::main.listen_port);

	G::main.hide_window = CheckHide_.GetCheck();
	G::WriteMainIni();
	Close();
}
