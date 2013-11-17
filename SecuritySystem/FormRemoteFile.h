#pragma once
#include "FormAttr.h"

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
	df::IntoPtr<FormAttr> attrPtr_;

	int remoteDirCount_ = 0;

	void FormClose();

	void OnInit() override;
	void OnLayout() override;

	void RemoteMenuBack();
	void RemoteDoubleClick(int i);


	void GetRemoteFileList(CC menu);

	void DownloadFile();

	void UploadFile();

	void DelFile();

	void FileAttr();

	void OnTransfer(FileConnect * fileCon);
	void OnCompleted(FileConnect * fileCon);
	void OnError(FileConnect * fileCon,CC code,CC msg);

	//文件传输状态信息
	SS tranState_;
	//记录时间间隔
	long timeTick_ = 0;
	//时间间隔内传输长度
	int64_t succedByte_ = 0;
};

