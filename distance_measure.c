
//port f regs
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
//**************************************************************************
//systic regs
#define NVIC_ST_CTRL_R (*((volatile unsigned long*)0xE000E010))
#define NVIC_ST_RELOAD_R (*((volatile unsigned long*)0xE000E014))
#define NVIC_ST_CURRENT_R (*((volatile unsigned long*)0xE000E018))
//************************************************************************
//port A regs
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PUR_R        (*((volatile unsigned long *)0x40004510))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
//**************************************************************************
//************************************************************************
//port B regs
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
//**************************************************************************
#define SW1          (*((volatile unsigned long *)0x40025040))
#define PF1          (*((volatile unsigned long *)0x40025008))
#define PF2          (*((volatile unsigned long *)0x40025010))
#define PF3          (*((volatile unsigned long *)0x40025020))
#define Trig	(*((volatile unsigned long *)0x40004100)) //PA6
#define Echo	(*((volatile unsigned long *)0x40004200)) //PA7
#define HUNDREDS (*((volatile unsigned long *)0x400040F0)) //PA5-PA2    output to the first decoder
#define TENTHS (*((volatile unsigned long *)0x4000503C)) //PB3-PB0	output to the second decoder
#define ONES (*((volatile unsigned long *)0x400053C0)) //PB7-PB4	output to the third decoder
#define clk_period  0.0625//us

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
//void SysCtlDelay(int);
//int SysCtlClockGet(void);
//void PORTF_init(void);
//unsigned long SW1;
unsigned long start;
unsigned long end;
unsigned long tens,huns,ones;
unsigned long num_cycles;
double time,meters,distance;
double fraction;
void portfInit(void)
{
	GPIO_PORTF_AMSEL_R=0x00;
	GPIO_PORTF_PCTL_R=0x00000000;
	GPIO_PORTF_DIR_R=0x0F;
	GPIO_PORTF_AFSEL_R=0x00;
	GPIO_PORTF_PUR_R=0x11;
	GPIO_PORTF_DEN_R=0x1F;
}

void portaInit(void)
{
	GPIO_PORTA_LOCK_R = 0x4C4F434B; 
	GPIO_PORTA_CR_R = 0xFC;
	GPIO_PORTA_AMSEL_R=0x00;
	GPIO_PORTA_PCTL_R=0x00000000;
	GPIO_PORTA_DIR_R&=~0x80;
	GPIO_PORTA_DIR_R|=0x7C;
	GPIO_PORTF_AFSEL_R=0x00;
	GPIO_PORTA_PUR_R=0x00;
	GPIO_PORTA_DEN_R=0xFC;
	
}

void portbInit(void)
{
	GPIO_PORTB_LOCK_R = 0x4C4F434B; 
	GPIO_PORTB_CR_R = 0xFF;
	GPIO_PORTB_AMSEL_R=0x00;
	GPIO_PORTB_PCTL_R=0x00000000;
	GPIO_PORTB_DIR_R|=0xFF;
	GPIO_PORTB_AFSEL_R=0x00;
	GPIO_PORTB_PUR_R=0x00;
	GPIO_PORTB_DEN_R=0xFF;
	
}

void systickInit(void)
{
	NVIC_ST_CTRL_R=0;
	NVIC_ST_RELOAD_R=0x00FFFFFF;
	NVIC_ST_CURRENT_R=0;
	NVIC_ST_CTRL_R=0x00000005;
}

void systickWait(unsigned long delay)
{
	NVIC_ST_RELOAD_R=delay-1;
	NVIC_ST_CURRENT_R=0;
	while ((NVIC_ST_CTRL_R & 0x00010000) == 0);
}

void wait10ms(unsigned long delay)
{
	unsigned long i;
	for (i=0;i<delay;i++)
	{
		systickWait(800000);
	}
}

void wait10us(unsigned long delay)
{
	unsigned long i;
	for (i=0;i<delay;i++)
	{
	systickWait(800);
	}
}

int main(void){ 
	//portaInit();
				unsigned long delay;
		SYSCTL_RCGC2_R |= 0x00000023;
	delay=SYSCTL_RCGC2_R;
	portfInit();
	portaInit();
	portbInit();
	systickInit();
	HUNDREDS=0x00;
	TENTHS=0x00;
	ONES=0x00;
	while(1)
	{
		if (!SW1)
		{
		Trig=0x40;
		wait10us(1);
		Trig=0x00;
		systickInit();
		while((Echo & 0x80) == 0);
		start=NVIC_ST_CURRENT_R;
		while ((Echo & 0x80) != 0);
		end=NVIC_ST_CURRENT_R;
		PF3=0x08;
		num_cycles=(start-end) & 0x00FFFFFF;
		time=num_cycles*clk_period;
		distance=time/58;
		huns = distance/100;
		tens = (distance/10)-(huns*10);
		ones = distance/10;
		ones = distance - (ones*10);
			//calculating fraction
		fraction=distance-(unsigned long)distance;	
			if(fraction>=0.5)
			{
			ones++;
			}
		HUNDREDS=huns<<2;  //display the distance on 3 seven segment displays
		TENTHS=tens;
		ONES=ones<<4;
	}
	}
}

