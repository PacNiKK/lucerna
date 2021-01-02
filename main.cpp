#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <mcp23017.h>
#include <pcf8591.h>
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
static const int scene_size=24;             //channels per scene
static const int time_per_step=20;          //ms per step of fade
static bool testmode=true;                  //true if no device connected (output on console)
static int fd = -1;

//pin variables inputs
int switch1=135;
int switch2=134;
int switch3=133;
int switch4=132;
int switch5=131;
int switch6=130;
int switch7=129;
int switch8=128;
int switch_fade=103;
int switch_rotary=102;
int rotary_dt=101;
int rotary_clk=100;

//pins outputs
int led1=120;
int led2=121;
int led3=122;
int led4=123;
int led5=124;
int led6=125;
int led7=126;
int led8=127;
int led_fade_switch=108;
int led_fade1=107;
int led_fade2=106;
int led_fade3=105;
int led_fade4=104;

//open DMX device
static void open_dev(void)
{
    if(!testmode){
        fd = open(dev, O_WRONLY);
        if (fd < 0) {
                perror("open");
                exit(-1);
        }
    }
}

//write buffer to DMX device
static void write_buffer(void)
{
    if(!testmode){
        if (write(fd, dmx.channel, 513) < 0) {
                perror("write");
                exit(-1);
        }
    }
}



//static variables for current state
static scene current(scene_size);
static scene fade_to(scene_size);
static scene black(scene_size);
static scene temp(scene_size);
static bool fade_flag=false;
static int fade_time=2000;
//static int master_level=255;
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
    mcp23017Setup(120, 0x20);
    mcp23017Setup(100, 0x21);
    pcf8591Setup(80, 0x4f);
    
    //0x20
    for(int i = 0 ; i < 8 ; ++i)
    pinMode(120+i,OUTPUT);
    for(int i = 0 ; i < 8 ; ++i)
    {
        pinMode(128+i, INPUT);
        pullUpDnControl(128+i, PUD_UP);
    }
    //0x21
    for(int i = 0 ; i < 4 ; ++i)
    {
        pinMode(100+i,INPUT);
        pullUpDnControl(100+i, PUD_UP);
    }
    for(int i=0 ; i < 8 ; ++i)
    pinMode(104+i, OUTPUT);
    for(int i = 0 ; i < 3 ; ++i)
    {
        pinMode(112+i, INPUT);
        pullUpDnControl(112+i, PUD_UP);
    }

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
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=200){
        printf("Button 6 Pressed\n");
        if(edit_flag==0){
            fade_to=sceneF;
            fade_flag=true;
        }else if(edit_flag==2){
            sceneF=current;
            sceneF.writable=true;
            sceneF.save("scenes/sceneF.txt");
            edit_flag=0;
        }
    last_interrupt=interrupt;
    }
}

void button_fade(void) {
    gettimeofday(&interrupt,NULL);
    if((interrupt.tv_sec * 1000 + interrupt.tv_usec / 1000)-(last_interrupt.tv_sec * 1000 + last_interrupt.tv_usec / 1000)>=300){
        printf("Button 6 Pressed\n");
        if (fade_time==2000){
            fade_time=0;
            digitalWrite(led_fade2, LOW);
            digitalWrite(led_fade3, LOW);
            digitalWrite(led_fade4, LOW);
        }else if(fade_time==0){
            fade_time=500;
            digitalWrite(led_fade2, HIGH);
        }else if(fade_time==500){
            fade_time=1000;
            digitalWrite(led_fade3, HIGH);
        }else{
            fade_time=2000;
            digitalWrite(led_fade4, HIGH);
        }
        printf("Fade now %d ms\n", fade_time);
        last_interrupt=interrupt;
    }
}

/*void rotary_master(void) {
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
}*/


//initialize interrupt functions
/*void interrupt_init(void){
    wiringPiISR(switch1, INT_EDGE_FALLING, &button1);
    wiringPiISR(switch2, INT_EDGE_FALLING, &button2);
    wiringPiISR(switch3, INT_EDGE_FALLING, &button3);
    wiringPiISR(switch4, INT_EDGE_FALLING, &button4);
    wiringPiISR(switch5, INT_EDGE_FALLING, &button5);
    wiringPiISR(switch_fade, INT_EDGE_FALLING, &button6);
    wiringPiISR(23, INT_EDGE_FALLING, &rotary_master);
    wiringPiISR(5, INT_EDGE_FALLING, &rotary_edit);
    wiringPiISR(6, INT_EDGE_FALLING, &edit);
    wiringPiISR(1, INT_EDGE_FALLING, &edit_mode);
    
}*/


int main(){
    gettimeofday(&last_interrupt, NULL);
    gpio_init();
    //interrupt_init();
    digitalWrite(led_fade1, HIGH);
    open_dev();
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
                write_buffer();
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
                current.master=analogRead(80);
                dmx=current.to_dmx();
                write_buffer();
                current.print_g();
                msleep(time_per_step-15);
            }
        }
    current.master=analogRead(80);
    dmx=current.to_dmx();
    write_buffer();
    current.print_g();
    if(digitalRead(switch1)==HIGH)
    button1();
    if(digitalRead(switch2)==HIGH)
    button2();
    if(digitalRead(switch3)==HIGH)
    button3();
    if(digitalRead(switch4)==HIGH)
    button4();
    if(digitalRead(switch5)==HIGH)
    button5();
    if(digitalRead(switch6)==HIGH)
    button6();
    if(digitalRead(switch_fade)==LOW)
    button_fade();
    }
    
    return 0;
}
