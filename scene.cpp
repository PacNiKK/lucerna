#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include "scene.h"
using namespace std;

void scene::sround(){
    for( int i=0; i<size; ++i){
        channel[i]=round(channel[i]);
    }
}
dmx_buffer scene::to_dmx(){
    dmx_buffer result;
    result.size=this -> size+1;
    for( int i=0; i<this->size; ++i){
        result.channel[i+1]=(unsigned char)round(this -> channel[i]*((double)this -> master/255));
    }
    return result;
}

scene scene::operator+(scene sAdd2){
        scene result;
        result.size=this -> size;
        for(int i=0; i<result.size; ++i){
            result.channel[i]=this -> channel[i] + sAdd2.channel[i];
        }
        return result;
    }
scene scene::operator-(scene sSub2){
    scene result;
    result.size=this -> size;
    for(int i=0; i<result.size; ++i){
        //if(sSub1.channel[i]-sSub2.channel[i]>=0){
            result.channel[i]=this -> channel[i]-sSub2.channel[i];
        //}else{
        //    result.channel[i]=0;
        //}
    }
    return result;
}
scene scene::operator*(int multi){
    scene result;
    result.size=this -> size;
    for(int i=0; i<result.size; ++i){
        //if(multi*this -> channel[i]<255){
            result.channel[i]=multi*this -> channel[i];
        //}else{
        //    result.channel[i]=255;
        //}
    }
    return result;
}
scene scene::operator/(int divide){
    scene result;
    result.size=this -> size;
    for(int i=0; i<result.size; ++i){
        result.channel[i]=this -> channel[i]/divide;
    }
    return result;
}

void scene::print(){
    printf("[ %d", (int)this -> channel[0]);
    for(int i=1; i < this -> size;++i){
        printf("; %d", (int)this -> channel[i]);
    }
    printf(" Master is at level: %d]\n", this -> master);
}

void scene::print_g(int HL){
    system("clear");
    for( int iCH=1 ; iCH <= this -> size ; ++iCH){
        
        if(iCH==HL+1){
            printf(">CH%d", iCH);
        }else{
            printf(" CH%d", iCH);
        }
        if(iCH<10) printf(" ");
        printf(":");
        for( int iValue = 0 ; iValue < (int)round(this -> channel[iCH-1]*((double)this -> master/255)); iValue+=2 ) printf("=");
        printf("\n");
    }
    printf("Master: %d\n", this -> master);
}

void scene::print_g(){
    system("clear");
    for( int iCH=1 ; iCH <= this -> size ; ++iCH){
        if(iCH<10) printf(" ");
        printf("CH%d:", iCH);
        for( int iValue = 0 ; iValue < (int)round(this -> channel[iCH-1]*((double)this -> master/255)); iValue+=2 ) printf("=");
        printf("\n");
    }
    printf("Master: %d\n", this -> master);
}

void scene::load(std::string path, int size, bool writable){
    fstream inputFile(path);
    int icurrent=0;
    if (inputFile.good()){
        for (int i=0;i<size;++i){
            inputFile >> icurrent;
            this -> channel[i]=icurrent;
        }
    }
    inputFile.close();
    this -> size = size;
    this -> writable = writable;
    cout << "Load scene in "+ path +"...\n";
}

void scene::save(std::string path){
    if(this -> writable){
        ofstream outputFile(path);
        if (outputFile.good()){
            cout << std::to_string(this->size);
            cout << endl;
            for (int i=0;i<this->size;++i){
                cout << std::to_string(this->channel[i]);
                cout << endl;
                outputFile << this -> channel[i];
                outputFile << endl;
            }
        }
        outputFile.close();
        cout << "Saved scene to File "+ path + "...";
    }
}

scene::scene(){
    this -> size=0;
    this -> writable=false;
    this -> master=255;
}
scene::scene(int size){
    this -> size=size;
    this -> writable=false;
    this -> master=255;
}

scene::scene(std::string path, int size, bool writable){
    fstream inputFile(path);
    int icurrent=0;
    if (inputFile.good()){
        for (int i=0;i<size;++i){
            inputFile >> icurrent;
            this -> channel[i]=icurrent;
        }
    }
    inputFile.close();
    this -> size = size;
    this -> writable = writable;
    this -> master=255;
    cout << "Load scene in "+ path +"...\n";
}

