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
static scene current(scene_size);
static scene fade_to(scene_size);
static scene black(scene_size);
static scene temp(scene_size);
static bool fade_flag=false;
static int fade_time=2000;
static int master_level=255;
static int edit_rot=0;
static int edit_rot_flag=0;
static int edit_channel=0;
static int edit_flag=0;
static int edit_change=0;


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
    pinMode(21, INPUT);
    pinMode(22, INPUT);
    pinMode(23, INPUT);
    pinMode(26, INPUT);
    pinMode(6, INPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(1, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    
    digitalWrite(24, HIGH);
    digitalWrite(25, HIGH);
    digitalWrite(27,HIGH);
    digitalWrite(28,HIGH);
    digitalWrite(29,LOW);

    pullUpDnControl(8, PUD_UP);
    pullUpDnControl(3, PUD_UP);
    pullUpDnControl(7, PUD_UP);
    pullUpDnControl(0, PUD_UP);
    pullUpDnControl(2, PUD_UP);
    pullUpDnControl(21, PUD_UP);
    pullUpDnControl(26, PUD_UP);
    pullUpDnControl(6, PUD_UP);
    pullUpDnControl(1, PUD_UP);
}

//Interrupt funcions for the buttons
void button1(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 1 Pressed\n");
        if(edit_flag==0){
        fade_to=sceneA;
        fade_flag=true;
        }else if(edit_flag==2){
            //sceneA=current;
            //sceneA.save("scenes/sceneA.txt");
            edit_flag=0;
        }
        last_interrupt=interrupt;
    }
}

void button2(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 2 Pressed\n");
        if(edit_flag==0){
        fade_to=sceneB;
        fade_flag=true;
        }else if(edit_flag==2){
            //sceneB=current;
            //sceneB.save("scenes/sceneB.txt");
            edit_flag=0;
        }
        last_interrupt=interrupt;
    }
}

void button3(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 3 Pressed\n");
        if(edit_flag==0){
        fade_to=sceneC;
        fade_flag=true;
        }else if(edit_flag==2){
            //sceneC=current;
            //sceneC.save("scenes/sceneC.txt");
            edit_flag=0;
        }
        last_interrupt=interrupt;
    }
}

void button4(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 4 Pressed\n");
        if(edit_flag==0){
        fade_to=sceneD;
        fade_flag=true;
        }else if(edit_flag==2){
            sceneD=current;
            sceneD.writable=true;
            sceneD.save("scenes/sceneD.txt");
            edit_flag=0;
        }
    last_interrupt=interrupt;
    }
}

void button5(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 5 Pressed\n");
        if(edit_flag==0){
            fade_to=sceneE;
            fade_flag=true;
        }else if(edit_flag==2){
            sceneE=current;
            sceneE.writable=true;
            sceneE.save("scenes/sceneE.txt");
            edit_flag=0;
        }
    last_interrupt=interrupt;
    }
}

void button6(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=300){
        printf("Button 6 Pressed\n");
        if (fade_time==2000){
            fade_time=0;
            digitalWrite(25, LOW);
            digitalWrite(27, LOW);
            digitalWrite(28, LOW);
        }else if(fade_time==0){
            fade_time=500;
            digitalWrite(25, HIGH);
        }else if(fade_time==500){
            fade_time=1000;
            digitalWrite(27, HIGH);
        }else{
            fade_time=2000;
            digitalWrite(28, HIGH);
        }
        printf("Fade now %d ms\n", fade_time);
        last_interrupt=interrupt;
    }
}

void rotary_master(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=35){
        if(edit_flag==0){
            if(digitalRead(22)) {
                if(master_level < 255) master_level+=15;
            }else{
                if(master_level > 0) master_level-=15;
            }
            fade_flag=true;
        }
    }
    last_interrupt=interrupt;
}

void rotary_edit(void){
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=35){
        if(edit_flag==1){
            if(digitalRead(4)==1) {
                edit_rot=1;
            }else{
                edit_rot=-1;
            }
            edit_rot_flag=1;
        }
    }
    last_interrupt=interrupt;
}

void edit_mode(void){
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        if(edit_change==1){
            edit_change=0;
            current=temp;
            temp=black;
            edit_rot=0;
            edit_rot_flag=1;
        }else{
            edit_change=1;
            temp=current;
            current=black;
            edit_rot=0;
            edit_rot_flag=1;
        }
    }
    last_interrupt=interrupt;
}

void edit(void){
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        if(edit_flag==1){
            digitalWrite(29, LOW);
            edit_flag=2;
            cout << "save to? press again to cancel..";

            cout << endl;
        }else if(edit_flag==2){
            edit_flag=0;
            cout << "save cancelled...";
            cout << endl;
        }else{
            digitalWrite(29, HIGH);
            edit_flag=1;
            edit_change=0;
            edit_rot_flag=1;
            cout << "edit mode started...";
            cout << endl;
        }
    }
    last_interrupt=interrupt;
}


//initialize interrupt functions
void interrupt_init(void){
    wiringPiISR(8, INT_EDGE_FALLING, &button1);
    wiringPiISR(3, INT_EDGE_FALLING, &button2);
    wiringPiISR(7, INT_EDGE_FALLING, &button3);
    wiringPiISR(0, INT_EDGE_FALLING, &button4);
    wiringPiISR(2, INT_EDGE_FALLING, &button5);
    wiringPiISR(26, INT_EDGE_FALLING, &button6);
    wiringPiISR(23, INT_EDGE_FALLING, &rotary_master);
    wiringPiISR(5, INT_EDGE_FALLING, &rotary_edit);
    wiringPiISR(6, INT_EDGE_FALLING, &edit);
    wiringPiISR(1, INT_EDGE_FALLING, &edit_mode);
    
}


int main(){
    gettimeofday(&last_interrupt, NULL);
    gpio_init();
    interrupt_init();
    digitalWrite(24, HIGH);
    if(!testmode) open_dev();
    dmx.size=scene_size+1;
    dmx.channel[0]=0x00;
    current=sceneA;
    fade_flag=true;
    fade_to=sceneA;
    scene diff(scene_size);
    while(1){
        if(edit_flag==1){
            if(edit_rot_flag==1){
                //cout << std::to_string(edit_rot);
                //cout << endl;
                if(edit_change==1){

                    if(edit_channel < scene_size && edit_channel > 0)
                    {
                        edit_channel+=edit_rot;
                    }else if(edit_channel==0 && edit_rot==1){
                        edit_channel=1;
                    }else if(edit_channel==scene_size && edit_rot==-1){
                        edit_channel=scene_size-1;
                    }
                    current=black;
                    current.channel[edit_channel]=255;
                    //cout << "Channel to edit: " + std::to_string(edit_channel);
                    //cout << endl;
                }else{
                    if(current.channel[edit_channel]<255 && current.channel[edit_channel]>0) {
                        current.channel[edit_channel]+=edit_rot*15;
                    } else if(current.channel[edit_channel]==0 && edit_rot==1){
                        current.channel[edit_channel]=15;
                    } else if(current.channel[edit_channel]==255 && edit_rot==-1){
                        current.channel[edit_channel]=240;
                    }
                    //cout << "Value of channel " + std::to_string(edit_channel) + " ist " + std::to_string(edit.channel[edit_channel]);
                    //cout << endl;
                }
                dmx=current.to_dmx();
                if(!testmode) write_buffer();
                current.print_g(edit_channel);
                printf("%d\n", edit_channel);
                edit_rot_flag=0;        
            }
            
        }
        if(fade_flag){
            fade_flag=false;
            int steps=fade_time/time_per_step;
            int time=fade_time;
            diff=fade_to-current;
            diff=diff/steps;
            for(int i=0;i<time;i+=time_per_step){
                current=current+diff;
                current.master=master_level;
                dmx=current.to_dmx();
                if(!testmode) write_buffer();
                current.print_g();
                msleep(time_per_step-15);
            }
        }

    }
    return 0;
}
