//
//  main.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/24/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include "typedefs.h"
#include "unordered_map"
#include "search.hpp"
#include "tarjan.hpp"
#include <thread>
#include "simpleClock.h"
#include "utilities.hpp"
#include "SimpleSharded.h"
#include "suspensionManager.hpp"
#include <dirent.h>
#include "AdjacecyListGraph.h"


using namespace std;

void testWithRandomGraphs();
void testMemoryUse(); void testMemoryUse(string);

void testWithCSPGraphs(string filename);
void testSingles();
void deleteSCCs(SCC_Set*);
void benchmark_CSP(string dir, string filename, bool benchSingle = true, const int RUNS = 100, const int THREADS = 8);
void benchmark_CSP_Dir(string dir,  bool benchSingle = true, const int RUNS = 100, const int THREADS = 8);
void benchmarkGraph(Graph<Vid>* graph, string name, bool benchSingle = true, const int RUNS = 100, const int THREADS = 8);
void benchmark_GeoGen(double , Vid , bool = true, const int  = 100, const int = 8);
void benchmark_Clusters(Vid , Vid , bool = true, const int  = 100, const int = 8);

#define DISPLAY_SCC_COUNT false


class M{
public:
    M(int _x) {x = _x;}
    int x;
    int array[5];
    bool operator==(M &other){
        return this == &other;
    }
    M(M&& m){
        x = m.x;
        array[0] = 13;
        m.array[0] = 34;
    }
    M(M& m)  = default;
};


class W{
public:
    M m;
    W(M _m): m(_m){;}
};


mutex mu;
int x; int N = 1000;

unordered_map<int, M*> umap;

void test3(int num){
    volatile long i = 0; volatile long k = 10000000000;
    while(i < k){
        ++i;
    }
    
    cout << i <<endl;
    
}

void sleeps(int secs){
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}
std::vector<std::string> GetDirectoryFiles(const std::string& dir) {
    std::vector<std::string> files;
    std::shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR* dir){ dir && closedir(dir); });
    struct dirent *dirent_ptr;
    if (!directory_ptr) {
        std::cout << "Error opening : " << std::strerror(errno) << dir << std::endl;
        return files;
    }
    
    while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
        files.push_back(std::string(dirent_ptr->d_name));
    }
    return files;
}

int main(int argc, const char * argv[]) {
    /*
     OpenAddressedMap map;
     Cell<Vid> cell;
     for(int i = 0; i < 10000; ++i)
     cout << map.put(i % 1000, WeakReference<Cell<Vid>>(&cell,4)).second << endl;;
     cout << map.put(5, WeakReference<Cell<Vid>>(&cell,4)).second << endl;
     */
    
    //  testWithRandomGraphs();
    
    // testWithCSPGraphs("/Users/alex/Downloads/CSP/Alt/alt10.4.1.graph");
    
    // testWithCSPGraphs("/Users/alex/Downloads/CSP/alt/alt12.2.1.graph");
    
    //testWithCSPGraphs("/Users/alex/Downloads/CSP/bill_timed/should_pass/tabp.7.graph");
    
    /*
     const auto& directory_path = std::string("/Users/alex/Downloads/CSP/alt/.");
     const auto& files = GetDirectoryFiles(directory_path);
     for (const auto& file : files) {
     std::cout << file << std::endl;
     }*/
    
    // benchmark_CSP("/Users/alex/Downloads/CSP/Alt/", "alt10.3.2.graph", false, 5, 8);
    // benchmark_GeoGen(0.00057, 30000, true, 50, 8);
    
    benchmark_CSP("/Users/alex/Downloads/CSP/bill_timed/should_pass/", "tring2.1.graph", true, 30, 8);
    // testMemoryUse("/Users/alex/Downloads/CSP/Alt/alt10.3.0.graph");
    // testMemoryUse();
    
    // benchmark_CSP_Dir("/Users/alex/Downloads/CSP/Alt/", true, 5, 8);
    // benchmark_CSP_Dir("/Users/alex/Downloads/CSP/bill_timed/should_pass/", true, 20, 8);
    
    
    
    return 0;
}

void testSingles(){
    
    AdjacencyListGraph<Vid> graph;
    
    graph.insertEdge(0,4);  graph.insertEdge(4,5); graph.insertEdge(5,6); graph.insertEdge(5, 5);
    graph.insertEdge(6, 4); graph.insertEdge(0, 5);
    graph.insertEdge(10, 15);     graph.insertEdge(15, 10);
    
    
    graph.insertEdge(10, 106);
    graph.insertEdge(101, 102);
    graph.insertEdge(102, 103);
    graph.insertEdge(103, 101);
    graph.insertEdge(104, 103);
    graph.insertEdge(104, 102);
    graph.insertEdge(104, 105);
    graph.insertEdge(105, 105);
    graph.insertEdge(105, 104);
    graph.insertEdge(105, 106);
    graph.insertEdge(106, 103);
    graph.insertEdge(106, 107);
    graph.insertEdge(107, 106);
    graph.insertEdge(108, 108);
    graph.insertEdge(108, 107);
    graph.insertEdge(108, 109);
    graph.insertEdge(109, 110);
    graph.insertEdge(110, 111);
    graph.insertEdge(109, 112);
    graph.insertEdge(112, 112);
    graph.insertEdge(113, 112);
    graph.insertEdge(113, 114);
    graph.insertEdge(114, 113);
    graph.insertEdge(114, 109);
    graph.insertEdge(200, 201);
    graph.insertEdge(200, 202);
    graph.insertEdge(201, 203);
    graph.insertEdge(201, 205);
    graph.insertEdge(202, 206);
    graph.insertEdge(203, 205);
    graph.insertEdge(204, 202);
    graph.insertEdge(205, 201);
    graph.insertEdge(205, 207);
    graph.insertEdge(206, 204);
    graph.insertEdge(207, 205);
    graph.insertEdge(209, 203);
    
    
    
    SCC_Set* SCCs  = Tarjan::singleThreadedTarjan(graph);
    SCC_Set* SCCs2 = Tarjan::multiThreadedTarjan(graph);
    
    
    for(auto& set: *SCCs){
        
        cout<< "(";
        
        for(auto& vert: *set)
            cout << vert << " ";
        
        
        
        cout <<")" << endl;
        
    }
    
    cout << SCCs->size() << " " << SCCs2->size() <<endl;
    
    delete SCCs; delete SCCs2;
    
}
//.915

void testMemoryUse(){
    // Graph<Vid>* graph   = Utility::clusters(10000, 5000, 8, 1000);
    
    Graph<Vid>* graph = Utility::GeoGenerateRandomGraph(0.00057, 1, 50000);
    
    std::cout<< "graph size: " << graph->size() <<endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto sccs = Tarjan::multiThreadedTarjan(*graph,8);
    Utility::deleteSCCs(sccs);
    delete graph;
    
}

void testMemoryUse(string path){
    Graph<Vid>* const graph = Utility::importGraphFromCSP(path);
    std::cout<< "graph size: " << graph->size() <<endl;
    
    std::cout << "sleeep" <<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto sccs = Tarjan::multiThreadedTarjan(*graph,8);
    Utility::deleteSCCs(sccs);
    delete graph;
}



void testWithRandomGraphs(){
    // insert code here...
    
    SimpleClock c;
    
    // Graph<Vid>* graph  = Utility::generateRandomGraph(0.0001,40000);
    
    Graph<Vid>* graph  = Utility::GeoGenerateRandomGraph(0.00057, 1, 30000);
    
    //  Graph<Vid>* graph   = Utility::clusters(10000, 1000, 6, 1000);
    
    
    const int N(500);
    SCC_Set* sets[N];
    
    for(int i = 0; i < N; ++i){
        
        c.begin();
        sets[i] = Tarjan::multiThreadedTarjan(*graph,8);
        c.end();
        //if(!i)
        //    for(auto set: *sets[i]){
        //      cout << set->size() <<endl;
        //  }
        cout << "searches leaked " << s << endl;
        cout << sets[i]->size() <<endl;
        Utility::deleteSCCs(sets[i]);
    }
    //c.printAccum("accumulated: ");  // 3.27669
    
    c.begin();
    auto sccs = Tarjan::singleThreadedTarjan(*graph);
    c.end("Single Threaded Tarjan");
    
    c.begin();
    auto sccs2 = Tarjan::singleThreadedTarjan(*graph);
    c.end("Single Threaded Tarjan");
    
    
    cout << sccs->size() << endl;
    
    cout << sccs2->size() << endl;
    
    //for(int i =0; i < N; ++i)
    //    cout << sets[i]->size() <<endl;
    
    
    //    int set1Card(0); int set2Card(0);
    //    for(auto set: *sccs){
    //        set1Card+= set->size();
    //
    //    }
    //
    //    for(auto set: *sccs2){
    //        set2Card+= set->size();
    //    }
    //
    
    //    cout << set1Card << "   :   " << set2Card <<endl;
    
    
    Utility::deleteSCCs(sccs); Utility::deleteSCCs(sccs2);
    /*
     c.begin();
     auto sccs3 = Tarjan::singleThreadedTarjan(*graph);
     c.end("Single Threaded Tarjan Run 3");
     Utility::deleteSCCs(sccs3);
     
     c.begin();
     auto sccs4 = Tarjan::singleThreadedTarjan(*graph);
     c.end("Single Threaded Tarjan Run 4");
     Utility::deleteSCCs(sccs4);
     
     c.begin();
     auto sccs5 = Tarjan::singleThreadedTarjan(*graph);
     c.end("Single Threaded Tarjan Run 5");
     Utility::deleteSCCs(sccs5);*/
    
    delete  graph;
    
    
}

void testWithCSPGraphs(string filename){
    
    Graph<Vid>* graph = Utility::importGraphFromCSP(filename);
    
    std::cout << "Number of Verticies: " << graph->size() << endl;
    
    SimpleClock profiler;
    
    profiler.begin();
    auto sccs = Tarjan::singleThreadedTarjan(*graph);
    profiler.end();
    
    
    cout << sccs->size() <<endl;
    
    const int N(20);
    SCC_Set* sets[N];
    
    for(int i = 0; i < N; ++i){
        
        profiler.begin();
        sets[i] = Tarjan::multiThreadedTarjan(*graph,8);
        profiler.end();
        //if(!i)
        //    for(auto set: *sets[i]){
        //      cout << set->size() <<endl;
        //  }
        cout << "searches leaked " << s << endl;
        cout << sets[i]->size() <<endl;
        Utility::deleteSCCs(sets[i]);
    }
    
    
    
    profiler.begin();
    auto sccs2 = Tarjan::multiThreadedTarjan(*graph, 8);
    profiler.end("Multi Threaded Tarjan");
    
    profiler.begin();
    auto sccs3 = Tarjan::singleThreadedTarjan(*graph);
    profiler.end("Single Threaded Tarjan");
    
    profiler.begin();
    auto sccs4 = Tarjan::singleThreadedTarjan(*graph);
    profiler.end("Single Threaded Tarjan");
    
    
    cout << sccs->size() << endl;
    
    cout << sccs2->size() << endl;
    
    //for(int i =0; i < N; ++i)
    //    cout << sets[i]->size() <<endl;
    
    
    
    int set1Card(0); int set2Card(0);
    for(auto set: *sccs){
        set1Card+= set->size();
        
    }
    
    for(auto set: *sccs2){
        set2Card+= set->size();
    }
    
    
    cout << set1Card << "   :   " << set2Card <<endl;
    
    delete graph;
    Utility::deleteSCCs(sccs); Utility::deleteSCCs(sccs2);
    Utility::deleteSCCs(sccs3); Utility::deleteSCCs(sccs4);
    
    
}

void benchmarkGraph(Graph<Vid>* graph, string name, bool benchSingle, const int RUNS, const int THREADS){
    
    SimpleClock profiler; string msg;
    
    //Bench Mark Single Threaded
    
    if(benchSingle){
        for(int run = 0; run < 5; ++run){
            auto sccs = Tarjan::singleThreadedTarjan(*graph);
            Utility::deleteSCCs(sccs);
        }
        
        for(int run = 0; run < RUNS; ++run){
            profiler.begin();
            auto sccs = Tarjan::singleThreadedTarjan(*graph);
            profiler.accumulate();
            Utility::deleteSCCs(sccs);
            
        }
        msg = "Single Threaded " + name;
        profiler.printAccum(std::move(msg), RUNS);
        profiler.resetAccum();
        
    }
    for(int run = 0; run < 5; ++run){
        auto sccs = Tarjan::multiThreadedTarjan(*graph, THREADS);
        Utility::deleteSCCs(sccs);
    }
    
    for(int run = 0; run < RUNS; ++run){
        profiler.begin();
        auto sccs = Tarjan::multiThreadedTarjan(*graph, THREADS);
        profiler.accumulate();
        if(DISPLAY_SCC_COUNT) std::cout<< sccs->size() << ",";
        Utility::deleteSCCs(sccs);
    }
    if(DISPLAY_SCC_COUNT) std::cout<< endl;
    
    msg = std::to_string(THREADS) + " Threads " + name;
    profiler.printAccum(std::move(msg), RUNS);
    profiler.resetAccum();
    
    delete graph;
}


void benchmark_CSP(string path, string filename, bool benchSingle, const int RUNS, const int THREADS){
    
    Graph<Vid>* const graph = Utility::importGraphFromCSP(path+filename);
    
    benchmarkGraph(graph, filename, benchSingle, RUNS, THREADS);
    
}

void benchmark_Clusters(Vid clusters, Vid clustSize, bool benchSingle, const int RUNS, const int THREADS){
    
    Graph<Vid>* const graph   = Utility::clusters(clusters, clustSize, 6, 1000);
    
    string graphName = "Clust_" + to_string(clusters) + "X" + to_string(clustSize);
    
    benchmarkGraph(graph, graphName, benchSingle, RUNS, THREADS);
    
}

void benchmark_GeoGen(double edgeProb, Vid cardinality, bool benchSingle, const int RUNS, const int THREADS){
    
    Graph<Vid>* graph = Utility::GeoGenerateRandomGraph(edgeProb, 1, cardinality);
    
    string graphName = "Geo_" + to_string(cardinality);
    
    benchmarkGraph(graph, graphName, benchSingle, RUNS, THREADS);
    
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void benchmark_CSP_Dir(string dir,  bool benchSingle, const int RUNS, const int THREADS){
    
    vector<string> graphs = GetDirectoryFiles(dir);
    auto it = graphs.begin();
    
    while(it != graphs.end()){
        if(hasEnding(*it, ".graph")) ++it;
        else it = graphs.erase(it);
        
    }
    for(string file: graphs)
        benchmark_CSP(dir, file, benchSingle, RUNS, THREADS);
    
}





