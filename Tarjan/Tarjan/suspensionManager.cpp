//
//  algorithmManager.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/3/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "suspensionManager.hpp"

bool SuspensionManager::suspend(Worker& worker, Search* const Sn, Cell<Vid>* const conflictCell){
    
    //If we fail to add the cell to the blocked list, it must be complete
    if(!conflictCell->addToBlockedList(Sn))
        return RESUME;
    
    
    //Conflict cell seems to be in progress, so mark this search as suspended
    
    Sn->suspendOn(conflictCell);
    
    /*We must check that the cell did not become complete while we suspended the 
     search on it as its unsuspend() function may have been called before 
     the search was "officially" suspended, hence leaving it forever suspended.
     If the cell did become complete, we try to manually unsuspended it.
     If the unsuspend succeeds, we can resume. Otherwise, we must suspend as
     the search may have already been put on pending and we do not want do threads to run it 
     at the same time
     */
    
    //In case resumeAll() already happened, we do its work for it and remove cell blocked on

    
    if(conflictCell->isComplete()){
        if(Sn->suspendedOnCompareAndExchange(conflictCell, nullptr))
            return RESUME;
        else
            return SUSPEND;

    }

    //We are reusing old empty vectors to represent the path of searches and cells
    //We reuse the mmeory to avoid allocating memory too much as memory is often
    //allocated sequentially on standard systems, hence presenting a potential bottleneck
    
    worker.cleanPaths();
    std::vector<Search*>&    S = worker.S;
    std::vector<Cell<Vid>*>& C = worker.C;
    std::vector<Vid>&        L = worker.L;

    
    
    //Find the search S1 that is directly blocking S
    Search* S0(conflictCell->getOwner());
    
    /******* Check for cycle ********/
    
    //Path: first pass
    
    Search* Si(Sn); Cell<Vid>* Ci; Vid Li;
    
    //is there already a blocking path from S1 to Sn?
    
    
    /*S1 --> S2 -->  S3 --> S4 -->...---> Sn-->..
     ^       ^      ^       ^            ^
     |       |      |       |            |
     C1      C2    C3       C4           Cn
     
     */
    
    do{
        Ci = Si->getBlockingCell(); //Here Si is S, Si must exist due to use of smart pointer
        if(!Ci) return SUSPEND;
        
        Si = Ci->getOwner();
        if(!Si) return SUSPEND;
        
        Li = Si->life;
        //Odd numbers signal that the search is currently in a transfer, so we can safely
        //suspend knowing any cycles are being resolved 
        if(Li & 1)
            return SUSPEND;
            
        C.push_back(Ci);
        S.push_back(Si);
        L.push_back(Li);
        
        
    }while(Si != Sn);
    
    //Second pass
    
    Si = Sn;
    
    Search* minPtr(S[0]); //We look for search with the smallest pointer value
    
    int lo(0);               //Index of search with smallest pointer value
    
    int pathSize(S.size());
    
    for(int i = 0; i < pathSize; ++i){
        
        Ci = Si->getBlockingCell();
        
        if(Ci != C[i])
            return SUSPEND;
        
        Si = Ci->getOwner();
        
        if(Si != S[i])
            return SUSPEND;
        
        if(Si->life != L[i])
            return SUSPEND;
        
        if(Ci->isComplete())
            return SUSPEND;
        
        if(Si < minPtr){
            minPtr = Si;
            lo = i;
        }
        
        
    }
    //Was the link leading to the second path valid?
    if(C[0] != conflictCell || C[0]->isComplete() || S[0] != S0)  ///May need to check S[0]
        return SUSPEND;
    
    //If we get here, the loop is confirmed
    
    //Acts as a consensus protocol, make the life of the search with the smallest pointer odd
    
    if(!S[lo]->life.compare_exchange_strong(L[lo], L[lo]+1))
        return SUSPEND;
    
    runCellTransfer(S, C);
    
    //Last two what do we do????
    Sn->removeCellBlockedOn();
    
    /*The life counter on the search with the lowest pointer is incremented so it is even again
     *so other searches are aware it is no longer in a transfer
     */
    
    (S[lo]->life)++;
    
    for(Search* Si: S)
        if(Si->done()){ //Reclaim memory from the finished search
            Si->ID = worker.getID();
            worker.reclaim(Si);
        }
    
    return RESUME;
    
}

/**
 @param S The blocking path from the search directly blocking S, sn, all the way to S
 
 The path vector is represented as this:
 
 s1 < ----- s1 <----s2 <--- ....<-----  sn-1 <----- sn
 
 */

void SuspensionManager::runCellTransfer(const std::vector<Search*>& S, const std::vector<Cell<Vid>*>& C)
{
    Search* const Sn(S.back());
    
    
    //Traverse the path starting with the search that S is directly blocked on
    for(int i = 0; i < S.size() - 1; ++i)
        Search::transferCells(S[i], Sn, C[i]);
    
    //Makes sure S is "aware" that all transferred cells are in the same SCC,
    //make the new top of S's tarjan stack (last of the transfered cells) have rank
    //of the cell of S that S(n-1) was blocked on
    
    Sn->controlStackTop()->promote(C.back()->index);
    
}

//Note: a search can only be moved from blocked on one cell to another during cycle but in that case the cell won't become complete randomly

void SuspensionManager::bulkUnsuspend(std::vector<Search*>* const suspendedList, Cell<Vid>* const prevBlockedOn){
    
    for(Search*& search: *suspendedList){
        //This call may be from a pre-mature resume, make sure we do not accidently wake another later real suspension
        //by making sure that the cell the search is blocked on.
        //This can't fail if search is actually suspended since it would not change suspOn(Cell)
        if(!search->removeCellBlockedOn(prevBlockedOn))
            search = nullptr; //Removes the search from suspended list so we do not later add it to pending
                              //since we found out it is not actually blocked
        
        
    }
    
}








