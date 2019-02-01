#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "q2_declarations.c"

int evm[101], voters[101], evm_no[101], vot_no[101];
struct booth booths[101];

void *evmStart(void* val){
    int* v = (int*)val;
    ++evm_no[booths[*v].booth_no];
    int ev_n = evm_no[booths[*v].booth_no];
    sleep(0.2);
    while(voters[booths[*v].booth_no]){
        int free_slots = rand();
        free_slots = free_slots%10;
        ++free_slots;
        polling_ready_evm(&booths[*v], free_slots, ev_n);
        sleep(0.5);
    }
    pthread_exit(NULL);
    return NULL;
}

void *voterStart(void* val){
    int* v = (int*)val;
    ++vot_no[booths[*v].booth_no];
    int vt_n = vot_no[booths[*v].booth_no];
    int nvm = voter_wait_for_evm(&booths[*v]);
    voter_in_slot(&booths[*v], vt_n, nvm);
    pthread_exit(NULL);
    return NULL;
}

int main() {
    int n, i, j, evms;

    scanf("%d", &n);

    pthread_t vots_th[n][100], evm_th[n][100];
    int nu[n];

    for (i=0 ; i<n ; i++){
        scanf("%d %d", &voters[i+1], &evms);
        booths[i] = booth_init(i+1);
        nu[i] = i;
        for(j=0 ; j<voters[i+1] ; j++) pthread_create(&vots_th[i][j], NULL, voterStart,&nu[i]);
        for(j=0 ; j<evms ; j++) pthread_create(&evm_th[i][j], NULL, evmStart,&nu[i]);
    }

    pthread_exit(NULL);

    return 0;
}
