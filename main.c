#include<stdio.h>
#include<wiringPi.h>

void button(void) {
    printf("Button Pressed\n");
}

int main() {
    wiringPiSetup();
    pinMode(8, INPUT);
    pullUpDnControl(8, PUD_UP);
    wiringPiISR(8, INT_EDGE_FALLING, &button);
	printf("Listening...\n");
	getchar();
}