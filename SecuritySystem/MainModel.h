#pragma once

class MainModel
{
public:

	//文件加密默认目录
	SS file_crypt_init_menu;

	//程序启动时隐藏窗口
	bool hide_window;

	//监听端口
	USHORT listen_port;

	//服务端认证密码,默认rxaa+_)(*&^%$#@!
	SS access_psw;

	META_DATA_INIT;
};

META_DATA_MEMBER_4(MainModel
	, file_crypt_init_menu
	, hide_window
	, listen_port
	, access_psw
	);