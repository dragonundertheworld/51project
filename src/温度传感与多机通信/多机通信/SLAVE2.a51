DECI EQU 27H
INT EQU 29H
SIG EQU 30H
;从机串口初始化
S_INIT: MOV SCON,#0F0H
;11110000->5C0N
;使能串口收发，并设置工作在模式三
MOV TMOD, #20H
;设置定时器11为工作模式2
MOV TH1,#0FDH
;波特率设置为9600
MOV TL1,#0FDH
SETB TR1
;主机发送地址
CHECK:
JNB RI, $
;若RI等于0，则继续等待
CLR RI
;清除接收中断标志RI
MOV A,SBUF
;接受主机发来的地址信息
CJNE A, #2,CHECK ;判断发送的地址是否 与本机地址相等，address(这里是1)表示本机地址
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零
;从机应答

ACK1:
SETB TB8
MOV SBUF,#2;向主机发送应答信号，即发回本机地址(1)给主机
JNB TI, $;trasmission
;判断发送缓冲区是否为空?
CLR TI
;清零发送中断标志II


TRASMIT_DATA_SIG: 
MOV A,#00101011B;(数据)
MOV P2,A

MOV A,SIG
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI
CLR ACC.7

TRASMIT_DATA_INT: 
MOV A,INT
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI

CLR ACC.7

TRASMIT_DATA_DECI: 
MOV A,DECI
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI

CLR ACC.7
RET

END