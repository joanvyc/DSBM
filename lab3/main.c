#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)
#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000)

int mem_fd;
void * gpio_map;
volatile unsigned *gpio;

#define GPIO_setValue(pin_num, value)\
	*(gpio+7+(-(!value)&3)) = 1 << pin_num

#define GPIO_readValue(pin_num)\
	*(gpio+13)&=1<<pin_num

#define GPIO_MODE_Output(pin_num) \
	*(gpio+(pin_num/10)) &= ~(7<<((pin_num%10)*3));\
	*(gpio+(pin_num/10)) |=  (1<<((pin_num%10)*3));\
	GPIO_setValue(pin_num,0)

#define GPIO_MODE_Input(pin_num) \
	*(gpio+(pin_num/10)) &= ~(7<<((pin_num%10)*3))


#define CLK_PERIOD 1<<21
#define CLK  11
#define Dout 9
#define Din  10
#define CS   8

enum state_t {CS_UP, CS_DOWN, SB, CONF, D2, D1, D0, W1, W2, READ, IDLE};

static enum state_t STATE;
int GPIO_init(){

	if((mem_fd = open("/dev/mem", O_RDWR|O_SYNC))<0){
		printf("can't open /dev/mem \n");
		return -1;
	}
	gpio_map = mmap(NULL,BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE);
	close(mem_fd);
	if(gpio_map == MAP_FAILED){
		printf("mmap error %d\n", (int)gpio_map);
		return -1;
	}
	gpio = (volatile unsigned *) gpio_map;
	//*(gpio)=0;
	return 0;
}

int SpiRead() {
	int read = 0;	
	int contador = 0;
	int clk_value = 0;	
	int read_count = 0;

	while (1) {	
		if(contador>=CLK_PERIOD){
			clk_value ^= 1;
			GPIO_setValue( CLK, clk_value );
			if (!clk_value) {
				switch( STATE ){
					case READ:
						if ( read_count++ >= 12){ 
							STATE=CS_UP; 
							read_count = 0;
							printf("Value read: %d\n", read); 
							read = 0;
						}
						break;
					default:
						STATE = (STATE + 1);	
						break;
				}	
			//	printf("State %d\n", STATE);

			} else {
				switch( STATE ){
					case CS_UP:
						GPIO_setValue( CS, 1);
						break;
					case CS_DOWN:
						GPIO_setValue( CS, 0);
						break;
					case SB:
						GPIO_setValue( Din,1);
						break;
					case CONF:
						GPIO_setValue( Din,1); 	
						break;
					case D2:
						GPIO_setValue( Din,0);
						break;
					case D1:
						GPIO_setValue( Din,0);
						break;
					case D0:
						GPIO_setValue( Din,0);
						break;
					case READ:
						GPIO_setValue( Din,0);
						read <<= 1;
						int tmp = ( GPIO_readValue( Dout ) );
						printf("Pin read %d\n", tmp); 
						read  |= tmp; //( GPIO_readValue( Dout ));
						break;
					default:
						break;
				}

			}
			contador = 0;
		}	
		contador++;
	}
	GPIO_setValue( CS, 1 );
	
}

int setup () {
	GPIO_MODE_Output(CLK);
	GPIO_MODE_Output(Din);
	GPIO_MODE_Output(CS);

	GPIO_MODE_Input(Dout);
}

int main() {

	GPIO_init();	
	
	setup();

	SpiRead();	
}
