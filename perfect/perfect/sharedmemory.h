//
//  sharedmemory.h
//  SharedMemory
//
//  Created by Gregory Zabrodskiy on 12/2/17.
//  Copyright © 2017 Gregory Zabrodskiy. All rights reserved.
//

#ifndef sharedmemory_h
#define sharedmemory_h

#define SM_KEY 38977

#define SM_MAX_PROCS 20
#define SM_MAX_INT 33554432 //(2^25)
#define SM_MAX_PEFECT 20


typedef struct  {
    int pid;
    int numFound;
    int numTested;
    int numSkipped;
    
    
} sm_procs;



typedef struct {
    char flag;
    int bb[27];
    int perfect[20];
    int numPerfect; 
    sm_procs pp[SM_MAX_PROCS];
    
    
} sm_shared;

//flag macros
#define SM_FLAG_CLEAR      0
#define SM_FLAG_BUSY       'u'
#define SM_FLAG_CHANGED    '*'

//bit operations
#define SM_SET_BIT(s,k)   (s->bb[(k/32)] |= (1 << (k%32)))
#define SM_CLR_BIT(s,k)   (s->bb[(k/32)] &= ~(1 << (k%32)))
#define SM_IS_BIT(s,k)    (s->bb[(k/32)] & (1 << (k%32)))

#endif /* sharedmemory_h */
