#include<stdio.h>
#include <unistd.h>
#include<wiringPi.h>

//Global definitions
#define scene_size 24  //Number of channels in each scene

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
void print_array(int *array, int size){
    printf("[ %i", array[0]);
    for(int i=1; i<size;i++){
        printf(" ,%i", array[i]);
    }
    printf(" ]\n");
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
    print_array(sceneA,scene_size);
	printf("Listening...\n");
	getchar();
    printf("Stopping...\n");
}