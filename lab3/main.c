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
	*(gpio+7+(-value&3)) = 1 << pin_num

#define GPIO_readValue(pin_num)\
	*(gpio+13)&=1<<pin_num

#define GPIO_MODE_Output(pin_num) \
	*(gpio+(pin_num/10)) &= ~(7<<((pin_num%10)*3));\
	*(gpio+(pin_num/10)) |=  (1<<((pin_num%10)*3));\
	GPIO_setValue(pin_num,0)

#define GPIO_MODE_Input(pin_num) \
	*(gpio+(pin_num/10)) &= ~(7<<((pin_num%10)*3))


#define CLK_PERIOD 1<<15
#define CLK  26
#define Dout 19
#define Din  13
#define CS   6

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
	
	int contador = 0;
	int clk_value = 0;	

	GPIO_setValue( CS, 1 );
	for (int i = 0; i < CLK_PERIOD; i++);
	GPIO_setValue( CS, 0 );
	while (1) {	
		if(contador>=CLK_PERIOD){
			clk_value ^= 1;
			GPIO_setValue( CLK, clk_value );
			if (clk_value) {

			} else {

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
