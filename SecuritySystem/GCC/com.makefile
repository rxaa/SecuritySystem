#������GCC4.7.2

#Դ�ļ�Ŀ¼
cpp_menu=../
#Դ�ļ���׺
source_suffix=.cpp
#obj��׺
obj_suffix=.o
#��Դ�ļ�
res_file=SecuritySystem.rc

VPATH = ../../../window:../../../df:../../../df/cryptology:../../../df/socket
other_objs =$(obj_menu)sha2.o $(obj_menu)des.o $(obj_menu)aes.o $(obj_menu)Window.o $(obj_menu)df.o $(obj_menu)IocpSocket.o 

files =$(wildcard $(cpp_menu)*$(source_suffix))
objects = $(files:$(cpp_menu)%$(source_suffix)=$(obj_menu)%$(obj_suffix))

#����
$(app_name): $(objects) $(obj_menu)resource.res $(other_objs)
	g++ -o $(app_name) $(objects) $(obj_menu)resource.res $(other_objs) -s -static -l gdi32 -l user32 -l kernel32 -l comctl32 -l msimg32 -l gdiplus -l comctl32 -l Ws2_32 -l ole32  -mwindows

	
all: $(other_objs)
$(other_objs): $(obj_menu)%$(obj_suffix): %$(source_suffix)
	$(compile_param) -c $< -o $@
	
	
#��Դ
$(obj_menu)resource.res: $(cpp_menu)$(res_file)
	windres  -J rc -O coff -i $(cpp_menu)$(res_file) -o $(obj_menu)resource.res

#����
all: $(objects)
$(objects): $(obj_menu)%$(obj_suffix): $(cpp_menu)%$(source_suffix)
	$(compile_param) -c $< -o $@