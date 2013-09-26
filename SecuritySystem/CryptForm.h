#pragma once


class CryptForm :
	public Window
{
public:
	CryptForm(void);
	~CryptForm(void);

	ImageList imageList_;
	ListView viewFile_;
	Button buttonEncrypt_;
	Button buttonDecrypt_;
	Button buttonBack_;
	Button buttonRefresh_;
	ComBox cryptMode_;
	ComBox diskList_;
	TextBox txtEn_;
	TextBox txtDe_;
	TextBox password_;
	Control progress_;
	Control txtInfo_;

	df::Dir dirList_;
	bool isCrypting_;


	void OnInit() override;

	void ViewDoubleClick(int i);
	void ViewClick(int i);

	//�ϲ�Ŀ¼
	void OnButtonBackClick();

	//��ʾĿ¼����
	void ShowMenuContent(CC menu);

	void OnEncryptClick();
	void OnDecryptClick();

};

