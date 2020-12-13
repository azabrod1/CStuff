//
//  Worker.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/10/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "worker.hpp"
#include "multiThreadedTarjan.hpp"


Worker::Worker(unsigned int _ID, MultiThreadedTarjan& _algo, const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict) : ID(_ID), MASK(1<<_ID), scheduler(_algo), graph(_graph), dict(_dict), spareCell(new Cell<Vid>), spareSearch(new Search) {
    
}

void Worker::operator()(){
    
    Search* search;
    
    while(true){
        
        search = scheduler.getSearch(this);
        if(!search){
            cleanUp();
            return;
        }
        
        explore(search);
        
    }
}

/**
 Pre: The cell must be owned by the thread
 Post: The cell's queue of neighboring cells is created. Each vertex that is a neighbor
 of the vertex represented by parameter cell will have a unique corresponding
 cell object after this call exists
 
 @param cell Pointer to the cell whose neighbors we are identifying
 */
void Worker::initNeighbors(Cell<Vid>* cell){
    
    //get set of all the vertex's neighbors
    auto neighbors = graph.getNeighborsVector(cell->vertex);
    
    std::vector<Cell<Vid>*>* succs = new std::vector<Cell<Vid>*>();
    
    //create cell objects for the successors of vertex that have not yet been seen
    for(auto& succ: neighbors){

        spareCell->vertex = succ; //Set vertex
        auto status = dict.put(succ, spareCell);
        
        //We used up the cell object to store the neighbor so we need a new one
        //for next time
        if(status.second)
            spareCell = new Cell<Vid>();
        
        Cell<Vid>* neighbor = status.first;
        
        if(!neighbor->isComplete()) //If the neighbor is complete, no search has to visit it again
            succs->push_back(neighbor);
        
    }
    
    cell->setNeighbors(succs);
}


void Worker::explore(Search* const search){
    
    Cell<Vid>* curr, *child;
    char attempt;
    
    while(!search->controlStackEmpty()){
        
        curr = search->controlStackTop();
        
        if(!curr->allNeighborsDone()){
            
            child = curr->getBestNeighbor();
            
            if(child->onStackOf(search)) //if on stack, update low link
                curr->promote(child->index);
            
            //Below conditional not synchronized, so need to handle the possible
            //case that the cell becomes complete after the conditional but before conquer
            
            else if(!child->isComplete()){
                
                attempt = child->conquer(search); //maybe eventually make it same as "add to stack method"??
                
                if(attempt == CellStatus::CONQUERED){
                    search->pushToStacks(child); //Put child on Tarjan and Control stack and assign cell an index
                    initNeighbors(child);
                    
                }
                else if(attempt == CellStatus::OCCUPIED){
                    bool res = scheduler.suspend(this, search, child);
                    
                    if(res)
                        return;
                    
                }
                
                //Do not need to do anything if cell is complete
            }
            
        }
        else{ //Cell has no more children left, time to backtrack
            
            search->controlStackPop();
            
            if(!search->controlStackEmpty())
                search->controlStackTop()->promote(curr->rank);
            
            if(curr->index == curr->rank){
                if(curr == search->tarjanStackTop())
                    buildSingletonSCC(search, curr);
                else
                    buildSCC2(search, curr);
            }
            
        }
    }
    //We can only get here if the search's control stack becomes empty. Thus,
    //we can reclaim the memory from the finished search at this point
    
    reclaim(search);
    
}

void Worker::buildSCC(Search* const search, Cell<Vid>* head){
    std::unordered_set<Vid>* scc(new std::unordered_set<Vid>());
    Cell<Vid>* curr;
    
    do{
        curr = search->tarjanStackTop();
        search->tarjanStackPop();
        curr->markComplete(); //need to unpause searches blocked on the cell AND delete cell.blockedON
        sc.begin();
        scheduler.resumeAllBlockedOn(curr);
        sc.accumulate();
        scc->insert(curr->vertex);
        
    }while(curr != head);
    
    SCCs.insert(scc);
}

void Worker::buildSCC2(Search* const search, Cell<Vid>* const head){
    //unordered set to store the SCC in
    std::unordered_set<Vid>* scc(new std::unordered_set<Vid>());
    //vector of searches to resume after we mark the cells as complete

    TarjanStack* tarjanStack(search->getTS());
    
    tarjanStack->back()->markComplete();
    auto rit(tarjanStack->rbegin());
    while(*rit != head)
        (*(++rit))->markComplete();
    
    for(auto it = (rit+1).base(); it != tarjanStack->end(); ++it){
        scheduler.resumeAllBlockedOn(*it);
        scc->insert((*it)->vertex);
    }
    
    tarjanStack->erase((rit+1).base(), tarjanStack->end());
    
    SCCs.insert(scc);
}

//TODO: use this thing
void Worker::buildSingletonSCC(Search* const search, Cell<Vid>* const cell){
    search->tarjanStackPop();
    cell->markComplete();
    scheduler.resumeAllBlockedOn(cell);
    SCCs.insert(new std::unordered_set<Vid>{cell->vertex});
    
}

void Worker::cleanUp(){
    delete spareCell; delete spareSearch;
    
    for(Search* search: recycled)
        delete search;
}

void Worker::allocateSpareSearch(){
    if(recycled.empty())
        spareSearch = new Search;
    else{
        spareSearch = recycled.back();
        recycled.pop_back();
    }
    
    
}






