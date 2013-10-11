#pragma once
#include "MainModel.h"
#include "MainSocket.h"



class G
{
private:
	//�����ļ�
	static df::Config mainIni_;
	//Ĭ�ϼ����˿�
	static const int defaultPort_ = 19836;

public:
	//���Ӷ�������ָ��
	typedef df::IntoPtr<MainConnecter> ConnectPtr;

	//�����ӵķ������б�
	static List < ConnectPtr> serverList_;

	//serverList_��
	static df::Lock listLock_;

	//�����ļ�ģ��
	static MainModel main;
	

	//���������˿�
	static SOCKET MainListen()
	{
		DirectFunc::InitFunc();
		return df::IocpSocket::Listen<MainConnecter>(main.listen_port);
	}


	//������
	static bool ReadMainIni()
	{
		bool res = mainIni_.ReadToObject(main);
		if (G::main.listen_port == 0)
			G::main.listen_port = defaultPort_;
		return res;
	}

	//д����
	static bool WriteMainIni()
	{
		return mainIni_.WriteFromObject(main);
	}
};

