#pragma once

#include "../../df/socket/IocpSocket.h"
#include "../../df/cryptology/Crypt.h"
#include "../../df/cmd.h"

struct FileConnect;

#include "FormCMD.h"
#include "FormRemoteFile.h"
#include "FormProc.h"

void Sha2PasswordBuf(SS & psw, unsigned char sha2Res[32]);
void Sha2Password(SS & psw);


///文件传输连接对象内容
struct FileConnect
{
	//文件传输缓冲 = RecvIo_缓冲长-包头长
	const static int  FileBufferSize = 4096 - 16 * 2;
	//传输状态
	enum
	{
		StateNone,//无
		StateDownload,//正在下载
		StateUpload,//正在上传
	};
	//文件句柄
	df::FileBin file_;
	//传输状态
	int state = StateNone;
	//文件已传输长度(通过初始化此值来进行断点续传)
	int64_t transferedSize_ = 0;
	//源文件名
	SS fileNameFrom_;
	//目标文件名
	SS fileNameTo_;
	//文件传输缓冲
	char buf_[FileBufferSize];
	//是否已投递RecvIO
	bool hasRecvIo_ = false;

	//回调
	//每传输FileBufferSize长度后的回调
	std::function < void()> onTransfer_;
	//文件传输完成
	std::function < void()> onCompleted_;
	//出现错误(错误码,信息)
	std::function < void(CC, CC)> onError_;

	void WriteErrLog(const CC & code ,const CC & msg)
	{
		df::WriteLog(tcc_("code = ") + code + tcc_("\r\nmsg = ") + msg + tcc_("\r\n状态:") + state + tcc_("\r\nfileNameFrom_ = ")
			+ fileNameFrom_ + tcc_("\r\nfileNameTo_ = ") + fileNameTo_ + tcc_("\r\ntransferedSize_ = ") + transferedSize_);
	}

	FileConnect()
		: onTransfer_([](){})
		, onCompleted_([](){})
		, onError_([&](const CC & code, const CC & msg){WriteErrLog(code, msg); })
	{
	}

	void Clear()
	{
		state = StateNone;
		file_.Close();


		transferedSize_ = 0;
	}

	void Release()
	{
		onTransfer_ = [](){};
		onCompleted_ = [](){};
		onError_ = [&](CC code, CC msg){
			WriteErrLog(code, msg);
		};
	}

	DISABLE_COPY_ASSIGN(FileConnect);
};


class MainConnecter
	: public df::IocpConnecter
{
public:

	MainConnecter(){}
	~MainConnecter();

	//认证口令
	static const uint16_t verifyWord_ = 65535;

	//会话密钥前2个字节
	static const uint16_t verifyPsw_ = 12345;

	//包头长度:4字节包长度,1字节末尾补0,2字节指令代号......4字节认证口令
	static const uint32_t headerSize_ = 4 + 1 + 2 + 4;
	static const uint32_t packageHeaderSize_ = 1 + 2 + 4;
	static const uint32_t uncryptHeaderSize_ = 4;

	bool hasSessionKey_ = false;
	bool isClient_ = false;

	//cmd命令
	std::unique_ptr<df::cmd> cmd_;

	//认证密钥
	uint8_t connectKey[16];

	//文件传输数据
	std::unique_ptr<FileConnect> file_;

	//文件传输连接对象
	ConnPtr fileConnect_;

	//dos命令窗口
	FormCMD * formCmdPtr_ = nullptr;
	//远程文件管理窗口
	FormRemoteFile * formFile_ = nullptr;

	FormProc * formProc_ = nullptr;

	//会话加密
	df::CryptAlg <df::CryptMode::AES_CBC> SessionCrypt_;

	//认证加密
	static df::CryptAlg <df::CryptMode::AES_CBC> VerifyCrypt_;

	//初始化认证密钥
	static void InitVerifyKey();

	void OnConnect() override;
	void OnRecv(char *, uint) override;
	void OnClosed() override;
	bool Send(uint16_t directive, const void * msg, uint len, df::IocpOverlap ** overlapIO = nullptr);
	bool Send(uint16_t directive)
	{
		return Send(directive, nullptr, 0);
	}

	bool Send(uint16_t directive, const CC & str)
	{
		return Send(directive, str.char_, (str.length_ + 1) * sizeof(TCHAR));
	}

	void OnSend(df::IocpOverlap *& io) override;

	//新建文件传输连接(fileConnect_),失败抛df::WinException异常,传输结束后不会自动关闭此连接,以便再次复用
	FileConnect * InitTransferFile();
	//开始下载,失败抛df::WinException异常
	bool StartDownloadFile();

	//开始上传,失败抛df::WinException异常
	bool StartUploadFile();

	//*******************************************
	// Summary : 发送下载文件命令
	// Parameter - int64_t pos : 断点
	// Parameter - const CC & fileName : 文件
	// Returns - bool : 
	//*******************************************
	bool SendDownloadFile(int64_t pos, const CC & fileName);


	//*******************************************
	// Summary : 发送文件上传命令
	// Returns - bool : 
	//*******************************************
	bool SendUploadFile();

	//*******************************************
	// Summary : 文件传输出错
	// Parameter - const CC & err : 错误信息:...\n....
	// Returns - bool : 
	//*******************************************
	bool SendTrandsferError(const CC & err);

	bool IsTransfering()
	{
		return fileConnect_ && fileConnect_->file_ &&  fileConnect_->file_->state != FileConnect::StateNone;
	}

	template<class Lamb>
	bool SendMsg(uint16_t directive, uint len, Lamb lam, df::IocpOverlap ** overlapIO = nullptr)
	{
		//COUT(tcc_("Send") << len);
		MY_ASSERT(directive < Direct::_DirectEnd);
		MY_ASSERT(hasSessionKey_);

		if (len > df::IocpOverlap::MAX_PACKAGE_SIZE)
		{
			COUT(tcc_("IocpConnecter::Send非法参数") << len);
			BREAK_POINT_MSG("IocpConnecter::Send非法参数");
			return false;
		}

		if (sock_ == 0)
		{
			return false;
		}

		//计算末尾补0数
		uint8_t footZero = (len + packageHeaderSize_) % SessionCrypt_.KeySize;
		if (footZero > 0)
		{
			footZero = SessionCrypt_.KeySize - footZero;
		}


		//新长度=包内容长+包头长+补0
		uint newSize = len + headerSize_ + footZero;
		df::IocpOverlap * io;

		//如果参数提供了overlapIO且大小合适,则复用
		if (overlapIO && (*overlapIO)->bufSize_ >= newSize)
		{
			io = *overlapIO;
			*overlapIO = nullptr;
		}
		else
		{
			io = df::IocpOverlap::New(newSize, this);
		}


		//COUT(tcc_("df::IocpOverlap::New ") << newSize);
		uint8_t * hp = (uint8_t *)io->buffer_;

		//4字节包长度
		*(uint32_t*)hp = newSize - uncryptHeaderSize_;
		hp += 4;

		//1字节补0
		*hp = footZero;
		hp++;

		//2字节指令码
		*(uint16_t*)hp = directive;
		hp += 2;

		//包内容;
		lam(hp);
		//memcpy(hp, msg, len);
		hp += len;

		//补0
		memset(hp, 0, footZero);
		hp += footZero;

		//4字节认证指令
		uint16_t word1 = (uint16_t)rand();
		uint16_t word2 = verifyWord_ ^ word1;
		((uint16_t*)hp)[0] = word1;
		((uint16_t*)hp)[1] = word2;

		MY_ASSERT((newSize - uncryptHeaderSize_) % 16 == 0);

		//从补0处开始加密
		SessionCrypt_.Encrypt(io->buffer_ + uncryptHeaderSize_, io->buffer_ + uncryptHeaderSize_, newSize - uncryptHeaderSize_);

		return SendIocpOverlap(io, newSize);
	}
};



