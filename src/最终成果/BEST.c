#include<reg52.h>
#include<intrins.h>
#include<absacc.h>		//ͷ�ļ�


#define uc unsigned char
#define ui unsigned int	
	//���Ŷ���
#define LCD P2

sbit RS = P0^1;
sbit RW = P0^2;
sbit E = P0^3;


sbit in_forward		=	P1^2;
sbit in_backward	=	P1^1;//�ֶ�ǰ�����˰��� 
sbit out_forward		=	P0^5;
sbit out_backward	=	P0^6;//���������� 
sbit limit_forward	=	P1^3;
sbit limit_backward	=	P1^4;//ǰ��������λ���� 
sbit led_forward		=	P1^6;
sbit led_backward	=	P1^5;//ǰ��������λָʾ�� 
sbit light		=	P3^7;//���ߴ����� 
sbit change		=	P1^0;//�ֶ��Զ��л�������Ĭ���Զ���  
sbit led_zhishi =	P1^7;
sbit beep = P0^0;	 //���������������

sbit W1=P2^2;
sbit W2=P2^1;
//����DHT11���ݿ�
sbit DATA = P0^7;
uc U8FLAG,U8temp,U8comdata,U8RH_data_H_temp,U8RH_data_L_temp,U8T_data_H_temp,U8T_data_L_temp,U8checkdata_temp,string[]={""};
uc U8RH_data_H,U8RH_data_L,U8T_data_H,U8T_data_L,U8checkdata,DataGet;
ui Mode,i = 0;
bit bdata FlagStartRH,hot,cold,humid,dry;

bit bdata flag1,flag2,change_flag,flag_shan,flag_shan1,flag_shan2,flag_HH;
uc m,n,shan;


//��ʱ��0��ʼ��
void Timer0_Init()
{
	ET0 = 1;        //����ʱ��0�ж�
	TMOD = 0x21;       //��ʱ��������ʽѡ��
	TL0 = 0xb0;     
	TH0 = 0x3c;     //��ʱ�������ֵ
	TR0 = 1;        //������ʱ��
	EA=1;
}


void Init()
{
	//��ʱ��1��ʼ��
	TMOD=0x21;//M1:M0=10 ��ʱ��0������ʽ2, ������8λ
	
	IT1=0;//�͵�ƽ
	ET1=0;//�ж�����
	TR1=1;//������ʱ������
	
	TH1=0xfd;
	TL1=0xfd;//���ö�ʱ����ֵ
//	TH1=0xFF;
//	TL1=0xE8;
	PCON=0x00;//SCON=0
	
	//���ڳ�ʼ��
	SM0=0;
	SM1=1;//���ڹ�����ʽ1 8λUART
	REN=1;//ʹ�ܴ��н���
	ES=1;//�����ж�����
	EA=1;//�������ж�����λ

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
		while(!TI);//��û�з��������� �ȴ�
		TI=0;//�������� ��־λ��0
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
		while(!TI);//��û�з��������� �ȴ�
		TI=0;//�������� ��־λ��0
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
 * @ ������  �� Delay_10us
 * @ ��  ��  �� 10us������ʱ
 * @ ��  ��  �� ��ʱʱ��--��λ10us
 * @ ����ֵ  �� ��
 ******************************************************************************/
void Delay_10us_1(unsigned int time)
{
	while(time--);
}
 
 /******************************************************************************
 * @ ������  �� Simulate_Pwm
 * @ ��  ��  �� ģ��PWM���ƣ���Ҫ��ͣ���У�
 * @ ��  ��  �� period ���ڣ���λ10us��
 *              duty   ռ�ձȣ��͵�ƽ��0-100��
 * @ ����ֵ  �� ��
 ******************************************************************************/
void Simulate_Pwm(unsigned char period, unsigned char duty)
{
	int i;
	unsigned char low = period / 100 * duty; //�͵�ƽ��ʱʱ��
	unsigned char high = period - low;       //�ߵ�ƽ��ʱʱ��
	for(i=0;i<=500;i++)
	{
	beep = 0;        //������IO��ƽ�õ�
	Delay_10us_1(low); //�͵�ƽ��ʱʱ��
	beep = 1;        //������IO��ƽ�ø�
	Delay_10us_1(high);//�ߵ�ƽ��ʱʱ��
	}
}

/******************************************************************************
 * @ ������  �� main
 * @ ��  ��  �� ������
 * @ ��  ��  �� ��
 * @ ����ֵ  �� ��
 ******************************************************************************/

void buzz(ui number)
{
	if (number==1)
	{
			Simulate_Pwm(100, 99);  //����100*10us��ռ�ձ�50%����Ƶ�ʣ�1KHz
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
	   	//��ʱ������forѭ��		  
	   	if(U8FLAG==1)break;
	   	//�ж�����λ��0����1	 
	   	   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 
	   	 
		U8comdata<<=1;
	   	U8comdata|=U8temp;        //0
    }//rof
}

/*--------------------------------
-----��ʪ�ȶ�ȡ�ӳ��� ------------
--------------------------------
----���±�����Ϊȫ�ֱ���--------
----�¶ȸ�8λ== U8T_data_H------
----�¶ȵ�8λ== U8T_data_L------
----ʪ�ȸ�8λ== U8RH_data_H-----
----ʪ�ȵ�8λ== U8RH_data_L-----
----У�� 8λ == U8checkdata-----
----��������ӳ�������----------
---- Delay();, Delay_10us();,COM(); 
--------------------------------*/
uc RH(void)
{
    //��������18ms 
    DATA=0;
	Delay1(180);  //ԭ��Ϊ5
	DATA=1;
	//������������������ ������ʱ20us
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	//������Ϊ���� �жϴӻ���Ӧ�ź� 
	DATA=1;
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(!DATA)		 //T !	  
	{
	    U8FLAG=2;
	    //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	    while((!DATA)&&U8FLAG++);
	    U8FLAG=2;
	    //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	    while((DATA)&&U8FLAG++);
	    //���ݽ���״̬		 
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
	    //����У�� 
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
	else   //����������Ӧ
	{
	   return 0;
	}
}



void delay5ms()   //��� -0.000000000001us
{
    unsigned char a,b;
    for(b=15;b>0;b--)
        for(a=152;a>0;a--);
}


/******LCD1602д�����********/
void write_com(unsigned char command)
{
	RS = 0; 
	RW = 0; 	//�߶���д
	E=0;
	LCD = command;
	delay5ms(); //������ʱ���Ҫ30���� ����ֱ�Ӹ�5ms
	E = 1;		//ʹ������ 
	delay5ms(); //���Ҫ���ӳ�150���� ����ֱ�Ӹ�5ms
	E = 0;
}

/******LCD1602д���ݺ���********/
void write_data(unsigned char dat)
{
	RS = 1;
	RW = 0;
	E=0;
	LCD = dat;
	delay5ms(); //������ʱ���Ҫ30���� ����ֱ�Ӹ�5ms
	E = 1;		//ʹ������ 
	delay5ms(); //���Ҫ���ӳ�150���� ����ֱ�Ӹ�5ms
	E = 0;
}
/******��ʼ��LCD1602********/
void init_lcd1()
{	
	write_com(0x06); //д�����ݺ����Զ����� �������ƶ���
	write_com(0x0c); //����ʾ���� �޹�� ����˸
	write_com(0x38); //��������8λ 16X2��ʾ 5*7����
	write_com(0x01); //���� 0000 0001
	write_com(0x81); //��1602��һ�е�1����ʼ��ʾ
}


void display(void)
	{uc array1[]={"HUMIDITY:      000000000000000000000000 MODE:"};
	while(array1[i] != '\0')
	{	
		write_data(array1[i]);
		i++;
	}	
}

void delay(ui x)//��ʱ���� 
{
	ui i,j;
	for(i=0;i<x;i++)
	for(j=0;j<110;j++);
}

int work(int s)//�������� 
{
	 int w1=2;
	 int w2=3;
	if(change==0)//�ж��л��Ƿ��� 
	{
		buzz(0);
		delay(20);//////////////////////
		if(change==0)
		change_flag=~change_flag;//�ֶ����Զ��л� 
		
		led_zhishi=change_flag;
		if(change_flag==1)
		{
			out_forward=1;
			out_backward=1;
			led_forward=1;
			led_backward=1;
		}
		buzz(1);
		while(!change)//�ͷ� 
		{buzz(1);}
	}
	
	if(change_flag==0)//�Զ�ģʽ 
	{	
		write_com(0xC6);
		write_data('A');
		delay5ms();
		if(~light==0||flag_HH==1)//ҹ�䣨�޹⣩ʱ�����¼� 	  flag_HH==1ʪ�ȸ�
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
		else if(~light==1&&flag_HH==0)//���죨�й⣩ʱ����¼� 
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
	else if(change_flag==1)//�ֶ�ģʽ 
	{ 
		write_com(0xC6);
		write_data('M');
		if((in_forward==0)&&(limit_forward==1))//�ر��Ƿ��� 
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
				out_backward=~out_backward;//��һ��������ٰ�һ�£�ֹͣ 
				
				buzz(1);
				led_forward=1;
				led_backward=out_backward;
				flag_shan1=0;
				
			}
			while(!in_forward)//�ͷ�
			buzz(1);
		}
		else if((in_backward==0)&&(limit_backward==1))//���Ƿ��� 
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
				out_forward=~out_forward;//��һ���ջأ��ٰ�һ�£�ֹͣ  
				
				buzz(1);
				led_backward=1;
				led_forward=out_forward;
				flag_shan2=0;
				
			}
			while(!in_backward)//�����ͷ�
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

void main()//������ 
{
	uc testnum,shiwei,gewei;
	ui humidity;
	int s=6;//������ͻȻ�����ѹ���ȵı�����ʩ��ÿ�������Ӧһ��״̬����⵽״̬�ı����õ��ֹͣת���ٷ���
	int i;
	buzz(1);
	led_zhishi=0;
	Timer0_Init();
	//Init();
	init_lcd1();
	testnum = RH();
  FlagStartRH = 0;
	humidity = U8RH_data_H;	 //����ʪ�ȣ�ֻȡ��������
	display();
	SendData(humidity);
	i=1;
	while(1)//������ѭ��
	{  
		s=work(s);//���ù������� 
		if (FlagStartRH == 1)	 //��ʪ��ת����־���
		{
			write_com(0x8A);
      testnum = RH();
 			FlagStartRH = 0;
			humidity = U8RH_data_H;	 //����ʪ�ȣ�ֻȡ��������
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

//��ʱ��0�ж�
void Timer0_ISR (void) interrupt 1 using 0
{
	ui RHCounter;
	TL0 = 0xb0;
	TH0 = 0x3c;     //��ʱ�������ֵ

	//ÿ5��������һ����ʪ��ת��
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
	if(RI)//������� �����ж�λRI��1
	{
		
		DataGet=SBUF;//ȡ�����յ�����
		RI=0;
		DataGet=DataGet-'0';
		SendData2(DataGet);//�����յ����ݷ��ͳ�ȥ
//		if(DataGet=='Y')
//			P1=0x00;//����
//		else if(DataGet=='N')
//			P1=0xFF;//�ص�
//		else
//			P1=0x55;//����
	}
}
