
#include <stdlib.h>

// int NoBlockAfterLoop() {
//     int a = 10;
//     int b = 20;
//     int c = 30;
//     for (unsigned i = 0; i < 100; i++) {
//         c = a * b;
//     }
//     return a; // IMPORTANT: Returning a, not c
// }

// int BlockAfterLoop() {
//     int a = 10;
//     int b = 20;
//     int c = 30;
//     for (unsigned i = 0; i < 100; i++) {
//         c = a * b;
//     }
//     return c; // IMPORTANT: Returning c
// }

// void InstructionDoesntCoverExitBlock() {
//     int rand = random();
//     int a = 10;
//     int b = 20;
//     int c = 30;
//     for (unsigned i = rand; i < 100; i++) {
//         c = a * b;
//     }
// }

void EsempioSlide20PDF12() {
    int a = 10;
    int b = 2;
    int c = rand();
    int e = 0;
    
    while(rand()) {
        if (a == b + c) {
            e = 2;
        } else {
            e = 3;
        }
        int d = a + 1;
        int f = e + 2;
    }
}