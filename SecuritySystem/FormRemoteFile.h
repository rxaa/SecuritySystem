#pragma once

class FormRemoteFile :
	public Window
{
public:
	FormRemoteFile(ConnPtr & con);
	~FormRemoteFile();


	Button buttonLocalRet_;
	Button buttonRemoteRet_;
	Button buttonLocalRefresh_;
	Button buttonRemoteRefresh_;
	Button buttonDownload_;
	Button buttonUpload_;
	Button buttonProperty_;
	Button buttonDelete_;

	FileListView viewLocal_;
	ListView viewRemote_;

	ListView viewQueue_;

	ComBox comLocal_;
	ComBox comRemote_;

	TextBox textInfo_;
	Control progress_;

	ConnPtr con_;

	void OnInit() override;
	void OnLayout() override;

	void RemoteDoubleClick(int i);

	void GetRemoteFileList(CC menu);

};

