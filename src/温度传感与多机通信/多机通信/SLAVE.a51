;主机初始化串口.
TEMP EQU R1
S_INIT: MOV SCON, #0D0H
;11110000->SCON
;使能事口收发，并设置工作在模式三
MOV TMOD, #20H
;设置定时器I1为工作模式2
MOV TH1,#0FDH
;波特率设置为9600
MOV TL1,#0FDH
SETB TR1
;开启定时器1

SEND_ADDR1:
SETB TB8
;置位IB8，使主机成为发送地址模式
MOV A,#1
MOV SBUF, A ;发送从机地址
JNB TI, $
;判断发送缓冲区是否为空?
CLR TI

;从机检测是否接收到应答信号
CHECK1:
JNB RI, $;
CLR RI
MOV A, SBUF
CJNE A, #1, FAIL
;收到应答信号不正确

;主机接收数据
RECEIVE1_SIG:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C
JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
;MOV P2,A
MOV R0,A

RECEIVE1_INT:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C
JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
;MOV P2,A
MOV R1,A

RECEIVE1_DECI:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C
JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
;MOV P2,A
MOV R2,A

SEND_ADDR2:
SETB TB8
;置位IB8，使主机成为发送地址模式
MOV A,#2
MOV SBUF, A ;发送从机地址
JNB TI, $
;判断发送缓冲区是否为空?
CLR TI

;从机检测是否接收到应答信号
CHECK2:
JNB RI, $;
CLR RI
MOV A, SBUF
CJNE A, #2, FAIL
;收到应答信号不正确

;主机接收数据
RECEIVE2_SIG:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C

JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
MOV R3,A
RET

RECEIVE2_INT:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C

JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
MOV R4,A
RET

RECEIVE2_DECI:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C

JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
MOV R5,A
RET

ERROR: 
MOV P1,#11110000B
AJMP $
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零

FAIL:
MOV P2,#00000001B
AJMP FAIL
END
