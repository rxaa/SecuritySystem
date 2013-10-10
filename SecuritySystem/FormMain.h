#pragma once


class FormMain :
	public Window
{
public:
	FormMain(void);
	~FormMain(void);

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
};

