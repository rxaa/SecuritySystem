#pragma once

class MainModel
{
public:

	//�ļ�����Ĭ��Ŀ¼
	SS file_crypt_init_menu;

	//��������ʱ���ش���
	bool hide_window;

	//�����˿�
	USHORT listen_port;

	SS access_psw;

	META_DATA_INIT;
};

META_DATA_MEMBER_4(MainModel
	, file_crypt_init_menu
	, hide_window
	, listen_port
	, access_psw
	);