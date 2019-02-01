#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef long long int ll;

// input
ll n;

// players and refrees in academy
ll pin, rin;

// players warmed up
ll ply_ready;

// players and refrees met with organiser
ll pmet, rmet;

// players and refrees left to come
ll total_left;

// game count
ll num_games;

// mutex lock variables
pthread_mutex_t organiser, pl_r, ply, refc, player1, player2, refree, game_start;

// mutex condition variables
pthread_cond_t ply1, ply2, ref, ready, started;

// Defining thread ids
pthread_t p_id[300005];

void meetOrganiser(char str[], ll no){
    if(!strcmp(str, "ply")){
        pthread_mutex_lock(&organiser);
        printf("player: %lld met organiser\n", no);
        pthread_mutex_lock(&ply);
        ++pmet;
        if(pmet%2){
            pthread_mutex_unlock(&ply);
            pthread_mutex_unlock(&organiser);
            pthread_mutex_lock(&player1);
            pthread_cond_wait(&ply1, &player1);
            pthread_mutex_unlock(&player1);
        }
        else{
            pthread_mutex_unlock(&ply);
            pthread_mutex_unlock(&organiser);
            pthread_mutex_lock(&player2);
            pthread_cond_wait(&ply2, &player2);
            pthread_mutex_unlock(&player2);
        }
    }
    else{
        pthread_mutex_lock(&organiser);
        printf("refree: %lld met organiser\n", no);
        pthread_mutex_lock(&refc);
        ++rmet;
        pthread_mutex_unlock(&refc);
        pthread_mutex_unlock(&organiser);
        pthread_mutex_lock(&refree);
        pthread_cond_wait(&ref, &refree);
        pthread_mutex_unlock(&refree);
    }

    return ;
}

void *playerCreate(void * bl){

    ++pin;
    ll pno = pin;

    printf("player: %lld entered academy\n", pno);

    meetOrganiser("ply", pno);

    printf("player: %lld enters court and warms up\n", pno);

    sleep(1);

    pthread_mutex_lock(&pl_r);
    ++ply_ready;
    if(ply_ready>=2) pthread_cond_signal(&ready);
    pthread_mutex_unlock(&pl_r);

    return NULL;
}

void *refreeCreate(void * bl){

    ++rin;
    ll rno = rin;

    printf("refree: %lld entered academy\n", rno);

    meetOrganiser("ref", rno);

    printf("refree: %lld enters court and adjusts equipment\n", rno);

    sleep(0.5);

    pthread_mutex_lock(&pl_r);
    pthread_cond_wait(&ready, &pl_r);
    ply_ready-=2;
    pthread_mutex_unlock(&pl_r);
    ++num_games;
    printf("starting game: %lld\n",num_games);
    pthread_mutex_lock(&game_start);
    pthread_cond_signal(&started);
    pthread_mutex_unlock(&game_start);

    return NULL;
}

void *newPerson(void * bl){
    while (total_left){
        int r = rand();
        if( ((float)(n-rin))/total_left > (((float)r)/RAND_MAX) ) pthread_create(&p_id[total_left], NULL, refreeCreate, NULL);
        else pthread_create(&p_id[total_left], NULL, playerCreate, NULL);

        --total_left;

        int tim = rand();
        tim = tim%3;

        sleep(tim);
    }
    return NULL;
}

int main()
{
    scanf("%lld", &n);
    total_left = 3*n;
    pthread_t create;
    pthread_create(&create, NULL, newPerson, NULL);
    while(num_games != n){
        if(rmet>=1 && pmet>=2){
            pthread_mutex_lock(&organiser);

            pthread_mutex_lock(&refc);
            --rmet;
            pthread_mutex_unlock(&refc);

            pthread_mutex_lock(&ply);
            pmet-=2;
            pthread_mutex_unlock(&ply);

            pthread_mutex_lock(&player1);
            pthread_cond_signal(&ply1);
            pthread_mutex_unlock(&player1);

            pthread_mutex_lock(&refree);
            pthread_cond_signal(&ref);
            pthread_mutex_unlock(&refree);

            pthread_mutex_lock(&player2);
            pthread_cond_signal(&ply2);
            pthread_mutex_unlock(&player2);

            pthread_mutex_lock(&game_start);
            pthread_cond_wait(&started,&game_start);
            pthread_mutex_unlock(&game_start);
            pthread_mutex_unlock(&organiser);
        }
    }
    return 0;
}
