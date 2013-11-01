//测试

SS psw = tcc_("123456");
SS res = tcc_("81A9C3B5E865200888386E00BA3AEA5D");
Sha2Password(psw);
TEST_IS(psw == res);

system("pause");
