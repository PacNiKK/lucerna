#include<stdio.h>
#include<wiringPi.h>

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

int main() {
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
    wiringPiISR(8, INT_EDGE_FALLING, &button1);
    wiringPiISR(3, INT_EDGE_FALLING, &button2);
    wiringPiISR(7, INT_EDGE_FALLING, &button3);
    wiringPiISR(0, INT_EDGE_FALLING, &button4);
    wiringPiISR(2, INT_EDGE_FALLING, &button5);
	printf("Listening...\n");
	getchar();
}