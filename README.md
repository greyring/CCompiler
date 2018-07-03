CCompiler：类C语言编译系统

代码挂载于[Github](https://github.com/greyring/CCompiler)

目录：

* c ：代码位置
* doc：参考标准文档以及报告
* test：测试代码文件

测试：
    
    进入c文件夹
    执行 make test2
    执行 ./test2 运行test
    输入 ../test/test.c 测试用c代码
    输出IR

目前已经开发完包括lex，yacc，AST，IR树，指令选择等模块，整体代码还未整合。



