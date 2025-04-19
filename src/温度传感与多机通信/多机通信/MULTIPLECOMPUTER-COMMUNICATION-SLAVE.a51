;从机初始化事口.
S_INIT: MOV SCON, #0F0H
;11110000->SCON
;使能事口收发，并设置工作在模式三
MOV TMOD, #20H
;设置定时器I1为工作模式2
MOV TH1,#0FDH
;波特率设置为9600
MOV TL1,#0FDH
SETB TR1
;开启定时器1
;从机检测是否收到主机请求
CHECK:
JNB RI, $
;若RI等于0，则继续等待
CLR RI
;清除接收中断标志RI
MOV A,SBUF
;接受主机发来的地址信息
CJNE A, #1,CHECK ;判断发送的地址是否 与本机地址相等，address(这里是1)表示本机地址
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零
;从机应答
ACK:
JNB TI, $;trasmission
;判断发送缓冲区是否为空?
CLR TI
;清零发送中断标志II
MOV SBUF,#1;向主机发送应答信号，即发回本机地址(1)给主机
;从机接收主机数据(采用奇校验方式)
RECEIVE:
JNB RI,$
CLR RI
MOV A, SBUF
MOV C,P;将奇偶标志位传给C
JNC ERROR;若c=0，则传输出错，转至出错处理
CLR ACC.7
;清除A中最高位做奇偶校验位
RET
ERROR: AJMP ERROR
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零
MOV P2,A
END