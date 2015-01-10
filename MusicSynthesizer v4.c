#include<avr/io.h>
#include<avr/interrupt.h>
#pragma data:code
#define uchar unsigned char
#define uint unsigned int
const uchar table[]=" Piano";

uint num = 0,count = 0;
uint Mode = 0,list = 2,pause = 0,aim = 0,sure = 0,del = 0;
char state = 0,S = 1;            //����ȫ�ֱ���,SΪ���ٱ���


const uint Mode_Data[16] = {0,440,494,523,587,659,698,784,880,998,1046,1156,1318,1396,1568}; //���������Ƶ��

//���߶�Ӧ��ʱ����ʼ����ֵ����la~��la+��ֹ������cpuƵ��1MHz����8��Ƶ
const uchar tone[] = {0x00,0x8E,0x7E,0x77,0x6A,0x5E,0x59,0x4F,0x47,
					  0x3F,0x3B,0x35,0x2F,0x2C,0x27,0x23,0X19,0X15};

uint Ssong[10][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

/************************
��ʱ����
************************/
void delay_us(int n)        //΢���ʱ������
{
     while(n--)
	{ 
	   asm("nop");    //��Ƭ��һ�������޶�����,��ʱ
	}
}

void delay_ms(int n)   //���뼶��ʱ������
{
     int m = 14500*n;
     while(m--)
    {
	    asm("nop");
    }
}
/**********************************************/

/************************
��Ļ��ʾ����
************************/
void write_com(uchar com)
{
     PORTD &= ~(1<<4);      //�͵�ƽָ��ģʽ���ߵ�ƽ���ݣ�
	 PORTD &= ~(1<<6);      //�͵�ƽд��
	 PORTB = com;
	 PORTD |= (1<<7);       //�ߵ�ƽʹ��
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
*       ���ֲ�������   *
* ���ܣ����Ƶ��Ϊx�ķ���*
* ��Χ��x:100-20000Hz��0��������   *
*     
***********************************/
void sound (int i)          
{
	 TIMSK |= (1<<2);
	 sei();         
	 if(i && i<17)                  //������������.����0��F����16���������ϡ�
	 {		
	     OCR1A = tone[i];
	     TCCR1A = 0X50;	                             //���ƼĴ�����ѡ��ɷ�ת���ܣ�����������
	     TCCR1B = 0X0a;
	     PORTD = (1<<5);
	 }
	 else
     {  
         TCCR1A = 0x00;                        //Ƶ��Ϊ0����ֹ����������
         TCCR1B = 0X00;
         OCR1A = 0;
     }
}

/************************************
���ຯ��
************************************/
void play(int n)         //����Ϊ�����ϵ����֣�
{	
	 /*write_com(0X80+0X40+n);
	 write_dat(16);*/
	 sound(n);                           //����������
	 delay_ms(1);
     //write_dat(20);
     TCCR1A = 0x00;  
	 TCCR1B = 0X00;                        //�жϣ�Ϊ��һ��������׼����
	 PORTD |= (1<<5);                      //PD5��Ϊ����ܽţ�
}


//___________________________________________
void INI_POTR()
{
	
	 DDRD |= 0xff;
	 DDRB = 0xff;  
	 PORTD = (1<<5);
	 PORTD &= ~(1<<7);
	 DDRA = 0Xff;   //A����Ϊ��������ڣ�
	 DDRB = 0Xff;     //PB0��Ϊ��Ƶ����������ȣ�
}
//_____________________________________
void INN_DEVICE()
{
    //	CLI();      //��־�Ĵ�����0.��ֹ�����жϣ�
	MCUCR = 0X00;     //  �жϼĴ������㣻
}
//____________________________________

//���������⺯����
//_______________________________
uchar key_press()
{
    uchar j;
    DDRA|=0XFF;
	PORTA|=0XF0;
	
	DDRA=0X0F;
	
	j=PINA;
	if(j==0XF0)//û�а�������
	{
//		DDRA=0XFF;//��D��ȫ��Ϊ���?
	    return 0;
	}
	else       //�а�������
	{
//		DDRA=0XFF;//��D��ȫ��Ϊ���
		return 1;
	}
	
}
//���������⺯����
//_______________________________
int key_in()                   
{   
	DDRA = 0xff;              //��ȫ������Ϊ������ٽ�����λ��Ϊ����
	PORTA = 0xf0;             //���øߵ͵�ƽ
	DDRA = 0xf0;
	uint i = 0,j = 16,Key = 17;        //һ��Ҫ��ij���ϳ�ֵ������
	unsigned char temp1,temp2;
	
	temp1 = PINA; 
	temp1 &= 0x0f;//ֻ���ĵ���λ�����
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
	//temp2 &= 0x0f;            //ȥ����
	//if(temp1 != temp2)
	//	return 17;
	DDRA = 0xff;
	PORTA= 0x0f;
	DDRA = 0x0f;
	temp1 = PINA;
	temp1 &= 0xf0;//ֻ���ĸ���λ�����
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
���ֲ��ź�����
*****************************/
void  m_sound(uint a)
{   
	 int m = (62500/a)-1;
	 OCR1A = m;
	 TCCR1A = 0x50;	          //���ƼĴ�����ѡ��ɷ�ת���ܣ�����������
	 TCCR1B = 0x0a;
}

//_______________________________________

//**********************************���ź�����
void music_play(uint a[][2])
{
	 const char Mtable[] = "music";
	 write_com(0X01);        //������
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
			     state=0;  //�л���
			     break;
			case 15:
			     aim=1;
			     break;
			case 14:
				 pause=1; 
				 break;
			case 13:
				 aim=0;   //������Ŀ
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
		    if(T==2)                   //�Ӽ���
	           delay_ms(a[i][1]-100);	
		    else if(T==0)
		       delay_ms(a[i][1]+100);
		    else
		       delay_ms(a[i][1]);
		    i++;                                 //��һ��������
		}
	    TCCR1A = 0x00;  
		TCCR1B = 0x00;                        //�жϣ�Ϊ��һ��������׼����					
    }
    if(a[i][1] == 0)
	{
		//aim = (++aim)%list; 
		++aim;
		aim %= list; 
	}
}


/*****************
¼�����ֺ�����
*****************/

void record()
{   
	write_com(0x01);        //������
    const uchar R_table[] = "Recording";

	for(int i = 0;i<9;i++)
	{
        write_dat(R_table[i]);
	}
	
    pause=1;
	while(pause)
	{
	   uint Skey = key_in();
	   if(1)                     //��ʼ¼��
	   {
			int i = 0,count = 0;             //�����������	
		    while(i<10)       //�޼����£���һֱѭ��,����¼�ƣ���ֵ֮�󣬲�Ϊ�㣬����while;
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
	                count = 0;                 //��ԭ��������
					i++;  
					delay_ms(20); 
				}
				if(Skey == 14)             //¼����ɣ�
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
	    TCCR1B = 0x00;                     //�жϣ�Ϊ��һ��������׼����
	}
}

//***************************��������
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
    {262,400},{294,400},{330,400},{262,400},{262,400},  //����
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
	INI_POTR();	                          //����������
	write_com(0x38);      //��ʾ���ȣ�
	write_com(0x01);        //������
	write_com(0x0f);         //�򿪹�ꣻ
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
	
	    if(num == 16)                  //*****ģʽѡ��ͨ����F���л�
        {
		   delay_ms(15); 
		   if(state == 1)
		      state=0;
		   else
		      state=1;				
	    }                                      
                       
//���Ź����л������ţ�����,����Ĭ��Ϊ���ࣻ	
//*********************************����ģʽ
	    if(state)               //�жϵ�ǰ״̬��stateΪ1�����ֲ���ģʽ
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
	    else if(num==11)                          //¼��ģʽ��
	    {
		    record();
	        delay_ms(20);
		}     
        else                   //��ǰ״̬state��Ϊ1������ģʽ
	   { 	
	        write_com(0x01);        //������
		    const uchar Ptable[] = "playing";
		    for(int i = 0;i<7;i++)
		    {
			    write_dat(Ptable[i]);
		    }

	        while((num != 16)&&(num != 11))
	       {
		       num = key_in();
		       play(num);  
	       }	                          //����ģʽ��
	    }
	}
	return 0;  	


}
