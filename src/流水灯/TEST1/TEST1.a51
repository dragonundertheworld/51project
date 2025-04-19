LED BIT P2.0 //
KEY BIT P1.0 //
ORG 0000H //以 下程序从0000h开始保存
LJMP MAIN //跳转 到main .
ORG 0030H //以 下程序从0030h开始保存
MAIN:
JB KEY, MAIN //KEY是否为高电平
JNB KEY,$ //key为低电平一直等待 !
CPL LED //改变灯 的状态
SJMP MAIN
END