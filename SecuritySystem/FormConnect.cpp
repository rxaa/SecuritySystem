#include "stdafx.h"
#include "FormConnect.h"
#include "FormLoad.h"

FormConnect::FormConnect()
{
	resourceID_ = IDD_CONNECT;
}


FormConnect::~FormConnect()
{
}

void FormConnect::OnInit()
{
	butOK_.Init(IDOK);
	butOK_.onDraw_ = Button::GreenButton;
	butOK_.onClick_ = [&]{
		NewWindow < FormLoad>()->OpenModal(this);
	};
}
