#include "StdAfx.h"
#include "FormAttr.h"

const uint FormAttr::AttrList[4] =
{
	FileAttribute::HIDDEN,
	FileAttribute::SYSTEM,
	FileAttribute::READONLY,
	FileAttribute::ARCHIVE,
};


FormAttr::FormAttr(FormRemoteFile & frf)
:formFile_(frf)
{
	resourceID_ = IDD_PROP;
}


FormAttr::~FormAttr(void)
{
}

void FormAttr::OnInit()
{
	butOK_.Init(IDOK);
	butOK_.onClick_ = [&]{
		Close();
	};

	buttonName_.Init(IDC_BUTTON1);
	textTime_.Init(IDC_EDIT2);
	textName_.Init(IDC_EDIT1);
	choice_.AddItem(IDC_CHECK1);
	choice_.AddItem(IDC_CHECK2);
	choice_.AddItem(IDC_CHECK3);
	choice_.AddItem(IDC_CHECK4);

	textName_.SetText(name_);

	formFile_.con_->Send(Direct::GetAttr, menu_ + name_);
}

void FormAttr::ShowAttr(CC attr)
{
	uint at = 0;
	df::StrToVar(attr, at);
	for (int i = 0; i < 4; i++)
	{
		choice_[i].SetCheck(at & AttrList[i]);
	}
}

