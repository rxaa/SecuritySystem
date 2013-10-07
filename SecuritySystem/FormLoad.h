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
	
	template<class LamT>
	static void RunAsync(CC msg, WinHandle win, LamT lam)
	{
		auto loadPtr = NewWindow<FormLoad>();
		loadPtr->contentText_ = msg;

		df::AsyncStart([=]{

			lam(*loadPtr.Get());

			loadPtr->onClose_();
		});

		loadPtr->OpenModal(win.handle_);
	}

};

