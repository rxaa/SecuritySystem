#pragma once
#include "MainModel.h"
#include "MainSocket.h"



class G
{
public:
	//Ĭ�ϼ����˿�
	static const int defaultPort_ = 19836;

	//�����ļ�
	static const SS & IniFile()
	{
		static SS name = df::GetProgressDir() + tcc_("SecuritySystemMain.ini");
		return name;
	}

	//���Ӷ�������ָ��
	typedef df::IntoPtr<MainConnecter> ConnectPtr;

	//�����ӵķ������б�
	static List < ConnectPtr> serverList_;

	//serverList_��
	static df::Lock listLock_;

	//�����ļ�ģ��
	static MainModel main;
	

	//���������˿�
	inline static SOCKET MainListen()
	{
		DirectFunc::InitFunc();
		return df::IocpSocket::Listen<MainConnecter>(main.listen_port);
	}


	//������
	inline static bool ReadMainIni()
	{
		bool res = df::Config::Read(IniFile(), main);
		if (G::main.listen_port == 0)
			G::main.listen_port = defaultPort_;
		return res;
	}

	//д����
	inline static bool WriteMainIni()
	{
		return df::Config::Write(IniFile(), main);
	}
};

