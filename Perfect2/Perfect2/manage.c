//
//  main.c
//  SharedMemory
//
//  Created by Gregory Zabrodskiy on 12/2/17.
//  Copyright © 2017 Gregory Zabrodskiy. All rights reserved.
//

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <memory.h>
#include <signal.h>

#include "sharedmemory.h"

//Is there space left in the process table?
int getIndex(sm_shared* const memory){
    
    int i;
    
    for(i = 0; i < SM_MAX_PROCS; ++i)
        if(memory->pp[i].pid)
            return i;
    
    return -1;
    
}


int main(int argc, const char * argv[]) {
    
    sm_shared *shared;
    
    int smid;
    char *sm;
    int pid;
    
    /* create and attach shared memory segment */
    
    if ((smid = shmget(SM_KEY, sizeof(shared), IPC_CREAT | 0666)) < 0) {
        perror("unable to access shared memory (1)");
        return 1;
    }
    if ((sm = shmat(smid, NULL, 0)) == (char *) -1) {
        perror("unable to access shared memory (2)");
        return 1;
    }
    
    printf("shared memory id = %d key = %d\n", smid, SM_KEY);
    
    shared = (sm_shared*) sm;
    
    /* init to all zeros */
    
    memset(shared, 0, sizeof(sm_shared));
    
    while (1){
        /* wait for any updates */
        while (shared->flag != SM_FLAG_CHANGED) sleep(1);
        
        /* set in update mode */
        shared->flag = SM_FLAG_BUSY;
        
        for (int i = 0; i < SM_MAX_PROCS; i++){
            
            if ((pid = shared->pp[i].pid) != 0){
                
                if (kill(pid, 0) != 0){
                    printf("process %d no longer exists\n", pid);
                    shared->pp[i].pid = 0;
                }
                else {
                    printf("process: %d\n" , shared->pp[i].pid);
                }
            }
            
        }
        
        shared->flag = SM_FLAG_CLEAR;
    }
    
    return 0;
}

