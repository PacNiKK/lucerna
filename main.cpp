#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "scene.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
using namespace std;

//settings
static const char* dev = "/dev/dmx0";       //dmx device location
static dmx_buffer dmx;
static const int scene_size=24;
static const int time_per_step=20;
static bool testmode=true;
static int fd = -1;


//open DMX device
static void open_dev(void)
{
        fd = open(dev, O_WRONLY);
        if (fd < 0) {
                perror("open");
                exit(-1);
        }
}

//write buffer to DMX device
static void write_buffer(void)
{
        if (write(fd, dmx.channel, 513) < 0) {
                perror("write");
                exit(-1);
        }
}



//static variables for current state
//static int fade_step;
//static string fade_to="current";
static scene current(scene_size);
static scene fade_to(scene_size);
static bool fade_flag=false;
static int fade_time=2000;
static int master_level=255;

//timestructs used for debouncing buttons
struct timeval last_interrupt;
struct timeval interrupt;

//initialize and load scenes
static scene sceneA("scenes/sceneA.txt", 24, FALSE);
static scene sceneB("scenes/sceneB.txt", 24, FALSE);
static scene sceneC("scenes/sceneC.txt", 24, FALSE);
static scene sceneD("scenes/sceneD.txt", 24, TRUE);
static scene sceneE("scenes/sceneE.txt", 24, TRUE);
static scene sceneF("scenes/sceneF.txt", 24, TRUE);

//sleep for m milliseconds
void msleep(int m){
    usleep(1000*m);
}

//initialize GPIO for Buttons and LEDs
void gpio_init(void) {
    wiringPiSetup();
    pinMode(8, INPUT);
    pinMode(3, INPUT);
    pinMode(7, INPUT);
    pinMode(0, INPUT);
    pinMode(2, INPUT);
    pullUpDnControl(8, PUD_UP);
    pullUpDnControl(3, PUD_UP);
    pullUpDnControl(7, PUD_UP);
    pullUpDnControl(0, PUD_UP);
    pullUpDnControl(2, PUD_UP);
}

//Interrupt funcions for the buttons
void button1(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=400){
        printf("Button 1 Pressed\n");
        fade_to=sceneA;
        fade_flag=true;
        last_interrupt=interrupt;
    }
}

void button2(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=400){
        printf("Button 2 Pressed\n");
        fade_to=sceneB;
        fade_flag=true;
        last_interrupt=interrupt;
    }
}

void button3(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=400){
        printf("Button 3 Pressed\n");
        fade_to=sceneC;
        fade_flag=true;
        last_interrupt=interrupt;
    }
}

void button4(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=400){
        printf("Button 4 Pressed\n");
        fade_to=sceneD;
        fade_flag=true;
    last_interrupt=interrupt;
    }
}

void button5(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=400){
        printf("Button 5 Pressed\n");
        if (fade_time==2000){
            fade_time=0;
        }else if(fade_time==1000){
            fade_time=2000;
        }else{
            fade_time+=500;
        }
        printf("Fade now %d ms\n", fade_time);
        last_interrupt=interrupt;
    }
}

//initialize interrupt functions
void interrupt_init(void){
    wiringPiISR(8, INT_EDGE_FALLING, &button1);
    wiringPiISR(3, INT_EDGE_FALLING, &button2);
    wiringPiISR(7, INT_EDGE_FALLING, &button3);
    wiringPiISR(0, INT_EDGE_FALLING, &button4);
    wiringPiISR(2, INT_EDGE_FALLING, &button5);
}


int main(){
    gettimeofday(&last_interrupt, NULL);
    gpio_init();
    interrupt_init();
    if(!testmode) open_dev();
    dmx.size=scene_size+1;
    dmx.channel[0]=0x00;
    current=sceneA;
    fade_flag=true;
    fade_to=sceneA;
    scene diff(scene_size);
    while(1){
        if(fade_flag){
            fade_flag=false;
            int steps=fade_time/time_per_step;
            int time=fade_time;
            diff=fade_to-current;
            diff=diff/steps;
            for(int i=0;i<time;i+=time_per_step){
                current=current+diff;
                current=(current*master_level)/255;
                dmx=current.to_dmx();
                if(!testmode) write_buffer();
                current.print_g();
                msleep(time_per_step-15);
            }

        }
    }
    return 0;
}
