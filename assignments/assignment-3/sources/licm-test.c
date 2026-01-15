
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

// void EsempioSlide20PDF12() {
//     int a = 10;
//     int b = 2;
//     int c = rand();
//     int e = 0;
    
//     while (rand()) {
//         if (a == b + c) {
//             e = 2;
//         } else {
//             e = 3;
//         }
//         int d = a + 1;
//         int f = e + 2;
//     }
// }

/* -------------------------------------------------------------------------- */
void WHILE_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() {
    int a = rand();
    int b = rand();
    int x = rand();
    
    a = x + 1;
    x = a + 1;
    while (rand()) {
        a = b + 1;
        a = a + 1;
    }
    x = a + 1;
}

void WHILE_BREAK_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() {
    int a = rand();
    int b = rand();
    int x = rand();
    
    a = x + 1;
    x = a + 1;
    while (rand()) {
        a = b + 1;
        a = a + 1;
        if (rand()) break;
    }
    x = a + 1;
}

void FOR_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() {
    int a = rand();
    int b = rand();
    int x = rand();
    
    a = x + 1;
    x = a + 1;
    for (int i = 0; i<100; i++) {
        a = b + 1;
        a = a + 1;
    }
    x = a + 1;
}
/* -------------------------------------------------------------------------- */
void MORTA_controlla_se_la_variabile_e_morta_fuori_dal_loop() {
    int a = rand();
    int b = rand();
    
    while (rand()) {
        a = b + 1;
    }
    
}
void VIVA_controlla_se_la_variabile_e_viva_fuori_dal_loop() {
    int a = rand();
    int b = rand();
    
    while (rand()) {
        a = b + 1;
    }
    a = a + a;
}
/* -------------------------------------------------------------------------- */
void ciclo_infinito_WHILE() {
    int a = rand();
    int b = rand();

    while (1) {
        a = b + 1;
    }

    a = a + a;
}
void ciclo_infinito_FOR() {
    int a = rand();
    int b = rand();

    for (;;) {
        b = a + 1;
    }
    b = b + b;
}
/* -------------------------------------------------------------------------- */
void variabile_definita_molteplici_volte_CON_COSTANTE() {
    int a = rand();
    int b = rand();

    while(rand()) {

        if (rand()) {
            a = 5;
        } else {
            a = b + 1;
        }

        b = a + 1;
    }

    a = a + 1;
}
void variabile_definita_molteplici_volte_CON_VARIABILE() {
    int a = rand();
    int b = rand();
    int c = rand();

    while(rand()) {

        if (rand()) {
            a = c - 1;
        } else {
            a = b + 1;
        }

        b = a + 1;
    }

    a = a + 1;
}
/* -------------------------------------------------------------------------- */
void WHILE_annidato() {
    int a = rand();
    int b = rand();
    int x = rand();
    
    a = x + 1;
    x = a + 1;
    while (rand()) {
        while (rand()) {
            a = b + 1;
            a = a + 1;
        }
    }
    x = a + 1;
}

void WHILE_annidato_due_volte() {
    int a = rand();
    int aa = rand();
    int b = rand();
    int x = rand();
    int xx = rand();
    
    a = x + 1;
    x = a + 1;
    aa = xx + 1;
    xx = aa + 1;
    while (rand()) {
        while (rand()) {
            a = b + 1;
            a = a + 1;
        }
        while (rand()) {
            aa = b + 1;
            aa = aa + 1;
        }
    }
    x = a + 1;
    xx = aa + 1;
}
/* -------------------------------------------------------------------------- */