#pragma once

//����ָ��
enum Direct
{
	GetHost,
	ResponseHost,
	//Ping,
	//ResponsePing,



	_DirectEnd,
};

class MainConnecter;

class DirectFunc
{
public:
	//ָ���
	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	//ָ�����
	static DirectProcFunc FuncList[Direct::_DirectEnd];


	//��ʼ��ָ���
	static void InitFunc();
};

