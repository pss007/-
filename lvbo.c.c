// 1 限幅滤波法
//先要根据经验判断，确定两次采样允许的最大偏差值A
//优点：能有效克服因偶然因素引起的脉冲干扰。
//缺点：无法抑制那种周期性的干扰，且平滑度差。
#define A 10
unsigned char Value;//最近一次有效采样值
unsigned char AmplitudeLimiterFilter()
{
	unsigned char NewValue;//当前采样值
	unsigned char ReturnValue;
	
	NewValue=GetAD();
	if(((NewValue-Value)>A))||((Value-NewValue)>A)))
		ReturnValue=Value;
	else
		ReturnValue=NewValue;
	return ReturnValue;

}

// 2 中位值滤波法
//连续采样N次，按大小排序，取中间值为本次有效值。
//优点：能有效克服因偶然因素引起的波动干扰;对温度，液位等变化缓慢的被测参数有良好的滤波效果。
//缺点：对流量，速度等快速变化的参数不宜。
#define N 9
unsigned char MiddleValueFilter()
{
	unsigned char i,j,k;
	unsigned char Temp;
	unsigned char ArrDataBuffer[N];
	
	for(i=0;i<N;i++)
	{
		ArrDataBuffer[i]=GetAD();
		Delay();
	}	
	for(j=0;j<N-1;j++)
	{
		for(k=0;k<N-j-1;k++)
		{
			if(ArrDataBuffer[k]>ArrDataBuffer[k+1])
				{
					Temp=ArrDataBuffer[k];
					ArrDataBuffer[k]=ArrDataBuffer[k+1];
					ArrDataBuffer[k+1]=Temp;
				}
		}
	}
	return ArrDataBuffer[(N-1)/2];
}

// 3 算术平均滤波法
//N较大时，信号平滑度较高，但灵敏度较低；N较小时，信号平滑度较低，但灵敏度较高。
//对于一般流量 N=12；对于压力 N=4
//优点：适用于对一般具有随机干扰的信号进行滤波。这种信号的特点是有一个平均值，信号在某一数值范围附近上下波动。
//缺点：对于测量速度较慢或者要求数据计算速度较快的实时控制不适用。也比较消耗RAM


// 4 递推平均滤波法，又称滑动平均滤波法
//把连续N个采样值看成一个队列，队列的长度固定为N。每次采集到一个新数据放入队尾，并扔掉原来队首的一次数据。把队列中的N个数据进行平均运算，即可获得新的滤波效果。
//流量 N=12；压力 N=4；液面 N=4~12; 温度 N=1~4.
//优点：对周期性干扰有良好的抑制作用，平滑度高；适用于高频震荡的系统。
//缺点：灵敏度低。对偶然出现的脉冲性干扰的抑制多用差；不易消除由脉冲干扰所引起的采样值偏差；不适用于脉冲干扰比较严重的场合。比较消耗RAM。

#define N 12
unsigned char Data[];
unsigned char GlideAverageValueFilter(Data[])
{
	unsigned char i;
	unsigned char Value;
	unsigned char Sum;
	Sum=0;
	Data[N]=GetAD();
	for(i=0;i<N;i++)
	{
		Data[i]=Data[i+1];//所有数据左移，低位扔掉
		Sum+=Data[i];	
	}	
	Value=Sum/N;
	return Value;
}


// 5 中位值平均滤波法 又称防脉冲干扰平均滤波法
//采集N个数据，去掉一个最大值和一个最小值，然后计算平均值 N=3~14
//优点：对于偶然出现的脉冲性干扰，可消除由其引起的采样值偏差。对周期性干扰由良好的抑制作用，平滑度高，适用于高频震荡的系统。
//缺点：测量速度慢，比较消耗RAM。

#define N 12 
unsigned char MiddleAverageValueFilter()
{
	unsigned char i,j,k,l;
	unsigned char Temp;
	unsigned char ArrDataBuffer[N];
	unsigned char Sum;
	unsigned char Value;
	for(i=0;i<N;i++)
	{
		ArrDataBuffer[i]=GetAD();
		Delay();
	}	
	for(j=0;j<N-1;j++)
	{
		for(k=0;k<N-j-1;k++)
		{
			if(ArrDataBuffer[k]>ArrDataBuffer[k+1])
				{
					Temp=ArrDataBuffer[k];
					ArrDataBuffer[k]=ArrDataBuffer[k+1];
					ArrDataBuffer[k+1]=Temp;
				}
		}
	}
	for(l=1;l<N-1;l++)
	{
		Sum+=ArrDataBuffer[l];
	}
	V	Value=Sum/(N-2);

	return Value;
}

// 6 递推中位值平均滤波法
//中位值滤波+递推平均滤波法
//优点：对于偶然出现的脉冲性干扰，可消除由其引起的采样值偏差。得到的滤波更趋于平滑，可信。
//缺点：要开设队列，比较消耗RAM。

char filter1(char NEW_DATA,char QUEUE[],char n)
{
	char max;
	char min;
	int sum;
	char i;
	QUEUE[0]= NEW_DATA;
	max=QUEUE[0];
	min=QUEUE[0];
	sum=QUEUE[0];
	for(i=n-1;i!=0;i--)
	{
		if(QUEUE[i]>max) 
			max=QUEUE[i];
		else if(QUEUE[i]<min)
			min=QUEUE[i];
		sum+=QUEUE[i];
		QUEUE[i]=QUEUE[i-1];
	}
	i=n-2;
	sum=sum-max-min+i/2;//平均值=（和值-最大值-最小值+n/2）/（队列长度-2）
	sum=sum/i;//+（n-2）/2的目的是为了四舍五入
	return (char)sum;
}
// 7 限幅平均滤波法
//每次采样到的新数据先进行限幅处理，在送入队列进行递推平均滤波处理。
//优点：对于偶然出现的脉冲干扰，可消除由于脉冲干扰所引起的采样值偏差
//缺点：需要开设队列存储历次采集数据，比较消耗RAM。

#define A 10
#define N 12
unsigned char Data[];
unsigned char LimitRangeAverageValueFilter(Data[])
{
	unsigned char i;
	unsigned char Value;
	unsigned short Sum;
	Data[N]=GetAD();
	if(((Data[N]-Data[N-1])>A)\\((Data[N-1]-Data[N])>A)))
		Data[N]=Data[N-1];
	else
		Data[N]=NewValue;
	for(i=0;i<N;i++)
	{
		Data[i]=Data[i+1];//所有数据左移，低位扔掉
		Sum+=Data[i];	
	}
	Value=Sum/N;
	return Value;
}

// 8 一阶滞后滤波法
//
//优点：对周期性干扰具有良好的抑制作用，适用于波动频率较高的场合。
//缺点：相位滞后，灵敏度较低。这种方法不能消除滤波频率高于1/2采样频率的干扰信号。对于没有乘法/除法运算指令的单片机来说，程序运算工作量较大。

#define a 128
unsigned char Value;
OneFactorialFilter()
{
	unsigned char NewValue;
	unsigned char ReturnValue;
	NewValue=GetAD();
	ReturnValue=(255-a)*NewValue+a*Value;
	ReturnValue/=255;
	return ReturnValue;
}


// 9 加权递推平均滤波法
//对递推平均滤波法的改进，即不同时刻的数据加以不同的权。越接近现时刻的数据，权取越大。给予新采集值的权系数越大，则灵敏度越高，但信号平滑度越低。
//优点：适用于有较大纯滞后时间常数的对象，如采样周期较短的系统。
//缺点：对于纯滞后时间常数较小，采样周期较长，变化缓慢的信号，不能迅速反应系统当前所受干扰的严重程度，滤波效果差。



// 10 消抖滤波法 
//设置一个滤波计数器。将每次采集值与当前有效值比较。如果采样值=当前有效值，则计数器清零；如果采样值≠当前有效值，则计数器加1。然后，判断计数器是否≥上限N（溢出）。如果计数器溢出，则将本次值替换当前有效值，并清计数器。
//优点：对于变化缓慢的被测参数有较好的滤波效果。可避免在临界值附近控制器的反复开/关跳动或显示器上数值抖动。
//缺点：对于快速变化的参数不宜。而且，如果在计数器溢出的那一次采集到的值恰好是干扰值，则会将干扰值当作有效值导入系统。

#define N 20
unsigned char Count;
unsigned char Value;
unsigned char AvoidDitheringFilter()
{
	unsigned char NewValue;
	if(NewValue==Value) 
		Count=0;
	else
		{
			Count++;
			if(Count>N)
				{
					Count=0;
					Value=NewValue;
				}
		}
		return NewValue;
}








//////////////////////排序///////////////////////////
		for (j=0;j<data_num-1;j++) 
	    for (i=0;i<data_num-j;i++)
		    if (arrow[i]>arrow[i+1])
		    {
		        t=arrow[i] ; arrow[i]=arrow[i+1] ; arrow[i+1]=t ;
		    }
		    
//////////////////////////////////////////////////////////////	
/////////////插入算法汇总///////////////////////////////////////
void research_max(s32 adc_cur)
{
	s32 dvalue = adc_cur - adc_pro;
	for(int i=0;i<10;i++)
	{
		if(dvalue>max_arrow[i])
		{
			for(int j=9;j>i;j--)
			{
				max_arrow[j] = max_arrow[j-1];
				position[j]  = position[j-1];
			}
			max_arrow[i] = dvalue;
			position[i] = adc_cur;
			break;
		}
	}
	adc_pro=adc_cur;
}	
/////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////软件复位//////////////////////////////////////////////////////
void sysReset(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}
//////////////////////////////关SWD开启IO功能////////////////////////////////////////////////////////////////
#define inY1 PAin(13)  
#define inY2 PAin(14)  
#define JTAG_SWD_DISABLE		0X02
#define SWD_ENABLE					0X01
#define JTAG_SWD_ENABLE			0X00
void JTAG_Set(u32 mode)
{
	RCC->APB2ENR|=1<<0;
	AFIO->MAPR&=0XF8FFFFFF;
	AFIO->MAPR|=mode<<25;
}
	JTAG_Set(JTAG_SWD_DISABLE);
	PA_IPU(GPIO_Pin_13|GPIO_Pin_14);//配置上拉输入模式

//////////////////////////////////////////////////////////////////////////////////////////

static int t=0;
if(sysTime2-t>50)
	switch_flag=!switch_flag;
t=sysTime2;
show_time=120*1000;
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////CRC校验///////////////////////////////////////////////////////////////
unsigned int crc16(unsigned char * dataIn,unsigned int usLen)
{
    static const unsigned char aucCRCHi[] = {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40
    };
    static const unsigned char aucCRCLo[] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
        0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
        0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
        0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
        0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
        0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
        0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
        0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
        0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
        0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
        0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
        0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
        0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
        0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
        0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
        0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
        0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
        0x41, 0x81, 0x80, 0x40
    };
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    unsigned char iIndex;
    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( dataIn++ );
        ucCRCLo = (unsigned char)( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return (unsigned int)( ucCRCHi << 8 | ucCRCLo );
}
							uint32_t len =  (((uint32_t)USART_RX_BUF[0])<<24)|//区分大小端
															(((uint32_t)USART_RX_BUF[1])<<16)|
															(((uint32_t)USART_RX_BUF[2])<<8)|
															(((uint32_t)USART_RX_BUF[3])<<0);
							uint32_t crc =  (((uint32_t)USART_RX_BUF[4])<<24)|
															(((uint32_t)USART_RX_BUF[5])<<16)|
															(((uint32_t)USART_RX_BUF[6])<<8)|
															(((uint32_t)USART_RX_BUF[7])<<0);
							
							len = len>USART_REC_LEN?USART_REC_LEN:len;
							uint32_t crcGet = crc16(&USART_RX_BUF[8],len);
							if(crcGet==crc)
								{
									.......
								}
////////////////////////////////////////////////////////////////////////////////////////////////
状态机
1 在事件中，判断状态。（横着写，使用较多，无优先顺序）
2 在状态中，判断事件。（竖着写，有优先顺序）
typedef struct 
{
   unsigned char KeyStatusIndex;   //当前的状态索引号
   unsigned char KeyOKStatus;   	//按下确认键时的状态索引号
   unsigned char KeyUpStatus;      //按下向上键时的状态索引号     
   unsigned char KeyDownStatus;    //按下向下键时的状态索引号
   unsigned char KeyBackStatus;     //按下返回键时的状态索引号
   void (*CurrentOperate)(void);      //当前状态应该执行的功能操作
}StatusTab;

unsigned char  KeyFuncIndex=0;    //存放当前的菜单索引
unsigned char  KEY_TYPE=0;		  //当前按下的键值类型
void (*KeyFuncPtr)(void);            //定义按键功能指针

void MenuOperate(void)		  //运行菜单
{
    switch(KEY_TYPE)
	{
		case  1:			  
				KeyFuncIndex=KeyTab[KeyFuncIndex].KeyOKStatus;  //确认按键
				break; 
		case  2:			 
				KeyFuncIndex=KeyTab[KeyFuncIndex].KeyUpStatus;	//向上按键s2
				break; 
	    case  3:		      
		    	KeyFuncIndex=KeyTab[KeyFuncIndex].KeyDownStatus; //向下按键s3
				break; 
		case  4:		
				KeyFuncIndex=KeyTab[KeyFuncIndex].KeyBackStatus; //返回按键
				break; 
		default : 
				break;
				//此处添加按键错误代码
	}
	KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;		//下面是执行按键的操作
	(*KeyFuncPtr)();     //执行当前的按键操作
}	 
/////////////////////////////////////////////////////////////////////////////////////
//1 在事件中，判断状态。（横着写，使用较多，无优先顺序）
//e0事件发生时，执行的函数  
void e0_event_function(int * nxt_state)  
{   //状态为全局变量，时间发生时，根据状态决定动作  
    int cur_state;     
    cur_state = *nxt_state;     
    switch(cur_state)  
    {         
        case s0: //观察表1，在e0事件发生时，s1处为空     
        case s2: //执行a0动作;             
        *nxt_state = s0;  
    }  
}  
  
//e1事件发生时，执行的函数  
void e1_event_function(int * nxt_state)  
{     
    int cur_state;     
    cur_state = *nxt_state;     
    switch(cur_state)  
    {         
        case s0: //观察表1，在e1事件发生时，s1和s2处为空             
            //执行a1动作;             
            *nxt_state = s1;  
    }  
}  
  
//e2事件发生时，执行的函数  
void e2_event_function(int * nxt_state)  
{     
    int cur_state;     
    cur_state = *nxt_state;     
    switch(cur_state)  
    {         
        case s0: //观察表1，在e2事件发生时，s2处为空         
        case s1:             
            //执行a2动作;             
            *nxt_state = s2;   
    }  
}  
/////////////////////////////////////////////////////////////////////////////////
//u8 a=0,b=0,c=0;
 enum
{
  // unsigned char Current_StatusIndex;   //当前的状态索引号
    Sleep_Status,   	//按下确认键时的状态索引号
		Out_Status,      //按下向上键时的状态索引号     
		In_Status,    //按下向下键时的状态索引号
    Adjust_Status,     //按下返回键时的状态索引号
   //void (*CurrentOperate)(void);      //当前状态应该执行的功能操作
}StatusTab=Sleep_Status;
 
 
void MenuOperate(void)//在状态中判断事件
{
	switch(StatusTab)
	{
		case Sleep_Status:
		{
			LED0=!LED0;
			a++;
			delay_ms(500);
			if(a==10) {a=0,StatusTab=Out_Status;}
		}
			break;
		case Out_Status:
		{
			LED1=!LED1;
			b++;
			delay_ms(500);
			if(b==10) {b=0;StatusTab=In_Status;}
		}
			break;
		case In_Status:
			LED0=!LED0;
			LED1=!LED1;
			c++;
			delay_ms(500);
			if(c==10) {c=0;StatusTab=Sleep_Status;}
			break;
		case Adjust_Status:
			
			break;
		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
程序设计时，应使用同一函数调用，入口参数不同，执行不同操作，内部进行状态切换，并返回状态
巧用三目运算


///////////////////////////内部flash，擦一页，写一页////////////////////////////////////////////////////////////////
//
//地址指针，注意指向问题，再操作时可能需要指回原来位置。
//
u8 i,j=0;	
s32 tmpLvdt;					
	//ledO=1;
	times=0;
	s64 dataTmp = 0;
											
	for(i=0;i<200;i++)
	{
		dataTmp += readAds1252();
	}
	tmpLvdt = showFilterY(dataTmp/200,70)/70;
	research_max_init(tmpLvdt);	
adjust_end=0;
while(1)
{
	if(adjust_end==1)break;
}
times=0;
while(1)
{
	s64 dataTmp = 0;					
	for(i=0;i<200;i++)
	{
		dataTmp += readAds1252();
	}
	tmpLvdt = showFilterY(dataTmp/200,70)/70;
	smgShowNum(tmpLvdt);
	refresh();
	FLASH_Unlock();
	if(data_num%256==0)//两个数组地址不在同一页，这里要分擦各自页码
	{
		FLASH_ErasePage(FStartAdd+FPageSize*(usePageStart+i));
		i++;
	}
	FLASH_ProgramWord(tmpaddr1,tmpLvdt);
	FLASH_ProgramWord(tmpaddr2,times);
	tmpaddr1+=4;
	tmpaddr2+=4;
	FLASH_Lock();
	research_max(tmpLvdt);
	data_num++;
	if(times<9000)
	{
		in_flag=0;
	}
	if(times>10000)
	{
		if(in_flag==1)//下降沿
		{
			u32 timesEnd = times-CNC_Delay;
			FLASH_Unlock();
			FLASH_ProgramWord((FStartAdd+FPageSize*usePageStart+0*4),20890);
			FLASH_ProgramWord((FStartAdd+FPageSize*usePageStart+1*4),data_num);
			FLASH_Lock();	
			CNC_Speed = (totalUm*60000/timesEnd)*10;
			saveAllSetData();
			adjust=0;				
			data_num=0;
			showABs=0;
			in_flag=0;
			//ledO=0;
			break;
		}
	}	
}

CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

/////////////////////////////////////////////////////////////////////////////

double binSearch(s32 adc)//返回的是times
{
	s32 lowIndex = 0;
	s32 highIndex = NumberOfData-1;
	s32 midIndex = (lowIndex+highIndex)/2;
	#define lowValue lvdt[lowIndex]
	#define highValue lvdt[highIndex]
	#define midValue lvdt[midIndex]
	if(adc<=lowValue)
	{
		return rong[lowIndex];
	}
	else if(adc>=highValue)
	{
		return rong[highIndex];
	}
	else
	{
		while(midIndex>lowIndex)
		{
			if(midValue>adc)
			{
				highIndex = midIndex;
			}
			else
			{
				lowIndex = midIndex;
			}
			midIndex = (lowIndex+highIndex)/2;
		}
		return (   (double)(adc-lowValue)*(double)(rong[highIndex]-rong[lowIndex])/(double)(highValue-lowValue)+(double)rong[lowIndex]  ) ;
	}
	#undef lowValue
	#undef highValue
	#undef midValue
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

s32 getCenter(s32 um)//0.1um
{
	s32 ttt = ((uint64_t)rong[NumberOfData-1])*CNC_Speed/60000-(totalUm*10-20000);
	if(um>ttt/2+(totalUm*10-20000))
	{
		um = (totalUm*10-20000);
	}
	else if(um>ttt/2)
	{
		um = um-ttt/2;
	}
	else
	{
		um = 0;
	}
	return um;
}
////////////////////////////////////////////////////////////////////////////////////////////
while(key)
{
	sleep(5);
	fun();
}

只执行一次用 do{} while(0);
						
while(t--)
{
	t=某个值;
}

/////////////////////////////////////////////////////////////////////////////////////////
//延时函数
 void delay_ms(int32_t nms) 
 {
  int32_t temp; 
  SysTick->LOAD = 8000*nms; 
  SysTick->VAL=0X00;//清空计数器 
  SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源 
  do 
  { 
       temp=SysTick->CTRL;//读取当前倒计数值 
  }
     while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达 
     
     SysTick->CTRL=0x00; //关闭计数器 
     SysTick->VAL =0X00; //清空计数器 
 }
 ///////////////////////////////////////////////////////////////////////////////////////////
 //595驱动
 //595发送数据
void sendbyte(unsigned char byte,unsigned char byte1)
{
	unsigned char i;        
	LED_EN = 0;
	for(i=0x80;i!=0;i>>=1)
	{
		LED_CK = 0;
		delay_us(10);
		if(byte & i)
		{
			LED_DA = 1;
		}		
		else
		{
			LED_DA = 0;
		}	
		LED_CK = 1;
		delay_us(10);
	}
	for(i=0x80;i!=0;i>>=1)
	{
		LED_CK = 0;
		delay_us(10);
		if(byte1 & i)
		{
			LED_DA = 1;
		}		
		else
		{
			LED_DA = 0;
		}	
		LED_CK = 1;
		delay_us(10);
	}
	LED_EN = 1;
}
///////////////////////////////////////////////////////////////////////////////////
//MC33972驱动
//MC33972
//Cmd  命令字 Switch Status Command 0x00
//Data 命令数据
//Res  返回值,表示引脚状态
unsigned long MC33972Handle(unsigned char Cmd,unsigned short DData)
{
	unsigned char i = 0;
	unsigned long Res = 0;

	MC33972_CLK = 0;
	MC33972_CS = 0;

	for(i=0;i<8;i++)
	{
		MC33972_CLK = 1;
		if(Cmd & 0x80) 
			MC33972_SI = 1;
		else 
			MC33972_SI = 0;
		Cmd <<= 1;
		MC33972_CLK = 0;
		Res <<= 1;
		if(MC33972_SO == 1) 
			Res |= 0x01;
	}
	for(i=0;i<16;i++)
	{
		MC33972_CLK = 1;
		if(DData & 0x8000) 
			MC33972_SI = 1;
		else 
			MC33972_SI = 0;
		DData <<= 1;
		MC33972_CLK = 0;
		Res <<= 1;
		if(MC33972_SO == 1) 
			Res |= 0x01;
	}
	MC33972_CS = 1;
	return(Res);
}
//////////////////////////////////////////////////////////////////////////
//33972读取状态和初始化
//MC33972初始化函数
void MC33972_Init(void)
{
	MC33972Handle(0x01,0xFF); //设置SP0-SP7为 高电平有效
	MC33972Handle(0x09,0xFF); //设置SP0-SP7为 高阻状态	
	MC33972Handle(0x05,0x3FFF); //设置SG0-SG13湿润电流为16mA
	MC33972Handle(0x08,0x3FFF); //设置SG0-SG13湿润电流定时器为20ms	 
	MC33972Handle(0x7f,0x0000);	//复位
}
		tmp = MC33972Handle(0x00,0x0000);
		
//读取开关状态
void readSwitchState(void)
{
	Keystate.CTRL_MODE = (tmp>>0&0x01);
	Keystate.OP_MODE = (tmp>>1&0x01);
	Keystate.Ground_MODE = (tmp>>2&0x01);
	Keystate.UP_MODE = (tmp>>3&0x01);
	Keystate.DOWN_MODE = (tmp>>4&0x01);
	Keystate.KEY_PRESS_FLAG = Keystate.UP_MODE|Keystate.DOWN_MODE;
	Keystate.Extend_key1 = (tmp>>5&0x01);//水平归零
	Keystate.Extend_key2 = (tmp>>6&0x01);//精度级别
	Keystate.Pressure_FLAG = (tmp>>11&0x01);//；力控压力信号
}

//浮点型转字符串
void show_char(u16 x, u16 y,u8 size ,double num)
{
	char str[8];
	sprintf(str,"%7.3f",num);
	lcdShowString(x,y,size,(u8*)str,RED,WHITE);
}
