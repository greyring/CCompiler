CCompiler：类C语言编译系统

代码挂载于[Github](https://github.com/greyring/CCompiler)

目录：

* c ：代码位置
* doc：参考标准文档
* test：测试代码文件

测试：
    
    进入c文件夹
    执行 make test
    执行 ./test 运行test
    输入 ./test/test.c 测试用c代码
    输出AST

目前已经开发完包括lex，yacc，AST，IR树，指令选择等模块，整体代码还未开发完成，所以现在先提交半成品。



