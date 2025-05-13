#include <stdlib.h>

void first_NO_second_NO(){
    int a = rand();
    int b = rand();

    while(rand()){
            a = b+1;
    }
    while(rand()){
        a = b+1;
    }
    b = a+1;
}

void first_YES_second_NO(){
    int a = rand();
    int b = rand();

    if(rand()){
        while(rand()){
            a = b+1;
        }
    }
    while(rand()){
        a = b+1;
    }
    b = a+1;
}

void first_NO_second_YES(){
    int a = rand();
    int b = rand();

    while(rand()){
        a = b+1;
    }
    if(rand()){
        while(rand()){
            a = b+1;
        }
    }
    b = a+1;
}

void first_YES_second_YES(){
    int a = rand();
    int b = rand();

    if(rand()){
        while(rand()){
            a = b+1;
        }
    }
    if(rand()){
        while(rand()){
            a = b+1;
        }
    }
    b=a+1;
}
