#pragma once


class FormMain :
	public Window
{
public:
	FormMain(void);
	~FormMain(void);

	static FormMain * ptr_;

	Button buttonConnect_;
	Button buttonDisconnect_;
	Button buttionFile_;
	Button buttonCancel_;
	Button buttonCmd_;
	Button buttonProcManag_;
	ListView viewHost_;
	Control hostCount_;

	Tray tary_;

	void OnInit() override;

	void RemoteFile();
	void CommanLine();
	void RemoteProcess();

	void PopHostErrMsg()
	{
		PopMessage(tcc_("没有选择主机!"),500);
	}
};

