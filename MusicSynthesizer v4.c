#include<avr/io.h>
#include<avr/interrupt.h>
#pragma data:code
#define uchar unsigned char
#define uint unsigned int
const uchar table[]=" Piano";

uint num = 0,count = 0;
uint Mode = 0,list = 2,pause = 0,aim = 0,sure = 0,del = 0;
char state = 0,S = 1;            //定义全局变量,S为变速变量


const uint Mode_Data[16] = {0,440,494,523,587,659,698,784,880,998,1046,1156,1318,1396,1568}; //存放声音的频率

//音高对应定时器初始化数值（低la~高la+休止符），cpu频率1MHz，用8分频
const uchar tone[] = {0x00,0x8E,0x7E,0x77,0x6A,0x5E,0x59,0x4F,0x47,
					  0x3F,0x3B,0x35,0x2F,0x2C,0x27,0x23,0X19,0X15};

uint Ssong[10][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

/************************
延时函数
************************/
void delay_us(int n)        //微妙级延时函数；
{
     while(n--)
	{ 
	   asm("nop");    //单片机一个周期无动作行,延时
	}
}

void delay_ms(int n)   //毫秒级延时函数；
{
     int m = 14500*n;
     while(m--)
    {
	    asm("nop");
    }
}
/**********************************************/

/************************
屏幕显示函数
************************/
void write_com(uchar com)
{
     PORTD &= ~(1<<4);      //低电平指令模式。高电平数据；
	 PORTD &= ~(1<<6);      //低电平写；
	 PORTB = com;
	 PORTD |= (1<<7);       //高电平使能
	 delay_ms(1);
	 PORTD &= ~(1<<7);        
}

void write_dat(uchar dat)
{     
     PORTD |= (1<<4);
	 PORTD &= ~(1<<6);
	 PORTB = dat;
	 PORTD |= (1<<7);
	 delay_ms(1);
	 PORTD &= ~(1<<7);
}

/***********************************
*       音乐产生函数   *
* 功能：输出频率为x的方波*
* 范围：x:100-20000Hz，0：不发声   *
*     
***********************************/
void sound (int i)          
{
	 TIMSK |= (1<<2);
	 sei();         
	 if(i && i<17)                  //按了音符键了.且在0到F键这16个发音键上。
	 {		
	     OCR1A = tone[i];
	     TCCR1A = 0X50;	                             //控制寄存器，选择可翻转功能，产生方波；
	     TCCR1B = 0X0a;
	     PORTD = (1<<5);
	 }
	 else
     {  
         TCCR1A = 0x00;                        //频率为0，休止符，不发声
         TCCR1B = 0X00;
         OCR1A = 0;
     }
}

/************************************
弹奏函数
************************************/
void play(int n)         //参数为键盘上的数字；
{	
	 /*write_com(0X80+0X40+n);
	 write_dat(16);*/
	 sound(n);                           //弹奏音符；
	 delay_ms(1);
     //write_dat(20);
     TCCR1A = 0x00;  
	 TCCR1B = 0X00;                        //中断，为下一个节拍做准备；
	 PORTD |= (1<<5);                      //PD5作为输出管脚；
}


//___________________________________________
void INI_POTR()
{
	
	 DDRD |= 0xff;
	 DDRB = 0xff;  
	 PORTD = (1<<5);
	 PORTD &= ~(1<<7);
	 DDRA = 0Xff;   //A口作为键盘输入口；
	 DDRB = 0Xff;     //PB0作为音频输出，接喇叭；
}
//_____________________________________
void INN_DEVICE()
{
    //	CLI();      //标志寄存器置0.禁止所有中断；
	MCUCR = 0X00;     //  中断寄存器置零；
}
//____________________________________

//键盘输入检测函数；
//_______________________________
uchar key_press()
{
    uchar j;
    DDRA|=0XFF;
	PORTA|=0XF0;
	
	DDRA=0X0F;
	
	j=PINA;
	if(j==0XF0)//没有按键按下
	{
//		DDRA=0XFF;//让D口全部为输出?
	    return 0;
	}
	else       //有按键按下
	{
//		DDRA=0XFF;//让D口全部为输出
		return 1;
	}
	
}
//键盘输入检测函数；
//_______________________________
int key_in()                   
{   
	DDRA = 0xff;              //先全部设置为输出，再将后四位置为输入
	PORTA = 0xf0;             //设置高低电平
	DDRA = 0xf0;
	uint i = 0,j = 16,Key = 17;        //一定要给ij赋上初值！！！
	unsigned char temp1,temp2;
	
	temp1 = PINA; 
	temp1 &= 0x0f;//只关心低四位的情况
	switch(temp1)
	{
		case 0b00000001:
			j = 0;
			break;
		case 0b00000010:
			j = 1;
			break;
		case 0b00000100:
			j = 2;
			break;
		case 0b00001000:
			j = 3;
			break;
	} 
	delay_ms(10);
	if(key_press())
	{
	//temp2 = PINA;
	//temp2 &= 0x0f;            //去抖动
	//if(temp1 != temp2)
	//	return 17;
	DDRA = 0xff;
	PORTA= 0x0f;
	DDRA = 0x0f;
	temp1 = PINA;
	temp1 &= 0xf0;//只关心高四位的情况
	switch(temp1)
	{
		case 0b00010000:
			i = 0;
			break;
		case 0b00100000:
			i = 1;
			break;
		case 0b01000000:
			i = 2;
			break;
		case 0b10000000:
			i = 3;
			break;
	}
	
	//temp2 = PINA;
	//temp2 &= 0xf0;
	//if(temp1 != temp2)
	//	return 17;
	Key = i*4 + j+1;
	return Key;     
	}
	else
		return 17;
	}

/*****************************
音乐播放函数；
*****************************/
void  m_sound(uint a)
{   
	 int m = (62500/a)-1;
	 OCR1A = m;
	 TCCR1A = 0x50;	          //控制寄存器，选择可翻转功能，产生方波；
	 TCCR1B = 0x0a;
}

//_______________________________________

//**********************************播放函数；
void music_play(uint a[][2])
{
	 const char Mtable[] = "music";
	 write_com(0X01);        //清屏；
	 for(int i = 0;i<5;i++)
	 {
		write_dat(Mtable[i]);
	 }
     
	 int i = 0; 
     char tem = aim,T = 1;  
     while((a[i][1] != 0)&&(tem == aim)&&state)     
    {	
        num = key_in();
	    switch(num)
	    {
			case 16:
			     state=0;  //切换到
			     break;
			case 15:
			     aim=1;
			     break;
			case 14:
				 pause=1; 
				 break;
			case 13:
				 aim=0;   //播放曲目
				 break;
			case 12:
				 pause=0;
				 break;
			case 10:
				 T=1;   
				 break;
			case 9:
				 T=0;
				 break;
			case 6:
				 T=2;
				 break;
            case 1:
				 S=0.5;
				 break;
            case 2:
				 S=1;
				 break;
            case 3:
				 T=2;
				 break;

			default:
					break;			
		}	
		if(pause)          
		{
            m_sound((a[i][0]*S));            
		    if(T==2)                   //加减速
	           delay_ms(a[i][1]-100);	
		    else if(T==0)
		       delay_ms(a[i][1]+100);
		    else
		       delay_ms(a[i][1]);
		    i++;                                 //下一个音符；
		}
	    TCCR1A = 0x00;  
		TCCR1B = 0x00;                        //中断，为下一个节拍做准备；					
    }
    if(a[i][1] == 0)
	{
		//aim = (++aim)%list; 
		++aim;
		aim %= list; 
	}
}


/*****************
录制音乐函数；
*****************/

void record()
{   
	write_com(0x01);        //清屏；
    const uchar R_table[] = "Recording";

	for(int i = 0;i<9;i++)
	{
        write_dat(R_table[i]);
	}
	
    pause=1;
	while(pause)
	{
	   uint Skey = key_in();
	   if(1)                     //开始录制
	   {
			int i = 0,count = 0;             //先清零计数器	
		    while(i<10)       //无键按下，则一直循环,且在录制；赋值之后，不为零，跳出while;
			{	
				Skey = key_in();
				count++;

				sound(Skey);
				delay_ms(100);
				OCR1A = 0;
				TCCR1A = 0x00;  
				TCCR1B = 0x00;  
				
				if(Skey&&Skey<11)       
				{
					
					Ssong[i][0] = Skey;           
	                Ssong[i][1] = count*2;         
	                count = 0;                 //还原计数器；
					i++;  
					delay_ms(20); 
				}
				if(Skey == 14)             //录制完成；
				{
                    i = 10;
				}	
						
			}
			pause = 0;
		}
	}
	
	for(int i = 0;i<10;i++)
	{
	    sound(Ssong[i][0]);
	    delay_ms(Ssong[i][1]);
	    OCR1A = 0;
	    TCCR1A = 0x00;  
	    TCCR1B = 0x00;                     //中断，为下一个节拍做准备；
	}
}

//***************************歌曲数据
uint music_data[][2] =
{
{1046,400},{998,400},{1046,400},{998,400},{1046,400},{784,400},{998,400},{880,400},{698,400},{440,400},{523,400},{698,400},{784,400},{880,400},
{523,400},{1046,400},{998,400},{1046,400},{998,400},{1046,400},{784,400},{998,400},{880,400},{698,400},
{440,400},{523,400},{698,400},{784,400},
{494,400},{880,400},{784,400},{698,400},
{0,0}
};                   


//  abcdefg

uint music_1[][2] =
{
    {262,400},{294,400},{330,400},{262,400},{262,400},  //乐谱
    {294,400},{330,400},{262,400},{330,400},{349,400},
    {392,800},{330,400},{349,400},{392,800},{392,300},
    {440,100},{392,300},{349,100},{262,400},{392,400},
    {440,300},{392,100},{349,300},{330,100},{262,400},
    {294,400},{196,400},{262,400},{294,800},{196,400},
    {262,400},{294,800},{0,0}
};
//music_1

int main()
{
	INI_POTR();	                          //必须启动！
	write_com(0x38);      //显示光标等；
	write_com(0x01);        //清屏；
	write_com(0x0f);         //打开光标；
	write_com(0x06);
	write_com(0x80+0x02); 
	for(int i = 0;i<6;i++)
	{
	    write_dat(table[i]);		
	}
	write_com(0x80+0x11);
	delay_ms(5);  
	while(1)
	{
	    num = key_in();
	
	    if(num == 16)                  //*****模式选择、通过按F键切换
        {
		   delay_ms(15); 
		   if(state == 1)
		      state=0;
		   else
		      state=1;				
	    }                                      
                       
//播放功能切换；播放，弹奏,启动默认为弹奏；	
//*********************************播放模式
	    if(state)               //判断当前状态：state为1，音乐播放模式
	    {
		   while(state)
		  {
		      switch(aim)
		     {
		      case 0:
		           music_play(music_data);
	               break;
	          case 1:
		           music_play(music_1);
		           break;
		      default:
		           break;
		     }
		     delay_ms(10);
		  }
	    }
	    else if(num==11)                          //录制模式；
	    {
		    record();
	        delay_ms(20);
		}     
        else                   //当前状态state不为1，弹奏模式
	   { 	
	        write_com(0x01);        //清屏；
		    const uchar Ptable[] = "playing";
		    for(int i = 0;i<7;i++)
		    {
			    write_dat(Ptable[i]);
		    }

	        while((num != 16)&&(num != 11))
	       {
		       num = key_in();
		       play(num);  
	       }	                          //弹奏模式；
	    }
	}
	return 0;  	


}
