#include "stdafx.h"
#include "FormLoad.h"


FormLoad::FormLoad()
{
	resourceID_ = IDD_LOAD;
}


FormLoad::~FormLoad()
{
}

void FormLoad::OnInit()
{
	picI_ = 0;
	if (!pic_.Load(IDB_PNG1))
	{
		PopMessage(tcc_("Í¼Æ¬¼ÓÔØÊ§°Ü!"));
	}
	else
	{
		SetTimer(1, 100);
		onClose_ = [&](){
			KillTimer(1);
			Close();
		};
	}
}

void FormLoad::OnPaint()
{
	pic_.DrawAlphaTo(gdi_, 50, 30, 31, 31, 0, picI_ * 31);
	gdi_.Txt(90, 35, contentText_);
}

void FormLoad::OnTimer(uint)
{
	picI_++;
	if (picI_ >= picCount_)
		picI_ = 0;
	OnPaint();
}

void FormLoad::RunAsync(CC msg, WinHandle win, std::function<void(FormLoad &)> form)
{
	df::IntoPtr<FormLoad> loadPtr = NewWindow<FormLoad>();
	loadPtr->contentText_ = msg;
	df::AsyncStart([=]{

		form(*loadPtr.Get());

		loadPtr->onClose_();
	});
	loadPtr->OpenModal(win.handle_);
}
