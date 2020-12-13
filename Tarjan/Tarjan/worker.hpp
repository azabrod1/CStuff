//
//  worker.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/10/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef worker_hpp
#define worker_hpp

#include <stdio.h>
#include "search.h"
#include "typedefs.h"
#include "graph.h"
#include "dictionary.h"
#include "stealingQueue.hpp"

class MultiThreadedTarjan;

template <class V> class Cell;
//Make sure pass ref
class Worker{
    
    friend class MultiThreadedTarjan;
    
private:
    
    //Variables
    MultiThreadedTarjan& scheduler;
    const Graph<Vid>& graph;
    Dictionary<Vid, Cell<Vid>*>& dict;
    const unsigned int ID;
    const long MASK;
    SCC_Set SCCs;
    SimpleClock sc;
    Cell<Vid>* spareCell;
    Search* spareSearch;
    std::vector<Search*> recycled;
    
    //Methods
    void explore(Search* const  search);
    
    void initNeighbors(Cell<Vid>* cell);
    
    void buildSCC(Search* const, Cell<Vid>*);
    
    void buildSCC2(Search* const, Cell<Vid>* const);
    
    void buildSingletonSCC(Search* const search, Cell<Vid>* const cell);
    
    void cleanUp();
    
    void allocateSpareSearch();
    
    
    
public:
    
    Worker(unsigned int _ID, MultiThreadedTarjan& _algo, const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict);
    void operator()();
    
    inline void cleanPaths(){
        S.clear(); C.clear(); L.clear();
    }
    
    void reclaim(Search* const search){
        search->reset();
        recycled.push_back(search);
    }
    
    inline unsigned getID(){
        return ID;
    }
    
    std::vector<Search*> S;
    std::vector<Cell<Vid>*> C;
    std::vector<Vid> L;
    
    
    
    
    
    
};




#endif /* worker_hpp */
