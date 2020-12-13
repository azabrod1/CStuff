//
//  suspensionManager2.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef suspensionManager_hpp
#define suspensionManager_hpp

#include <stdio.h>
#include <mutex>
#include "stealingQueue.hpp"
#include <vector>
#include "worker.hpp"

#define SUSPEND true

#define RESUME false


class SuspensionManager{
    
    
private:
    
    /*Functions to suspend searches and retrieve which searches are suspended on which
     We abstact away the implementation of these functions from the rest of the class so that
     it is easy to change how we track suspended seaches by just altering these functions */
    
    inline void unSuspend(Search* const search){
        search->removeCellBlockedOn();
    }
    
    void runCellTransfer(const std::vector<Search*>& S, const std::vector<Cell<Vid>*>& C);

    
public:
    
    bool suspend(Worker& worker, Search* const Sn, Cell<Vid>* const conflictCell);
    
    void bulkUnsuspend(std::vector<Search*>* const suspendedList, Cell<Vid>* const prevBlockedOn);
    
    void bulkUnsuspendLF(BlockedList<Search*>* const suspendedList, Cell<Vid>* const prevBlockedOn, std::vector<Search*>& toUnsuspend){
        
        int size(suspendedList->size()); Search* search;
        
                
        if(suspendedList->smallList(size)){
            
            for(int e = 0; e < size; ++e){
                search = suspendedList->masterList[0][e];
                if(search && search->removeCellBlockedOn(prevBlockedOn))
                    toUnsuspend.push_back(search);
                
                
            }
            
        }
        
        else{
            assert(false);  //Not supported yet
        }
        

    }
    
    
};



#endif /* suspensionManager2_hpp */
