#pragma once


class FormLoad :
	public Window
{
public:
	FormLoad();
	~FormLoad();

	
	Bitmap pic_;
	//���ڲ��ŵ�ͼƬ����
	int picI_;
	static const int picCount_ = 8;

	SS contentText_;

	void OnInit() override;
	void OnPaint() override;
	void OnTimer(uint id) override;

	static void RunAsync(CC msg, WinHandle win,std::function<void(FormLoad &)>);

};

