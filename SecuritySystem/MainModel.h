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
	
	META_DATA_INIT;
};

META_DATA_MEMBER_3(MainModel, file_crypt_init_menu, hide_window, listen_port);