#include "StdAfx.h"
#include "FormAttr.h"

const uint FormAttr::AttrList[] =
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
	butOK_.onClick_ = [&] {
		ReAttr();
		Close();
	};

	buttonName_.Init(IDC_BUTTON1);
	textTime_.Init(IDC_EDIT2);
	textName_.Init(IDC_EDIT1);
	choice_.AddItem(IDC_CHECK1);
	choice_.AddItem(IDC_CHECK2);
	choice_.AddItem(IDC_CHECK3);
	choice_.AddItem(IDC_CHECK4);

	buttonName_.onClick_ = [&] {
		Rename();
	};

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

void  FormAttr::Rename()
{
	SS newName = textName_.GetText();
	if (newName.Length() == 0)
	{
		PopMessage(tcc_("请输入文件名!"));
		return;
	}

	SS msg = menu_ + name_ + tcc_("\n") + menu_ + newName;
	formFile_.con_->Send(Direct::ReName, msg);

}

void FormAttr::ReAttr()
{
	uint attr = 0;
	int i = 0;
	for (auto a : choice_)
	{
		attr |= a->GetCheck() * AttrList[i];
		i++;
	}

	SS msg;
	msg << menu_ << name_ << tcc_("\n") << attr;
	formFile_.con_->Send(Direct::ReAttr, msg);
}