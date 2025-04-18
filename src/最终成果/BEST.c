#include<reg52.h>
#include<intrins.h>
#include<absacc.h>		//头文件


#define uc unsigned char
#define ui unsigned int	
	//引脚定义
#define LCD P2

sbit RS = P0^1;
sbit RW = P0^2;
sbit E = P0^3;


sbit in_forward		=	P1^2;
sbit in_backward	=	P1^1;//手动前进后退按键 
sbit out_forward		=	P0^5;
sbit out_backward	=	P0^6;//电机控制输出 
sbit limit_forward	=	P1^3;
sbit limit_backward	=	P1^4;//前进后退限位开关 
sbit led_forward		=	P1^6;
sbit led_backward	=	P1^5;//前进后退限位指示灯 
sbit light		=	P3^7;//光线传感器 
sbit change		=	P1^0;//手动自动切换按键（默认自动）  
sbit led_zhishi =	P1^7;
sbit beep = P0^0;	 //定义蜂鸣器的引脚

sbit W1=P2^2;
sbit W2=P2^1;
//定义DHT11数据口
sbit DATA = P0^7;
uc U8FLAG,U8temp,U8comdata,U8RH_data_H_temp,U8RH_data_L_temp,U8T_data_H_temp,U8T_data_L_temp,U8checkdata_temp,string[]={""};
uc U8RH_data_H,U8RH_data_L,U8T_data_H,U8T_data_L,U8checkdata,DataGet;
ui Mode,i = 0;
bit bdata FlagStartRH,hot,cold,humid,dry;

bit bdata flag1,flag2,change_flag,flag_shan,flag_shan1,flag_shan2,flag_HH;
uc m,n,shan;


//定时器0初始化
void Timer0_Init()
{
	ET0 = 1;        //允许定时器0中断
	TMOD = 0x21;       //定时器工作方式选择
	TL0 = 0xb0;     
	TH0 = 0x3c;     //定时器赋予初值
	TR0 = 1;        //启动定时器
	EA=1;
}


void Init()
{
	//定时器1初始化
	TMOD=0x21;//M1:M0=10 定时器0工作方式2, 可重置8位
	
	IT1=0;//低电平
	ET1=0;//中断允许
	TR1=1;//开启定时器计数
	
	TH1=0xfd;
	TL1=0xfd;//设置定时器初值
//	TH1=0xFF;
//	TL1=0xE8;
	PCON=0x00;//SCON=0
	
	//串口初始化
	SM0=0;
	SM1=1;//串口工作方式1 8位UART
	REN=1;//使能串行接收
	ES=1;//串口中断允许
	EA=1;//开启总中断允许位

}
void SendData(int humidity)
{
	int i=0;
	unsigned char gewei,shiwei,array1[]={"HUMIDITY:  "};
	gewei=humidity%10+'0';
	shiwei=humidity/10+'0';
	array1[9]=shiwei;
	array1[10]=gewei;

	while(array1[i] != '\0')
	{	
		SBUF=array1[i];
		while(!TI);//若没有发送完数据 等待
		TI=0;//若发送完 标志位清0
		i++;
	}

}
void SendData2(int humidity)
{
	int i=0;
	unsigned char gewei,shiwei,array1[]={" "};
	gewei=humidity%10+'0';
	shiwei=humidity/10+'0';
	array1[0]=shiwei;
	array1[1]=gewei;

	while(array1[i] != '\0')
	{	
		SBUF=array1[i];
		while(!TI);//若没有发送完数据 等待
		TI=0;//若发送完 标志位清0
		i++;
	}

}

void Delay1(ui j)
{
    uc i;
    for(;j>0;j--)
    { 	
		for(i=0;i<27;i++);
    }
} 

void  Delay_10us(void)
{
    uc i;
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}	



/******************************************************************************
 * @ 函数名  ： Delay_10us
 * @ 功  能  ： 10us粗略延时
 * @ 参  数  ： 延时时间--单位10us
 * @ 返回值  ： 无
 ******************************************************************************/
void Delay_10us_1(unsigned int time)
{
	while(time--);
}
 
 /******************************************************************************
 * @ 函数名  ： Simulate_Pwm
 * @ 功  能  ： 模拟PWM控制（需要不停运行）
 * @ 参  数  ： period 周期（单位10us）
 *              duty   占空比（低电平，0-100）
 * @ 返回值  ： 无
 ******************************************************************************/
void Simulate_Pwm(unsigned char period, unsigned char duty)
{
	int i;
	unsigned char low = period / 100 * duty; //低电平延时时间
	unsigned char high = period - low;       //高电平延时时间
	for(i=0;i<=500;i++)
	{
	beep = 0;        //蜂鸣器IO电平置低
	Delay_10us_1(low); //低电平延时时间
	beep = 1;        //蜂鸣器IO电平置高
	Delay_10us_1(high);//高电平延时时间
	}
}

/******************************************************************************
 * @ 函数名  ： main
 * @ 功  能  ： 主函数
 * @ 参  数  ： 无
 * @ 返回值  ： 无
 ******************************************************************************/

void buzz(ui number)
{
	if (number==1)
	{
			Simulate_Pwm(100, 99);  //周期100*10us，占空比50%——频率：1KHz
		  beep=0;
	}
	else
	{
		beep=0;
	}
}

void  COM(void)
{
    uc i;
    for(i=0;i<8;i++)	   
    {
  	    U8FLAG=2;
	   	while((!DATA)&&U8FLAG++);
		Delay_10us();
		Delay_10us();
		Delay_10us();
  		U8temp=0;
        if(DATA)U8temp=1;
		    U8FLAG=2;
		while((DATA)&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		U8comdata<<=1;
	   	U8comdata|=U8temp;        //0
    }//rof
}

/*--------------------------------
-----温湿度读取子程序 ------------
--------------------------------
----以下变量均为全局变量--------
----温度高8位== U8T_data_H------
----温度低8位== U8T_data_L------
----湿度高8位== U8RH_data_H-----
----湿度低8位== U8RH_data_L-----
----校验 8位 == U8checkdata-----
----调用相关子程序如下----------
---- Delay();, Delay_10us();,COM(); 
--------------------------------*/
uc RH(void)
{
    //主机拉低18ms 
    DATA=0;
	Delay1(180);  //原来为5
	DATA=1;
	//总线由上拉电阻拉高 主机延时20us
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	//主机设为输入 判断从机响应信号 
	DATA=1;
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(!DATA)		 //T !	  
	{
	    U8FLAG=2;
	    //判断从机是否发出 80us 的低电平响应信号是否结束	 
	    while((!DATA)&&U8FLAG++);
	    U8FLAG=2;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((DATA)&&U8FLAG++);
	    //数据接收状态		 
	    COM();
	    U8RH_data_H_temp=U8comdata;
	    COM();
	    U8RH_data_L_temp=U8comdata;
	    COM();
	    U8T_data_H_temp=U8comdata;
	    COM();
	    U8T_data_L_temp=U8comdata;
	    COM();
	    U8checkdata_temp=U8comdata;
	    DATA=1;
	    //数据校验 
	    U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	    if(U8temp==U8checkdata_temp)
	    {
	   	   U8RH_data_H=U8RH_data_H_temp;
	   	   U8RH_data_L=U8RH_data_L_temp;
	   	   U8T_data_H=U8T_data_H_temp;
	   	   U8T_data_L=U8T_data_L_temp;
	   	   U8checkdata=U8checkdata_temp;
	    }
		return 1;
	}
	else   //传感器不响应
	{
	   return 0;
	}
}



void delay5ms()   //误差 -0.000000000001us
{
    unsigned char a,b;
    for(b=15;b>0;b--)
        for(a=152;a>0;a--);
}


/******LCD1602写命令函数********/
void write_com(unsigned char command)
{
	RS = 0; 
	RW = 0; 	//高读低写
	E=0;
	LCD = command;
	delay5ms(); //这里延时最低要30纳秒 我们直接给5ms
	E = 1;		//使能拉高 
	delay5ms(); //最低要求延迟150纳秒 我们直接给5ms
	E = 0;
}

/******LCD1602写数据函数********/
void write_data(unsigned char dat)
{
	RS = 1;
	RW = 0;
	E=0;
	LCD = dat;
	delay5ms(); //这里延时最低要30纳秒 我们直接给5ms
	E = 1;		//使能拉高 
	delay5ms(); //最低要求延迟150纳秒 我们直接给5ms
	E = 0;
}
/******初始化LCD1602********/
void init_lcd1()
{	
	write_com(0x06); //写入数据后光标自动右移 整屏不移动。
	write_com(0x0c); //开显示功能 无光标 不闪烁
	write_com(0x38); //数据总线8位 16X2显示 5*7点阵
	write_com(0x01); //清屏 0000 0001
	write_com(0x81); //在1602第一行第1个开始显示
}


void display(void)
	{uc array1[]={"HUMIDITY:      000000000000000000000000 MODE:"};
	while(array1[i] != '\0')
	{	
		write_data(array1[i]);
		i++;
	}	
}

void delay(ui x)//延时函数 
{
	ui i,j;
	for(i=0;i<x;i++)
	for(j=0;j<110;j++);
}

int work(int s)//工作函数 
{
	 int w1=2;
	 int w2=3;
	if(change==0)//判断切换是否按下 
	{
		buzz(0);
		delay(20);//////////////////////
		if(change==0)
		change_flag=~change_flag;//手动，自动切换 
		
		led_zhishi=change_flag;
		if(change_flag==1)
		{
			out_forward=1;
			out_backward=1;
			led_forward=1;
			led_backward=1;
		}
		buzz(1);
		while(!change)//释放 
		{buzz(1);}
	}
	
	if(change_flag==0)//自动模式 
	{	
		write_com(0xC6);
		write_data('A');
		delay5ms();
		if(~light==0||flag_HH==1)//夜间（无光）时拉回衣架 	  flag_HH==1湿度高
		{
			delay(20);
			if(~light==0||flag_HH==1)
			{  if(s%w1 != 0)
				{ out_forward=1;
					out_backward=1;
				  delay(2500);}
				else if((limit_backward==0)&&(flag_shan==0))
				{	
					out_forward=1;
					out_backward=1;
					led_forward=0;
					
					for(shan=0;shan<6;shan++)
					{
						led_backward=~led_backward;
						buzz(led_backward);
						delay(500);
					}
					flag_shan=1;
				}
				else if(limit_backward==1)
				{	write_com(0xCD);
					write_data('I');
					write_data('N');
					write_data(' ');
					out_forward=0;
					out_backward=1;///
					
					led_backward=1;
					led_forward=0;
					flag_shan=0;
				}
			}
			s=w1;
		}
		else if(~light==1&&flag_HH==0)//白天（有光）时伸出衣架 
		{
			delay(20);
			if(~light==1&&flag_HH==0)
			{ if(s%w2 != 0)
				{ 
					out_forward=1;
					out_backward=1;
				  delay(2500);}
				else if((limit_forward==0)&&(flag_shan==0))
				{	
					out_forward=1;
					out_backward=1;
					
					led_backward=0;
					for(shan=0;shan<6;shan++)
					{
						led_forward=~led_forward;
						buzz(led_forward);
						delay(500);
					}
					flag_shan=1;
				}
				else if(limit_forward==1)
				{	write_com(0xCD);
					write_data('O');
					write_data('U');
					write_data('T');
					out_forward=1;////
					out_backward=0;
					
					led_backward=0;
					led_forward=1;
					flag_shan=0;
				}
			s=w2;
			}
		}
	}
	else if(change_flag==1)//手动模式 
	{ 
		write_com(0xC6);
		write_data('M');
		if((in_forward==0)&&(limit_forward==1))//关闭是否按下 
		{
			buzz(0);
			delay(5);
			if((in_forward==0)&&(limit_forward==1))
			{	write_com(0xCD);
					write_data('O');
					write_data('U');
					write_data('T');
				buzz(1);
				out_forward=1;
				out_backward=~out_backward;//按一下伸出，再按一下，停止 
				
				buzz(1);
				led_forward=1;
				led_backward=out_backward;
				flag_shan1=0;
				
			}
			while(!in_forward)//释放
			buzz(1);
		}
		else if((in_backward==0)&&(limit_backward==1))//打开是否按下 
		{
			buzz(0);
			delay(5);
			if((in_backward==0)&&(limit_backward==1))
			{ write_com(0xCD);
			  write_data('I');
				write_data('N');
				write_data(' ');
				buzz(1);	
				out_backward=1;
				out_forward=~out_forward;//按一下收回，再按一下，停止  
				
				buzz(1);
				led_backward=1;
				led_forward=out_forward;
				flag_shan2=0;
				
			}
			while(!in_backward)//按键释放
			buzz(1);
		}
		if((limit_forward==0)&&(flag_shan1==0))
		{
			delay(5);
			if((limit_forward==0)&&(flag_shan1==0))
			{	
				
				out_backward=1;
				led_backward=0;
				for(shan=0;shan<6;shan++)
				{
					led_forward=~led_forward;
					buzz(led_forward);
					delay(500);
				}
				flag_shan1=1;
			}
		}
		if((limit_backward==0)&&(flag_shan2==0))
		{
			delay(5);
			if((limit_backward==0)&&(flag_shan2==0))
			{	
				
				out_forward=1;
				led_forward=0;
				for(shan=0;shan<6;shan++)
				{
					led_backward=~led_backward;
					buzz(led_backward);
					delay(500);
				}
				flag_shan2=1;
			}
		}
	}
return s;
}

void main()//主函数 
{
	uc testnum,shiwei,gewei;
	ui humidity;
	int s=6;//避免电机突然反向电压不稳的保护措施，每个方向对应一种状态，检测到状态改变先让电机停止转动再反向
	int i;
	buzz(1);
	led_zhishi=0;
	Timer0_Init();
	//Init();
	init_lcd1();
	testnum = RH();
  FlagStartRH = 0;
	humidity = U8RH_data_H;	 //读出湿度，只取整数部分
	display();
	SendData(humidity);
	i=1;
	while(1)//进入死循环
	{  
		s=work(s);//调用工作函数 
		if (FlagStartRH == 1)	 //温湿度转换标志检查
		{
			write_com(0x8A);
      testnum = RH();
 			FlagStartRH = 0;
			humidity = U8RH_data_H;	 //读出湿度，只取整数部分
			gewei=humidity%10+'0';
      shiwei=humidity/10+'0';
      write_data(shiwei);
			write_com(0x8B);
			write_data(gewei);
			delay5ms();
			if(humidity>=80)
			flag_HH=1;
			else 
			flag_HH=0;
			TR0 = 1;
		}
	}
}

//定时器0中断
void Timer0_ISR (void) interrupt 1 using 0
{
	ui RHCounter;
	TL0 = 0xb0;
	TH0 = 0x3c;     //定时器赋予初值

	//每5秒钟启动一次温湿度转换
    RHCounter ++;
    if (RHCounter >= 20)
    {
       FlagStartRH = 1;
	   RHCounter = 0;
	   TR0=0;
    }
}
void SeriesInterrupt() interrupt 4
{
	if(RI)//接收完毕 接收中断位RI置1
	{
		
		DataGet=SBUF;//取出接收的数据
		RI=0;
		DataGet=DataGet-'0';
		SendData2(DataGet);//将接收的数据发送出去
//		if(DataGet=='Y')
//			P1=0x00;//开灯
//		else if(DataGet=='N')
//			P1=0xFF;//关灯
//		else
//			P1=0x55;//交替
	}
}
