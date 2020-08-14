#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<wiringPi.h>
#include <time.h>
#include <errno.h>    

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

//Global definitions
#define scene_size 24  //Number of channels in each scene
#define step 100
//char path[] ="scenes/";

//Setup the GPIO pins as inputs or outputs and the buttons as pull-up
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

//The interrupt iunctions that fire when buttons are pressed
void button1(void) {
    printf("Button 1 Pressed\n");
}

void button2(void) {
    printf("Button 2 Pressed\n");
}

void button3(void) {
    printf("Button 3 Pressed\n");
}

void button4(void) {
    printf("Button 4 Pressed\n");
}

void button5(void) {
    printf("Button 5 Pressed\n");
}

//Installing GPIO interrupts for the pins
void interrupt_init(void){
    wiringPiISR(8, INT_EDGE_FALLING, &button1);
    wiringPiISR(3, INT_EDGE_FALLING, &button2);
    wiringPiISR(7, INT_EDGE_FALLING, &button3);
    wiringPiISR(0, INT_EDGE_FALLING, &button4);
    wiringPiISR(2, INT_EDGE_FALLING, &button5);
}



//Function to print a integer-array
//Todo: color based on value?
void print_array(int *array, int size){
    printf("[ %i", array[0]);
    for(int i=1; i<size;i++){
        printf(" ,%i", array[i]);
    }
    printf(" ]\n");
}

void print_array_g(int *array, int size){
    system("clear");
    for(int iSize=0;iSize<size;iSize++){
        printf("CH%d:", iSize);
        for(int iValue=0;iValue<array[iSize];iValue+=2) printf("=");
        printf("\n");
    }
}

//Read scene file and output array

int set_scene_file(char *path, int *scene){
    FILE *fptr;

    if ((fptr = fopen(path,"r")) == NULL){
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    for(int i=0;i<scene_size;i++){
        fscanf(fptr, "%d%[^\n]", &scene[i] );
    }
    return 0;
}

//changes [*scene] in [time] to [*fadeto]
void scene_fade(int *scene, int *fadeto, int size, int time){
    //doNext
}

int main() {
    gpio_init();
    interrupt_init();
    int sceneA[scene_size] = {0};
    int sceneB[scene_size] = {0};
    int sceneC[scene_size] = {0};
    int sceneD[scene_size] = {0};
    int sceneE[scene_size] = {0};
    int sceneF[scene_size] = {0};
    int sceneNow[scene_size] = {0};
    set_scene_file("scenes/sceneA.txt", sceneA);
    print_array(sceneA,scene_size);
    msleep(2000);
    print_array_g(sceneA,scene_size);
    scene_fade(sceneNow, sceneA, scene_size, 2000);
	printf("Listening...\n");
	getchar();
    printf("Stopping...\n");
}