;主机串口初始化
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
SEND_ADDR: SETB TB8
;置位IB8，使主机成为发送地址模式
JNB TI, $
;判断发送缓冲区是否为空?
CLR TI
MOV A,#1
MOV SBUF, A ;发送从机地址
;从机检测是否接收到应答信号
CHECK:
JNB RI, $
CLR RI
MOV A, SBUF
CJNE A, #1, FAIL
;收到应答信号不正确
FAIL:AJMP FAIL
;主机发送数据
SEND_DATA: MOV A,#01010101B;(数据)
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
JNB TI,$
CLR TI
CLR TB8
;清零IB8，使主机进入发送数据状态
MOV SBUF, A
CLR ACC.7
RET

END