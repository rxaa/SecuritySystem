#include "StdAfx.h"
#include "CryptForm.h"

#include "../../df/cryptology/Crypt.h"


CryptForm::CryptForm(void)
{
	resourceID_ = IDD_FILE_CRYP;
	isCrypting_ = false;
}


CryptForm::~CryptForm(void)
{
	G::WriteMain();
}

void CryptForm::OnInit()
{
	txtInfo_.Init(IDC_EDIT4);

	imageList_.Add(IDB_BITMAP2);
	imageList_.Add(IDB_BITMAP3);
	imageList_.Add(IDB_BITMAP4);

	viewFile_.Init(IDC_LIST1);
	viewFile_.AddColumn(cct_("名称"), 388);
	viewFile_.AddColumn(cct_("大小"), 222);
	viewFile_.SetImageList(imageList_);
	viewFile_.onDoubleClick_ = [&](int i){
		ViewDoubleClick(i);
	};
	viewFile_.onSelectChange_ = [&](int i){
		ViewClick(i);
	};

	buttonEncrypt_.Init(IDC_BUTTON1);
	buttonEncrypt_.onClick_ = [&](){
		OnEncryptClick();
	};
	buttonDecrypt_.Init(IDC_BUTTON2);
	buttonDecrypt_.onClick_ = [&](){
		OnDecryptClick();
	};

	buttonEncrypt_.Enable(FALSE);
	buttonDecrypt_.Enable(FALSE);

	buttonBack_.Init(IDC_BUTTON3);
	buttonBack_.onDraw_ = Button::GreenButton;
	buttonBack_.onClick_ = [&]{
		OnButtonBackClick();
	};

	cryptMode_.Init(IDC_COMBO1);
	cryptMode_.Add(cct_("AES"));
	cryptMode_.Add(cct_("Triple-DES"));
	cryptMode_.Add(cct_("DES"));
	cryptMode_.SetSelectIndex(0);

	progress_.Init(IDC_PROGRESS1);

	txtEn_.Init(IDC_EDIT1);
	txtDe_.Init(IDC_EDIT2);
	password_.Init(IDC_EDIT3);

	diskList_.Init(IDC_COMBO2);
	diskList_.onSelectChange_ = [&]{
		ShowMenuContent(diskList_.GetIndexText(diskList_.GetSelectIndex()));
	};


	buttonRefresh_.Init(IDC_BUTTON4);
	buttonRefresh_.onDraw_ = Button::BlueButton;
	buttonRefresh_.onClick_ = [&](){
		ShowMenuContent(diskList_.GetText());
	};

	///获取磁盘列表
	df::EachDisk([&](DiskInfo & disk){
		disk.name_[2] = tt_('\\');
		diskList_.Add(CC(disk.name_, 3));
	});
	//显示上次目录
	diskList_.SetText(G::main.file_crypt_init_menu);
	ShowMenuContent(G::main.file_crypt_init_menu);
}


void CryptForm::ViewDoubleClick(int i)
{
	if (i < 0 || i >= dirList_.listDir_.Size())
		return;

	SS newMenu = diskList_.GetText() + viewFile_.GetText(i, 0) + cct_("\\");

	diskList_.SetText(newMenu);
	ShowMenuContent(newMenu);
}

void CryptForm::ShowMenuContent(CC menu)
{
	G::main.file_crypt_init_menu = menu;
	viewFile_.Clear();

	SS txt;
	if (menu[0] == 0)
	{
		dirList_.Clear();
		df::EachDisk([&](DiskInfo & disk){
			txt = DiskInfo::typeInfo[disk.type_];
			txt.AddByte(disk.freeSize_) << cct_(" / ");
			txt.AddByte(disk.totalSize_);

			viewFile_.AddImageRow(1, disk.name_, txt);
			dirList_.listDir_.Add(cct_(""), 0);
		});
		return;
	}

	dirList_.Get(menu);
	for (auto & dir : dirList_.listDir_)
	{
		viewFile_.AddImageRow(0, dir.name_, cct_(""));
	}

	for (auto & file : dirList_.listFile_)
	{
		viewFile_.AddImageRow(2, file.name_, txt.ClearString().AddByte(file.size_));
	}
}

void CryptForm::OnButtonBackClick()
{
	SS menu = diskList_.GetText();
	int pos = menu.Find(tt_('\\'), menu.Length() - 2, false);
	if (pos > 0)
		menu.SetSize(pos + 1);
	else
		menu.ClearString();

	diskList_.SetText(menu);
	ShowMenuContent(menu);
}

void CryptForm::OnEncryptClick()
{
	int index = viewFile_.GetSelectIndex();
	if (index < 0 || index < dirList_.listDir_.Size())
	{
		PopMessage(cct_("请选择要加密的文件!"));
		return;
	}

	SS psw = password_.GetText();
	if (psw == cct_(""))
	{
		PopMessage(cct_("请输入密码!"));
		password_.SetFocus();
		return;
	}
	SS enFile = txtEn_.GetText();
	if (enFile == cct_(""))
	{
		PopMessage(cct_("请输入生成文件名!"));
		return;
	}
	SS menu = diskList_.GetText();

	SS selectFile = viewFile_.GetText(index, 0);


	df::IntoPtr<CryptForm> winPtr(this);

	buttonEncrypt_.Enable(FALSE);
	buttonDecrypt_.Enable(FALSE);
	df::AsyncStart([=]{
		try
		{
			isCrypting_ = true;
			ON_EXIT({
				isCrypting_ = false;
			});
			winPtr->txtInfo_.SetText(cct_("正在加密:") + selectFile);

			auto lamb = [&](long long fileSize, long long succedSize){
				if (IsClosed())
					Throw(cct_("加密中断!"));

				progress_.SetProgressPos(int(succedSize * 100 / fileSize));
			};


			switch (cryptMode_.GetSelectIndex())
			{
			case 0:
				df::Crypt::EncryptFile<df::CryptMode::AES_CBC>(menu + selectFile, menu + enFile, psw, lamb);
				break;
			case 1:
				df::Crypt::EncryptFile<df::CryptMode::Triple_DES_CBC>(menu + selectFile, menu + enFile, psw, lamb);
				break;
			case 2:
				df::Crypt::EncryptFile<df::CryptMode::DES_CBC>(menu + selectFile, menu + enFile, psw, lamb);
				break;
			default:
				MessageERR(cct_("未知算法:") + cryptMode_.GetText());
				break;
			}

			winPtr->buttonRefresh_.onClick_();
			winPtr->Message(cct_("加密完成!"));
		}
		catch (df::WinException & we)
		{
			MessageERR(we.message_);
		}

		if (IsClosed())
			return;

		buttonEncrypt_.Enable(TRUE);
		buttonDecrypt_.Enable(TRUE);
		txtInfo_.SetText(cct_(""));

	});


}

void CryptForm::OnDecryptClick()
{
	int index = viewFile_.GetSelectIndex();
	if (index < 0 || index < dirList_.listDir_.Size())
	{
		PopMessage(cct_("请选择要解密的文件!"));
		return;
	}

	SS psw = password_.GetText();
	if (psw == cct_(""))
	{
		PopMessage(cct_("请输入密码!"));
		password_.SetFocus();
		return;
	}
	SS enFile = txtDe_.GetText();
	if (enFile == cct_(""))
	{
		PopMessage(cct_("请输入生成文件名!"));
		return;
	}
	SS menu = diskList_.GetText();

	SS selectFile = viewFile_.GetText(index, 0);


	df::IntoPtr<CryptForm> winPtr(this);

	buttonEncrypt_.Enable(FALSE);
	buttonDecrypt_.Enable(FALSE);
	df::AsyncStart([=]{
		try
		{
			isCrypting_ = true;
			ON_EXIT({
				isCrypting_ = false;
			});
			winPtr->txtInfo_.SetText(cct_("正在解密:") + selectFile);
			df::Crypt::DecryptFile(menu + selectFile, menu + enFile, psw
				, [&](long long fileSize, long long succedSize){

					if (IsClosed())
						Throw(cct_("解密中断!"));

					progress_.SetProgressPos(int(succedSize * 100 / fileSize));

			});

			winPtr->buttonRefresh_.onClick_();
			winPtr->Message(cct_("解密完成!"));
		}
		catch (df::WinException & we)
		{
			MessageERR(we.message_);
		}

		if (IsClosed())
			return;

		buttonEncrypt_.Enable(TRUE);
		buttonDecrypt_.Enable(TRUE);
		txtInfo_.SetText(cct_(""));

	});
}

void CryptForm::ViewClick(int i)
{
	if (i < 0 || i < dirList_.listDir_.Size() || isCrypting_)
	{
		buttonEncrypt_.Enable(FALSE);
		buttonDecrypt_.Enable(FALSE);
		txtEn_.SetText(cct_(""));
		txtDe_.SetText(cct_(""));
		return;
	}
	buttonEncrypt_.Enable(TRUE);
	buttonDecrypt_.Enable(TRUE);

	SS file = viewFile_.GetText(i, 0);

	txtEn_.SetText(file + cct_(".ssen"));
	file.Replace(cct_(".ssen"), cct_(""));

	txtDe_.SetText(file);

}
