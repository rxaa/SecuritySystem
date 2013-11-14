#pragma once

class FormRemoteFile;

class FormAttr :
	public Window
{
public:
	FormAttr(FormRemoteFile & frf);
	~FormAttr(void);

	FormRemoteFile & formFile_;
	Button butOK_;
	TextBox textName_;
	Button buttonName_;
	TextBox textTime_;
	ChoiceBox choice_;

	static const uint AttrList[4];

	SS name_;
	SS menu_;
	
	void OnInit() override;
	void ShowAttr(CC attr);
};

