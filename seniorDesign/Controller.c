//L2 brake
//R2 accelerate
//https://github.com/adafruit/Adafruit-Retrogame
//https://github.com/jacobfk20/EmulationStation/tree/Release-beta
//http://aloshi.com/
//  ~/.emulationstation/es_input.cfg

// https://gist.github.com/neolao/e4d5960c4ee1e8ed5291 !!!!!!
// https://gist.github.com/neolao/bcbe33bef7eaab062c20
//https://github.com/recalbox/recalbox-os/wiki/Configure-N64-controllers-(EN)

#include <wiringPi.h> //http://wiringpi.com/download-and-install/
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

//gcc -o Controller Controller.c -lwiringPi -pthread -lcrypt -lm -lrt
// sudo ./blink

bool pressKeys(int delayInput)
{
    static int keyboardFd = -1;
    int rd,n;
    bool ret = false;

    DIR *dirp;
    struct dirent *dp;
    regex_t kbd;

    char fullPath[1024];
    static char *dirName = "/dev/input/by-id";
    
    int result;
    struct input_event forcedKey;


    // Send ls<ret>
    uint16_t keys[] = {KEY_L,KEY_S,KEY_ENTER,0}; 
	// replace with uint16_t keys[] = {KEY_WHATEVER_BREAK_IS}
    int index;

    /* Find the device with a name ending in "event-kbd" */

    if(regcomp(&kbd,"event-kbd",0)!=0)
    {
        printf("regcomp for kbd failed\n");
        return false;

    }
    if ((dirp = opendir(dirName)) == NULL) {
        perror("couldn't open '/dev/input/by-id'");
        return false;
    }


    do {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL) 
        {
            printf("readdir (%s)\n",dp->d_name);
            if(regexec (&kbd, dp->d_name, 0, NULL, 0) == 0)
            {
                printf("match for the kbd = %s\n",dp->d_name);
                sprintf(fullPath,"%s/%s",dirName,dp->d_name);
                keyboardFd = open(fullPath,O_WRONLY | O_NONBLOCK);
                printf("%s Fd = %d\n",fullPath,keyboardFd);
                printf("Getting exclusive access: ");
                result = ioctl(keyboardFd, EVIOCGRAB, 1);
                printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");

            }


        }
    } while (dp != NULL);

    closedir(dirp);


    regfree(&kbd);


    /* Now write some key press and key release events to the device */


    index = 0;
    while (delayInput >0)
    {
		while(keys[index] != 0)
		{
		
			forcedKey.type = EV_KEY;
			forcedKey.value = 1;    // Press
			forcedKey.code = keys[index];
			gettimeofday(&forcedKey.time,NULL);
				
			n = write(keyboardFd,&forcedKey,sizeof(struct input_event));
			printf("n=%d\n",n); // can be commented later

			forcedKey.type = EV_KEY;
			forcedKey.value = 0 ;   // Release
			forcedKey.code = keys[index];
			gettimeofday(&forcedKey.time,NULL);       
				 
			n = write(keyboardFd,&forcedKey,sizeof(struct input_event));
			printf("n=%d\n",n); // can be commented later
		
			index += 1;
		}
		delayInput --;
	}
    close(keyboardFd);

    return(true);

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
	 
	 pressKeys(sumVals);
	 sumVals = 0;
  }
  return 0;
}

