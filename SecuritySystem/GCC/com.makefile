#测试于GCC4.7.2

#源文件目录
cpp_menu=../
#源文件后缀
source_suffix=.cpp
#obj后缀
obj_suffix=.o
#资源文件
res_file=SecuritySystem.rc

VPATH = ../../../window:../../../df:../../../df/cryptology:../../../df/socket
other_objs =$(obj_menu)sha2.o $(obj_menu)des.o $(obj_menu)aes.o $(obj_menu)Window.o $(obj_menu)df.o $(obj_menu)IocpSocket.o 

files =$(wildcard $(cpp_menu)*$(source_suffix))
objects = $(files:$(cpp_menu)%$(source_suffix)=$(obj_menu)%$(obj_suffix))

#链接
$(app_name): $(objects) $(obj_menu)resource.res $(other_objs)
	g++ -o $(app_name) $(objects) $(obj_menu)resource.res $(other_objs) -s -static -l gdi32 -l user32 -l kernel32 -l comctl32 -l msimg32 -l gdiplus -l comctl32 -l Ws2_32 -l ole32  -mwindows

	
all: $(other_objs)
$(other_objs): $(obj_menu)%$(obj_suffix): %$(source_suffix)
	$(compile_param) -c $< -o $@
	
	
#资源
$(obj_menu)resource.res: $(cpp_menu)$(res_file)
	windres  -J rc -O coff -i $(cpp_menu)$(res_file) -o $(obj_menu)resource.res

#编译
all: $(objects)
$(objects): $(obj_menu)%$(obj_suffix): $(cpp_menu)%$(source_suffix)
	$(compile_param) -c $< -o $@