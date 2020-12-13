//
//  main.cpp
//  C_Random
//
//  Created by Alex Zabrodskiy on 4/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <time.h>
#include <memory>
#include <random>
#include "Aqueue.h"
#include "blockedList.cpp"
#include "testBlockedList.h"

#include "tbb/critical_section.h"
#include "tbb/concurrent_unordered_map.h"
#include <tbb/concurrent_queue.h>
#include <fstream>
#include <sstream>
#include "Profiler.h"
#include "cuckoohash_map.hh"

using namespace std;

const static int N = 150;

mutex mu;

void idk();

void fileStuff(string filename);

struct  S{
    int x, y;
    
    S(int _x, int _y) : x(_x), y(_y){;}
    S(){x =  y = 0;}
    
};


//0...3


enum e{
    a,b,c,d=10,f
    
};
int oddFib(int num){
    int extra;
    if(num <  1)
        return 1;
    if(num == 2)
        return 2;
    if(!(num % 4))
        extra =  2;
    else
        extra = oddFib(num -3);
    return oddFib(num-1) * extra + oddFib(num- 2);
    

    
}

void mut(vector<int>& arr){
    for(int i = 1; i < arr.size(); ++i){
        for(int j = i; j < 0; j--){
            if(arr[j-1]%2>= arr[j] %2) swap(arr[j-1], arr[j]);
            else break;
        }
    }
    
    
    
}

class XX{
public:
    XX(uint v) : x(v), z(v*2){
    }
   const  uint x;
    const short z;
    
   
};




int main(int argc, const char * argv[]) {
    
//    vector<long> vect;
//    
//    for(int i =  0; i < 33; ++i){
//        
//        vect.push_back(i);
//        std::cout << i << "     " << vect.capacity() << std::endl;
//        
//        
//        
//    }
//    
//    unordered_map<int, S> m;
//    m.emplace(1, S(3,1));
//    m.at(1).x = 10;
//    
//   cout <<  m.at(1).x << "    " << m[1].y << endl;
    
    auto vect = new vector<int>(3);
    vect->push_back(3); vect->push_back(4); vect->push_back(3); vect->push_back(4);
    cout << vect->capacity() << endl;
    
    

    
}

void fileStuff(std::string filename){
    
    ifstream file(filename);
    std::string line;
    
    if(!file.is_open())
        std::cerr << "Issue reading file: " << filename << std::endl;
    
    std::getline(file, line); //ignore the first line
    
    int vertex; char garbage; int neighbor;
    
    while(std::getline(file, line)){
        std::istringstream iss(line);
        
        iss >> vertex >> garbage;
        
        std::cout << "vertex: " << vertex << "  Neighbors:  ";
        
        while(iss >> neighbor)
            std::cout  << neighbor << "  ";
        
        std::cout << endl;
        
        
    }
    
    
    
}



struct Node{
    
    int* cell;
    int vertex;
    
    Node(){};
    
    Node(const int _vertex) : vertex(_vertex){;}
    
    ~Node(){
        cout << "deleted" << endl;
    }
    
    static thread_local int thrd;
    
};

int thread_local Node::thrd = 5;

void idStuff(Node* node, int t){
    
    cout << "first time " << node->thrd << endl;
    
    node->thrd = t;


    cout << "second time "  << node->thrd << endl;

}

class NodeDeleter{
public:
    int i = 2;
    void operator()(Node* const n){
        ++i;
        cout << "node deleter: " << n->vertex << "   " << i << endl;
    }
    
    static void deletee(Node* const n){
        cout << "node deleter:  2 " << (n->vertex * 100)<< endl;
    }
};

void idk(){
//    
//    Node* node = new Node(1);
//    
//    shared_ptr<Node> ptr(node);
//    
//    Node* node2 = new Node(2);
//    
//    ptr.reset(node2, NodeDeleter());
//    
//    Node* node4 = new Node(4);
//
//    
//    ptr.reset(node4, NodeDeleter());
//
//    
//    ptr.reset(node2, NodeDeleter::deletee);
//
//    
//    Node* node3 = new Node(3);
//    
//    ptr.reset(node3);
//
   // ptr2.reset((node2), NodeDeleter());
    
    
   vector<thread> thds;
    int T(5);
    Node node; Node n2;
    
    for(int i = 0; i < T; ++i)
        thds.emplace_back(idStuff, &node, i);
    
    for(thread& t: thds)
        t.join();
    

}















