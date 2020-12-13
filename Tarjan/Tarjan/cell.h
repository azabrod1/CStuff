//
//  node.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/25/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef cell_h
#define cell_h

#include "typedefs.h"
#include <vector>
#include <mutex>
#include <iostream>
#include <algorithm>
#include "search.hpp"


template <class V>
struct Cell{
    
private:
    
    //We save the neighbor queue as a pointer so we can delete it
    //after we
    std::vector<Cell<V>*>* neighborQueue; //Neighbors to visit
    std::mutex  blockedOnMutex;
    std::vector<Search*>* blockedOn;     //What searches are blocked on this cell
    //std::atomic<BlockedList<Search*>*> blockedSearches;
    
public:
    V           vertex;
    Vid         index;
    Vid         rank;
    Status      status;
    bool        transferred;
    bool        root = false;

    
    Cell(){
        ++s2;
        transferred      = false;
        neighborQueue    = nullptr;
        status           = CellStatus::NEW_CELL;
        blockedOn        = new std::vector<Search*>;
        //blockedSearches  = nullptr;
    }
    
    Cell(V _vertex) : Cell(){
        this->vertex = _vertex;
    }
    
    ~Cell(){
        //delete  blockedSearches.load();
    }
    /*Non synchronized methods ;for use when only a guess of the cell status is desired
     Many of these methods have synchronized analogs if it is neccesary
    ***********************************************************************************/
    
    inline bool isComplete(){
        return status == CellStatus::COMPLETE_CELL;
    }
    
    inline bool isNew(){
        return status == CellStatus::NEW_CELL;
    }
    
    inline bool isUnclaimed(){
        return status == CellStatus::NEW_CELL || status == CellStatus::COMPLETE_CELL;
    }
    
    inline bool onStackOf(Search* const search){
        
        return search == status;
    }
    /* Pre: Must be the owner of the cell and all its neighbors must have been explored
     * This function effectivly retires the cell
     */
    inline void markComplete(){
        status = CellStatus::COMPLETE_CELL;
        
    }
    
    inline std::vector<Search*>* removeBlockedList(){
        std::lock_guard<std::mutex> lock(blockedOnMutex);
        std::vector<Search*>* toReturn(blockedOn);
        return toReturn;
    }
    
    
    inline BlockedList<Search*>* removeBlockedListLF(){
//        BlockedList<Search*>* toReturn(blockedSearches);
//        return toReturn;
        return 0;

        
    }
    
    
    inline void initIndex(const Vid idx){
        this->index = idx; this->rank = idx;
    }
    
    inline void promote(Vid rankUpdate){
        this->rank = std::min(this->rank, rankUpdate);
    }
    
    /*Synchronized methods
     ***********************************************************************************/
    
    /** Transfer: The function changes the stack label of the cell to another search, officially transfering
     * ownership of the cell. Delta gives the cell an updated index to match the indicies of the new stack.
     * We add any searches that were blocked on the cell to the passed in by reference vector
     *blockedSearches
     */
    
    
    
    inline void transfer(const Vid delta, Search* const newOwner){
        transferred = true;
        rank +=delta; index += delta;
        status = newOwner;
    }
    
    //Thread safe version of the owner() method
    inline Search* getOwner(){
        
        Search* owner(status.load());
        
        if(owner == CellStatus::NEW_CELL || owner == CellStatus::COMPLETE_CELL) return nullptr;
        
        return owner;
    }
    
    
    inline bool addToBlockedList(Search* const search){
        
        std::lock_guard<std::mutex> lock(blockedOnMutex);
        if(isComplete()) return false;
        
        blockedOn->push_back(search);
        
        return true;
    }
    
    inline bool addToBlockedListLF(Search* const search){
        
//        if(isComplete()) return false;
//        
//        if(!blockedSearches.load()){
//            BlockedList<Search*>* BL(new BlockedList<Search*>); BlockedList<Search*>* null = nullptr;
//            if(!blockedSearches.compare_exchange_strong(null, BL))
//                delete BL;
//            
//        }
//        
//        blockedSearches.load()->push_back(search);
        
        return true;
    }
    
    
    inline void blockSearch(Search* const search){
        std::lock_guard<std::mutex> lock(blockedOnMutex);
        blockedOn->push_back(search);
    }
    
    inline void blockSearchLF(Search* const search){
        
//        if(!blockedSearches.load()){
//            
//            BlockedList<Search*>* BL(new BlockedList<Search*>); BlockedList<Search*>* null = nullptr;
//            if(!blockedSearches.compare_exchange_strong(null, BL))
//                delete BL;
//            
//        }
//        
//        
//        blockedSearches.load()->push_back(search);
    }

    
    //Returns true if and only if the search successfully took ownership of the cell
    //If conquer succeeds, the cell should be added to the conquerer's stack
    char conquer(Search* const conquerer){
        Search* newCell(CellStatus::NEW_CELL);
        
        if(status == CellStatus::NEW_CELL &&
           status.compare_exchange_strong(newCell, conquerer))
            return CellStatus::CONQUERED;
        
        if(status == CellStatus::COMPLETE_CELL) 
            return CellStatus::COMPLETE;
        
        return CellStatus::OCCUPIED;
    }
    
    
    /**Attemts to conquer the cell on behalf of the search conquerer so that it can be added
     *to conquerer's stack. It is similar to the function above, but it either succeeds in
     *conquering the cell and returns true or fails and returns false and does not
     *put the search on the cell;s blockedOn list.
     * The main difference between this function and the normal conquer is that if the search fails
     *to conquer the cell, nothing else happens so it is as if the search never tried.
     */
    bool conquerOrFail(Search* const conquerer){
        Search* newCell(CellStatus::NEW_CELL);

        if(status == CellStatus::NEW_CELL &&
           status.compare_exchange_strong(newCell, conquerer))
            return true;
        
        
        return false;
    }
    
    /*** Methods Not Requiring Sychronization Due To Cell Ownship Precondition******/
    
    /* All methods in this section assume the cell is owned by the search calling the method*/
    
   
    //The purpose of the method is to label a cell which is reserved for a specific owner
    //and hence faces no race conditions from competing threads 
    inline void setOwner(Search* const newOwner){
        status.store(newOwner);
    }
    
    
    inline void setNeighbors(std::vector<Cell<V>*>* neighbors){
        if(!neighbors->size()) //Cell has no neighbors
            delete neighbors;
        else
            this->neighborQueue = neighbors;
        
    }
    
    inline bool allNeighborsDone(){
        return !neighborQueue; //the queue is set to null when it runs out of neighbors
    }
    
    /*Returns a neighbor of this node to be explored. 
     The function tries to choose a node that is not currently 
     being explored by another search if one exists.
     The function assumes the neighbors queue is nonempty*/
    inline Cell<Vid>* getBestNeighbor(){

        Cell<Vid>* candidate = neighborQueue->back();
        
        //If the next element in the queue is not owned by another search,
        //return it
        if(candidate->isUnclaimed()|| candidate->onStackOf(status))
            neighborQueue->pop_back();
        
        //otherwise try to find a cell in the queue that is not occupied by another
        //search to avoid conflict. If non found, we hace no choice but to return an occupied cell
        //Note that this is not sychronized, so the cell might become occupied by the time
        //we return. This is ok since we make no guarantee that we return an unoccupied cell,
        //we just try to do so as an optimization
        
        else{
            
            auto it = neighborQueue->begin();
            
            //return first unoccupied cell
            while(it != neighborQueue->end()){
                candidate = *it;
                
                //is this neighbor occupped? If the neighbor is unclaimed or owned
                //by the current search, we return it for inspection
                if(candidate->isUnclaimed() || candidate->onStackOf(status))
                    break;
                ++it;
            }
            
            if(it == neighborQueue->end())
                neighborQueue->pop_back();
            
            else
                neighborQueue->erase(it);
        }
        
        //delete the old array to save memory once elements in the queue have been used up
        //This is important because a graph may have > 100,000 verticies so having a neighbor array for
        //all of them will result in a massive memory footprint
        
        if(neighborQueue->size() == 0){
            delete neighborQueue;
            neighborQueue = nullptr;
        }
        
        return candidate;
    }
    

};


#endif /* node_h */
