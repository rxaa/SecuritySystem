#pragma once

class FormRemoteFile :
	public Window
{
public:
	FormRemoteFile();
	~FormRemoteFile();

	Button buttonLocalRet_;
	Button buttonRemoteRet_;
	Button buttonLocalRefresh_;
	Button buttonRemoteRefresh_;
	Button buttonDownload_;
	Button buttonUpload_;
	Button buttonProperty_;
	Button buttonDelete_;

	ListView viewLocal_;
	ListView viewRemote_;

	ListView viewQueue_;

	ComBox comLocal_;
	ComBox comRemote_;

	TextBox textInfo_;
	Control progress_;

	void OnInit() override;
	void OnLayout() override;
};

