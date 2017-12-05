
// to run in baackground
// moduinKeyboard&
// to check if running type jobs -l (thats the letter l)
// Need to know the PID to kill which you can get by typing (which is returned by jobs)
// kill -KILL PID


#include <linux/uinput.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

int pressKeys(int delayInput)
{
   struct uinput_setup usetup;
   /*Open the file directories */
   int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
   if(fd){
		/*
		* The ioctls below will enable the device that is about to be
		* created, to pass key events, in this case the space key.
		*/
	   ioctl(fd, UI_SET_EVBIT, EV_KEY);
	   ioctl(fd, UI_SET_KEYBIT, KEY_SPACE);
	   
	   /*to set ours we would need the line */
	   /*ioctl(fd,UI,SET,KEYBIT, KEY_P); */
	   
	   /*Setup the Device we are using with its info like vendor product etc */
	   memset(&usetup, 0, sizeof(usetup));
	   usetup.id.bustype = BUS_USB;
	   usetup.id.vendor = 0x1; /* sample vendor */
	   usetup.id.product = 0x2; /* sample product */
	   upsetup.id.version = 2;
	   strcpy(usetup.name, "Example device");

	   ioctl(fd, UI_DEV_SETUP, &usetup);
	   ioctl(fd, UI_DEV_CREATE);

	   /*
		* On UI_DEV_CREATE the kernel will create the device node for this
		* device. We are inserting a pause here so that userspace has time
		* to detect, initialize the new device, and can start listening to
		* the event, otherwise it will not notice the event we are about
		* to send. This pause is only needed in our example code!
		*/
	   sleep(1);

	   /* Key press, report the event, send key release, and report again */
	    while(delayInput > 0){
		   emit(fd, EV_KEY, KEY_SPACE, 1);
		   emit(fd, EV_SYN, SYN_REPORT, 0);
		   emit(fd, EV_KEY, KEY_SPACE, 0);
		   emit(fd, EV_SYN, SYN_REPORT, 0);
		   printf("emitting space key");
		   delayInput--;
		}

	   /*
		* Give userspace some time to read the events before we destroy the
		* device with UI_DEV_DESTOY.
		*/
	   sleep(1);

	   ioctl(fd, UI_DEV_DESTROY);
	   
	   close(fd); 
	   status = 0;
   }
   else{
	   printf("Error opening keyboard; %s \n", strerror(errno));
	   status = -1;
   }
   return 0;
}

int main ( void ) {
	
  wiringPiSetup();
  int pin1 = 7; //pin 7
  int pin2 = 0; //pin 11
  int pin3 = 2; //pin 13
  int pin4 = 1; //pin 12
  int pin5 = 3; // pin 15
  int pin6 = 4; //pin 16
  int pin7 = 5; //pin 18
  int pin8 = 6; // pin 22
  
  int pinArr[8] = {pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8};
  int sumVals = 0;
  if (wiringPiSetup() == -1) {
    printf( "Setup didn't work... Aborting." );
    exit (1);
  }
  for( int j =0; j<8; j++){
	pinMode(pinArr[j], INPUT);
  }
  while(1){
	 for( int j =0; j<8; j++){
		sumVals += digitalRead(pinArr[j]);
	 }	
	 sumVals = 3;
	 pressKeys(sumVals);
	 sumVals = 0;
  }
  return 0;
}