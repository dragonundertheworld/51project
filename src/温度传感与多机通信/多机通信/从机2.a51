FLAG BIT F0                  ;  声明传感器复位返回的标志位 
      DQ BIT P3.4                ;  定义DQ 作为数据传输端口名 
      ORG 0000H 
      AJMP MAIN 
      ORG 0100H       
;主程序 
MAIN: LCALL RESET              ;  复位 
   LCALL FBLCHANGE           ;  改变温度的初始分辨率为9位
   LCALL GET_TEMP        ;  得到温度
   LCALL CHANGE          ;  跳转到数据处理子程序
   LJMP S_INIT
;复位程序       
RESET:SETB DQ             ;  总线释放 
   NOP         ;  保持高电平，延时
   CLR DQ             ;  总线置 0，请求响应
   MOV R0,#0FBH    
TSR1: DJNZ R0,TSR1                 ;  延时
   SETB DQ         ;  再释放
   MOV R0,#25H 
TSR2: JNB DQ,TSR3        ;  改变为 0，则代表得到对应
   DJNZ R0,TSR2            ;  未得到则继续等待，判断
TSR3: SETB FLAG        ;  得到相应则标志位置 1，代表传感器正常存在 
   MOV R0,#07FH 
TSR4: DJNZ R0,TSR4                 ;  延时
   SETB DQ         ;  释放总线，完成复位 
RET 
 
 
;改变温度分辨率
FBLCHANGE:
   LCALL RESET       ;  复位
   MOV A,#0CCH         ;  跳过ROM
   LCALL WRITE       ;  把A写入传感器
   MOV A,#3FH          ;  改变温度分辨率为9
   LCALL WRITE          ;  把A写入传感器
RET
 

;得到温度并转换
GET_TEMP:
   SETB DQ        ;  释放总线
   LCALL RESET       ;  复位
   JB FLAG,TSS2       ;  若传感器不存在，则直接返回主程序
RET
 
TSS2: MOV A,#0CCH
   LCALL WRITE       ;  执行跳过ROM指令
   MOV A,#44H
   LCALL WRITE         ;  执行测温指令
   MOV R3,#0FFH 
   DJNZ R3,$         ;  延时
   LCALL RESET            ;  复位
   MOV A,#0CCH 
   LCALL WRITE         ;  执行跳过ROM置零
   MOV A,#0BEH 
   LCALL WRITE         ;  执行读取温度数据指令
   LCALL READ        ;  跳转至通信子程序
RET 
 
 
         
;写入数据            ;  不可超过120us，否则无法写入
WRITE:MOV R2,#8        ;  写入八位二进制码，即循环次数
   CLR C             ;  进位标志位初始置零
WR1:  CLR DQ         ;  拉低总线为写入做准备
   MOV R3,#5
   DJNZ R3,$         ;  延时
   RRC A         ;  A的最低位给CY，使A从低到高写入从机
   MOV DQ,C         ;  将A又循环写入C，写入总线以输入到传感器
   MOV R3,#25
   DJNZ R3,$         ;  延时
   SETB DQ             ;  释放，表示此位写入完毕
   NOP
   NOP   
   DJNZ R2,WR1        ;  循环八次，一次写入八位
   SETB DQ             ;  释放总线
RET 
 
 
;读出数据
READ: MOV R4,#2        ;  读取两个八位数据，外层循环次数
   MOV R1,#29H        ;  立即数寻址给定存储位置
REE0: MOV R2,#8         ;  给定数据位数，是内层循环次数
REE1: CLR C             ;  进位标志位初始置零
   SETB C 
   NOP
   NOP 
   CLR DQ 
   NOP 
   NOP 
   NOP
   SETB DQ             ;  输入脉冲并持续2-3个机器周期
   MOV R3,#7   
   DJNZ R3,$          ;  延时，等待传感器响应
   MOV C,DQ          ;  按位读出
   MOV R3,#45
   DJNZ R3,$           ;  延时
   RRC A              ;  把C存入A内
   DJNZ R2,REE1        ;  循环8次
   MOV @R1,A         ;  存储A
   DEC R1           ;  更换地址
   DJNZ R4,REE0         ;  循环2次
RET
 
 
        
;数据处理函数
CHANGE: 
   MOV A,29H 
   MOV 26H,29H
   MOV 25H,28H 
   MOV 24H,28H         ;  在24H存储原始数据防止丢失
   MOV C,25H.7         ;  存储符号位进C
   JNC SN1           ;  判断温度的正负，正数则跳过转补码程序
   MOV A,25H
   CPL A           ;  取补码，由于无效位置1,25H不必担心数据溢出
   MOV 25H,A 
   MOV A,26H 
   CPL A 
   INC A            ;  由于是末位，需要加一
   MOV 26H,A 
   MOV 29H,26H 
   MOV 28H,25H          ;  在26H，25H中操作后放回29H，28H
   MOV 30H,#01H       ;在30H中保存符号信息
SN1:  MOV C,28H.0          ;  正负温度到此均得到整数部分绝对值
   RRC A 
   MOV C,28H.1 
   RRC A 
   MOV C,28H.2 
   RRC A 
   MOV C,28H.3           ;  分别循环，存入A内，连续4次滤掉小数部分
   RRC A          ;  可分析A内八位恰为整数部分（最高位为0）
   JC SL0 
   MOV 27H,#00H 
   AJMP SL5 
SL0:  MOV 27H,#05H 
SL5:  MOV 29H,A           ;  为小数部分显示0和5做准备
MOV P1,A
RET
;LJMP MAIN                    ;  返回主函数，程序执行完毕
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
CJNE A, #1,CHECK ;判断发送的地址是否 与本机地址相等，address(这里是1)表示本机地址
;若不相等则继续等待，不做处理


CLR SM2
;将SM2位清零
;从机应答

ACK0:
SETB TB8
MOV SBUF,#1;向主机发送应答信号，即发回本机地址(1)给主机
JNB TI, $;trasmission
;判断发送缓冲区是否为空?
CLR TI
;清零发送中断标志II

TRASMIT_DATA: 
;MOV 29H,#01011101B
MOV A,29H;(数据)
MOV P2,A
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI
;清零TB8，使从机进入发送数据状态
CLR ACC.7

CHECK1:
JNB RI, $
;若RI等于0，则继续等待
CLR RI
;清除接收中断标志RI
MOV A,SBUF
;接受主机发来的地址信息
CJNE A, #1,CHECK1 ;判断发送的地址是否 与本机地址相等，address(这里是1)表示本机地址
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零
;从机应答

ACK1:
SETB TB8
MOV SBUF,#1;向主机发送应答信号，即发回本机地址(1)给主机
JNB TI, $;trasmission
;判断发送缓冲区是否为空?
CLR TI
;清零发送中断标志II

;MOV 27,#05H
MOV A,27H;(数据)
MOV P2,A
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI
;清零TB8，使从机进入发送数据状态
CLR ACC.7

CHECK2:
JNB RI, $
;若RI等于0，则继续等待
CLR RI
;清除接收中断标志RI
MOV A,SBUF
;接受主机发来的地址信息
CJNE A, #1,CHECK2 ;判断发送的地址是否 与本机地址相等，address(这里是1)表示本机地址
;若不相等则继续等待，不做处理
CLR SM2
;将SM2位清零
;从机应答

ACK2:
SETB TB8
MOV SBUF,#1;向主机发送应答信号，即发回本机地址(1)给主机
JNB TI, $;trasmission
;判断发送缓冲区是否为空?
CLR TI
;清零发送中断标志II

;MOV 30H,#02H
MOV A,30H;(数据)
MOV P2,A
MOV C, P
CPL C
MOV ACC.7, C;生成最终要 发送的数据到A
CLR TB8
MOV SBUF, A
JNB TI,$
CLR TI
;清零TB8，使从机进入发送数据状态
CLR ACC.7

LJMP MAIN
;LJMP S_INIT
END
