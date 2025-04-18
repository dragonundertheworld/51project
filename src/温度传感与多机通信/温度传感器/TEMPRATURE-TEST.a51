	  FLAG BIT F0                  ;  声明传感器复位返回的标志位 
      DQ BIT P3.4                  ;  定义DQ 作为数据传输端口名 
      ABIT EQU 35H 				   ;  个
      BBIT EQU 36H 				   ;  十
      CBIT EQU 37H                 ;  百
      ORG 0000H 
      AJMP MAIN 
      ORG 0100H       
;主程序 
MAIN: LCALL RESET          	   ;  复位 
	  LCALL FBLCHANGE       	   ;  改变温度的初始分辨率为9位
	  LCALL GET_TEMP  			   ;  得到温度
	  AJMP CHANGE    			   ;  跳转到数据处理子程序
;复位程序				  	
RESET:SETB DQ    		  		   ;  总线释放 
	  NOP						   ;  保持高电平，延时
	  CLR DQ 			     	   ;  总线置 0，请求响应
	  MOV R0,#0FBH 
								  
TSR1: DJNZ R0,TSR1     	           ;  延时
	  SETB DQ 					   ;  再释放
	  MOV R0,#25H 
TSR2: JNB DQ,TSR3 				   ;  改变为 0，则代表得到对应
	  DJNZ R0,TSR2 		     	   ;  未得到则继续等待，判断
TSR3: SETB FLAG 				   ;  得到相应则标志位置 1，代表传感器正常存在 
	  MOV R0,#06BH 
TSR4: DJNZ R0,TSR4     	           ;  延时
	  SETB DQ 					   ;  释放总线，完成复位 
RET 
 
 
;改变温度分辨率
FBLCHANGE:
	  LCALL RESET				   ;  复位
	  MOV A,#0CCH			  	   ;  跳过ROM
	  LCALL WRITE				   ;  把A写入传感器
	  MOV A,#1FH			       ;  改变温度分辨率为9
	  LCALL WRITE			       ;  把A写入传感器
RET
 
 
;得到温度并转换
GET_TEMP:
	  SETB DQ					   ;  释放总线
	  LCALL RESET				   ;  复位
	  JB FLAG,TSS2				   ;  若传感器不存在，则直接返回主程序
RET
 
TSS2: MOV A,#0CCH
	  LCALL WRITE				   ;  执行跳过ROM指令
	  MOV A,#44H
	  LCALL WRITE  				   ;  执行测温指令
	  LCALL DISPLAY 			   ;  不仅延时，还让程序完全生效前数码管置零
	  LCALL RESET  			   	   ;  复位
	  MOV A,#0CCH 
	  LCALL WRITE  				   ;  执行跳过ROM置零
	  MOV A,#0BEH 
	  LCALL WRITE  				   ;  执行读取温度数据指令
	  LCALL READ 				   ;  跳转至通信子程序
RET 
 
 
									
;写入数据					   	   ;  不可超过120us，否则无法写入
WRITE:MOV R2,#8 				   ;  写入八位二进制码，即循环次数
	  CLR C  				       ;  进位标志位初始置零
WR1:  CLR DQ 					   ;  拉低总线为写入做准备
	  MOV R3,#6 
	  DJNZ R3,$  				   ;  延时
	  RRC A 					   ;  A的最低为给CY，使A从低到高写入从机
	  MOV DQ,C  				   ;  将A又循环写入C，写入总线以输入到传感器
	  MOV R3,#23 
	  DJNZ R3,$  				   ;  延时
	  SETB DQ  				   	   ;  释放，表示此位写入完毕
	  NOP 
	  DJNZ R2,WR1 				   ;  循环八次，一次写入八位
	  SETB DQ  				       ;  释放总线
RET 
 
 
;读出数据
READ: MOV R4,#2 				   ;  读取两个八位数据，外层循环次数
	  MOV R1,#29H 				   ;  立即数寻址给定存储位置
REE0: MOV R2,#8  				   ;  给定数据位数，是内层循环次数
REE1: CLR C  				       ;  进位标志位初始置零
	  SETB C 
	  NOP
	  NOP 
	  CLR DQ 
	  NOP 
	  NOP 
	  NOP 
	  SETB DQ  				  	    ;  输入脉冲并持续2-3个机器周期
	  MOV R3,#7 				   
	  DJNZ R3,$  				    ;  延时，等待传感器响应
	  MOV C,DQ  				    ;  按位读出
	  MOV R3,#23
	  DJNZ R3,$   				    ;  延时
	  RRC A  				   	    ;  把C存入A内
	  DJNZ R2,REE1				    ;  循环8次
	  MOV @R1,A 				    ;  存储A
	  DEC R1 				 	    ;  更换地址
	  DJNZ R4,REE0 				    ;  循环2次
RET
 
 
									
;数据处理函数
CHANGE: 
	  MOV A,29H 
	  MOV 26H,29H
	  MOV 25H,28H 
	  MOV 24H,28H 				    ;  在24H存储原始数据防止丢失
	  MOV C,25H.7 				    ;  存储符号位进C
	  JNC SN1 				 	    ;  判断温度的正负，正数则跳过转补码程序
	  MOV A,25H
	  CPL A  		    			;  取补码，由于无效位置1,25H不必担心数据溢出
	  MOV 25H,A 
	  MOV A,26H 
	  CPL A 
	  INC A  					     ;  由于是末位，需要加一
	  MOV 26H,A 
	  MOV 29H,26H 
	  MOV 28H,25H 				     ;  在26H，25H中操作后放回29H，28H
SN1:  MOV C,28H.0 				     ;  正负温度到此均得到整数部分绝对值
	  RRC A 
	  MOV C,28H.1 
	  RRC A 
	  MOV C,28H.2 
	  RRC A 
	  MOV C,28H.3 				      ;  分别循环，存入A内，连续4次滤掉小数部分
	  RRC A  						  ;  可分析A内八位恰为整数部分（最高位为0）
	  JC SL0 
	  MOV 27H,#00H 
	  AJMP SL5 
SL0:  MOV 27H,#05H 
SL5:  MOV 29H,A 				      ;  为小数部分显示0和5做准备
  	  LCALL DISPLAY 				  ;  转入显示函数
LJMP MAIN  				              ;  返回主函数，程序执行完毕
 
 
;显示函数
DISPLAY:
	  MOV A,29H 
	  MOV B,#10 
	  DIV AB 
	  MOV ABIT,B 
	  MOV B,#10 
	  DIV AB 
	  MOV BBIT,B 
									
     MOV CBIT,A 				      ;  此时可知CBA即为百十个位
	  MOV R0,#4  
	  MOV P2,#21H					  ;  循环显示
DIS0: MOV R1,#250 				      ;  长时间延时循环
DIS1: MOV C,24H.7 
	  JNC DIS2 						  ;  正数转移，负数继续
	  MOV R0,#0EH					  
	  SETB P2.1
	  MOV A,R0
	  MOV P0,A
	  CLR P2.1
	  MOV A,#40H
	  MOV P0,A 
	  LCALL DELAY 
	  AJMP DIS3
DIS2: MOV R0,#0EH   
	  SETB P2.1 
	  MOV A,R0
	  MOV P0,A
	  CLR P2.1 
	  MOV A,#00H
	  MOV P0,A
	  LCALL DELAY  				  	  ;  符号位判断显示
DIS3: MOV A,27H  					  ;  显示小数
	  MOV DPTR,#TAB
	  MOVC A,@A+DPTR 
	  MOV R0,#07H
	  SETB P2.1
	  MOV P0,R0 
	  CLR P2.1
	  MOV P0,A  
	  LCALL DELAY 					  ;  小数位判断显示
DIS4: MOV A,ABIT  					  ;显示个位
	  MOV DPTR,#TAB1
	  MOVC A,@A+DPTR 
	  MOV R0,#0BH 
	  SETB P2.1
	  MOV P0,R0
	  CLR P2.1
	  MOV P0,A  
	  LCALL DELAY 
DIS5: MOV A,BBIT  					  ;显示十位
	  MOV DPTR,#TAB
									
	  MOVC A,@A+DPTR
	  MOV R0,#0DH
	  SETB P2.1
      MOV P0,R0
	  CLR P2.1 
	  MOV P0,A  
	  LCALL DELAY 				 	  	 
RET 
 
 
;延迟函数
DELAY:  MOV R5,#08H
DD:		MOV R6,#0FAH
		DJNZ R6,$
		DJNZ R5,DD
		RET
 
TAB: 				   				  ;  共阴极数码管0-9
	  DB  3FH,06H,5BH,4FH,66H,6DH,7DH,07H,7FH,6FH
TAB1: 				   				  ;  带小数点的个位显示数码0-9
	  DB  0BFH,86H,0DBH,0CFH,0E6H,0EDH,0FDH,87H,0FFH,0EFH
 
 
LOOP:
	  JMP LOOP ;
END