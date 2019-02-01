#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct booth{
    int booth_no;
    pthread_mutex_t booth_lock;
    pthread_cond_t booth_cond, vot_done;
};

int voters[101], evm[101], done[101];

void polling_ready_evm(struct booth* booth, int count, int ev_no){
    printf("EVM %d at Booth %d is free with slots = %d\n", ev_no, booth->booth_no, count);
    pthread_mutex_lock(&booth->booth_lock);
    while (count-- && voters[booth->booth_no]) {
        evm[booth->booth_no] = ev_no;
        sleep(0.1);
        pthread_cond_signal(&booth->booth_cond);
        pthread_cond_wait(&booth->vot_done, &booth->booth_lock);
        sleep(0.3);
    }
    if(!voters[booth->booth_no] && !done[booth->booth_no]) {
        done[booth->booth_no] = 1;
        pthread_cond_broadcast(&booth->vot_done);
        printf("\033[31mVoters at Booth %d are done with voting\n\033[0m", booth->booth_no);
    }
    pthread_mutex_unlock(&booth->booth_lock);
    return;
}

int voter_wait_for_evm(struct booth* booth){
    int nvm;
    pthread_mutex_lock(&booth->booth_lock);
    pthread_cond_wait(&booth->booth_cond, &booth->booth_lock);
    nvm = evm[booth->booth_no];
    pthread_mutex_unlock(&booth->booth_lock);
    voters[booth->booth_no]--;
    return nvm;
}

void voter_in_slot(struct booth* booth, int voter_no, int nvm){

    printf("\033[32mVoter %d at Booth %d got allocated EVM %d\n\033[0m", voter_no, booth->booth_no, nvm);
    sleep(0.3);
    pthread_mutex_lock(&booth->booth_lock);
    pthread_cond_signal(&booth->vot_done);
    pthread_mutex_unlock(&booth->booth_lock);
    return;
}

struct booth booth_init(int booth_no){
    struct booth booth;
    booth.booth_no = booth_no;
    pthread_mutex_init(&booth.booth_lock,NULL);
    pthread_cond_init(&booth.booth_cond,NULL);
    pthread_cond_init(&booth.vot_done,NULL);
    return booth;
}
