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



	void OnInit() override;
	void OnPaint() override;
	void OnTimer(uint id) override;

};

