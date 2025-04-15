
#include <stdlib.h>

int main() {

    int randcond = rand();

    int v = 1; // A

    if (randcond) {

        v += 1; // B

    } else {

        v += 1; // C

        if (randcond) {

            v += 1; // D

        } else {

            
            v += 1; // E

        }
        
        v += 1; // F

    }

    v += 1; // G

    return v;
}
