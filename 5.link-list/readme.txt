结构体数组
	长度固定，定义后不可以更改(浪费或不够用)
	数组中的每个元素，地址连续，方便寻址，利于修改数据和查询
	不利于元素数据插入和删除

链表
	长度不固定,可以随意修改长度
	每一个成员(元素)地址不连续(动态链表),寻址需要依赖成员指针
	有利于元素数据插入和删除

【Makefile】
Makefile 文件描述了 Linux 系统下 C/C++ 工程的编译规则
它用来自动化编译 C/C++项目。
一旦写编写好 Makefile 文件，只需要一个 make 命令，整个项目就开始自动编译，
不再需要手动执行 gcc 命令。
Makefile 文件定义了一系列规则，指明了源文件的编译顺序、
依赖关系、是否需要重新编译等。

【Makefile文件名有严格的规定】
只能够命名为Makefile或makefile


基本语法↓
【目标】:【依赖】
	【由依赖生成目标的规则（指令）】
	