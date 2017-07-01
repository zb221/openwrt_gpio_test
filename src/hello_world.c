
#include <linux/reboot.h>

#include <stdio.h>             
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

#define RESET_BIT (4)

#define GPIO_DEV	"/dev/gpio"

#define	RALINK_GPIO6332_SET_DIR		0x51
#define RALINK_GPIO6332_SET_DIR_IN	0x13
#define RALINK_GPIO6332_SET_DIR_OUT	0x14
#define	RALINK_GPIO6332_READ		0x52
#define	RALINK_GPIO6332_WRITE		0x53
#define	RALINK_GPIO6332_SET		0x22
#define	RALINK_GPIO6332_CLEAR		0x32

#define	RALINK_GPIO9564_SET_DIR		0x61
#define RALINK_GPIO9564_SET_DIR_IN	0x15
#define RALINK_GPIO9564_SET_DIR_OUT	0x16
#define	RALINK_GPIO9564_READ		0x62
#define	RALINK_GPIO9564_WRITE		0x63
#define	RALINK_GPIO9564_SET		0x23
#define	RALINK_GPIO9564_CLEAR		0x33

/*
 * ioctl commands
 */
#define	RALINK_GPIO_SET_DIR		0x01
#define RALINK_GPIO_SET_DIR_IN		0x11
#define RALINK_GPIO_SET_DIR_OUT		0x12
#define	RALINK_GPIO_READ		0x02
#define	RALINK_GPIO_WRITE		0x03
#define	RALINK_GPIO_SET			0x21
#define	RALINK_GPIO_CLEAR		0x31
#define	RALINK_GPIO_READ_INT		0x02 //same as read
#define	RALINK_GPIO_WRITE_INT		0x03 //same as write
#define	RALINK_GPIO_SET_INT		0x21 //same as set
#define	RALINK_GPIO_CLEAR_INT		0x31 //same as clear
#define RALINK_GPIO_ENABLE_INTP		0x08
#define RALINK_GPIO_DISABLE_INTP	0x09
#define RALINK_GPIO_REG_IRQ		0x0A
#define RALINK_GPIO_LED_SET		0x41

enum {
	gpio_in,
	gpio_out,
};
enum {
	gpio3100,
	gpio6332,
	gpio9564,
};


int gpio_set_dir(int r, int dir)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDWR);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (dir == gpio_in) {
			req = RALINK_GPIO_SET_DIR_IN;
	}
	else {
			req = RALINK_GPIO_SET_DIR_OUT;
	}
	if (ioctl(fd, req, 0xffffffff) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


int gpio_write_int(int r, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDWR);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	req = RALINK_GPIO_WRITE;
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


void gpio_test_write(void)
{
	int i = 0;
	int time = 10;

	//set gpio direction to output
	gpio_set_dir(gpio3100, gpio_out);

       while(1){
	   		//turn off LEDs
			printf("led off\n");

			sleep(1);
               	gpio_write_int(gpio3100, 0xffffffff);

               	sleep(1);
               	//turn on all LEDs
			printf("led on\n");
               	gpio_write_int(gpio3100, 0);

			//reset 
			reset_service();

       }
}


int reset_service(void)
{
	int fd;
	unsigned int gpioState;
	int press_cnt;

	fd = open("/dev/gpio", O_RDWR);
	if (fd < 0) 
	{
		perror("Open Error!\n");
		return -1;
	}

	if (ioctl(fd, RALINK_GPIO6332_SET_DIR_IN, (0x1 << RESET_BIT)) == -1)
	{
		printf("set direction of reset failed\n");
		return -1;
	}

	press_cnt = 0;

 		if (ioctl(fd, RALINK_GPIO6332_READ, &gpioState) == -1)
 		{
 			perror("Read reset state failed\n");
 		}
 		else
 		{
 			if (gpioState & (0x1 << RESET_BIT))
 			{
 				if (press_cnt >= 5)
 				{
 					system("rm /overlay/* -rf");
 					reboot(LINUX_REBOOT_CMD_RESTART);
 				}
 				press_cnt = 0;
 			}
 			else
 			{
 				press_cnt++;
 			}
 		}
 		sleep(1);

	return 0;
}


int main(int argc, char *argv[])
{
	gpio_test_write();
	
	return 0;
}

