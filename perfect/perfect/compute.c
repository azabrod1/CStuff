//
//  compute.c
//  SharedMemory
//
//  Created by Gregory Zabrodskiy on 12/2/17.
//  Copyright © 2017 Gregory Zabrodskiy. All rights reserved.
//


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include <unistd.h>

#include "sharedmemory.h"



int isPerfect(int i);
sm_procs* reg(sm_shared *shared);
int testPerfect(sm_shared *shared, sm_procs *proc, int ni);


int main(int argc, const char* argv[]){
    
    sm_shared *shared = NULL;
    sm_procs* proc;
    int ni;
    
    // immediate mode (test only)
    if (argc == 3 && strcmp(argv[2], "-i")== 0){
        
        if (isPerfect(atoi(argv[1])))
            printf("%s is perfect!\n", argv[1]);
        return 0;
    }
    // regular mode
    if (argc == 2){
        ni = atoi(argv[1]);
        if (ni == 0){
            printf("Invalid starting number %s\n", argv[1]);
            return 1;
        }
        proc = reg(shared);
        
        if (proc == NULL)
            return 1;
        
        while (ni > 0){
        
            testPerfect(shared, proc, ni);
            ni++;
        }
        
    }
    
    
    
    return 0;
    
}


int testPerfect(sm_shared *shared, sm_procs *proc, int ni){
    
    printf("testing %d\n", ni);
    
    int a = (shared->bb[(ni/32)] |= (1 << (ni%32)));
    
    printf("testing %d (%d)\n", ni, a);
    
    while(SM_IS_BIT(shared, ni) == 0 && ni <= SM_MAX_INT){
        printf("skipping %d\n", ni);
        ni++;
        proc->numSkipped++;
    }
    
    printf("testing %d\n", ni);

    
    SM_SET_BIT(shared, ni);
    
    proc->numTested++;
    
    if (isPerfect(ni)){
        proc->numFound++;
        shared->perfect[shared->numPerfect++] = ni; //__todo__
        printf("found %d\n", ni);
    }
    
    if (shared->numPerfect == SM_MAX_PEFECT){
        printf("max number of perfects found\n");
        return 0;
    }
    
    return (ni + 1) % SM_MAX_INT;
    
}


sm_procs* reg(sm_shared *shared){
    
    pid_t pid = getpid();
    int smid;
    char *sm;
    
    printf("accessing key = %d\n", SM_KEY);
    
    if ((smid = shmget(SM_KEY, sizeof(sm_shared), 0666)) < 0) {
        perror("cannot access shared memory (1)");
        return NULL;
    }
    
    if ((sm = shmat(smid, NULL, 0)) == (char *) -1) {
        perror("cannot access shared memory (2)");
        return NULL;
    }
    
    shared = (sm_shared*) sm;
    
    
    /* wait for the manager update */
    while (shared->flag == 'u')
        sleep(1);
    

    for (int i = 0; i < SM_MAX_PROCS; i++){
        if (shared->pp[i].pid == pid){
            printf("found my pid %d\n", pid);
            shared->flag = SM_FLAG_CHANGED;
            sleep(5); // for testing only
            return &(shared->pp[i]);
        
        }
    }
    for (int i = 0; i < SM_MAX_PROCS; i++){
    
        if (shared->pp[i].pid == 0){
            shared->pp[i].pid = pid;
            printf("registered my pid %d\n", pid);
            shared->flag = SM_FLAG_CHANGED;
            sleep(5); // for testing only
            return &(shared->pp[i]);
        }
        
    }

    printf("too many processes\n");
    return NULL;
}




int isPerfect(int i){
    
    int total = 1;
    int divisor = 2;
    
    while (divisor <= sqrt(i)){
        
        if (i % divisor == 0){
            total += divisor;
            if (divisor * divisor != i)
                total += (i/divisor);
            
            
        }
        divisor++;
    }
    return (total == i);
    
}
