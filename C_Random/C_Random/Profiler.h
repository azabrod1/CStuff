//
//  Profiler.h
//  C_Random
//
//  Created by Alex Zabrodskiy on 6/8/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef Profiler_h
#define Profiler_h

//
//  Profiler.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/8/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include<iostream>
#include<chrono>

class Profiler{
    
    std::chrono::time_point<std::chrono::high_resolution_clock> time;
    std::chrono::duration<double> accum;
    bool accumInit = false;
    
public:
    Profiler() : time(std::chrono::high_resolution_clock::now()){
        
    }
    void begin(){
        time = std::chrono::high_resolution_clock::now();
    }
    
    void accumulate(){
        auto finish = std::chrono::high_resolution_clock::now();
        
        if(!accumInit){
            accum = finish - time;
            accumInit = true;
        }
        
        else
            accum += (finish - time);
        
    }
    
    void resetAccum(){
        accumInit = false;
    }
    
    void printAccum(std::string&& msg = ""){
        if(msg != "")
            std::cout << "\nTime elapsed for " << msg << ":  "<< accum.count() << " seconds."<< std::endl;
        else
            std::cout << "\nTime elapsed:  " << accum.count() << " seconds."<< std::endl;
        
        
        
    }
    
    double getAccum(){
        return accum.count();
    }
    
    
    
    
    
    
    void end(std::string&& msg = ""){
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - time;
        if(msg != "")
            std::cout << "Time elapsed for " << msg << ":  "<< elapsed.count() << " seconds."<< std::endl;
        else
            std::cout << "Time elapsed:  " << elapsed.count() << " seconds."<< std::endl;
    }
    
    long long returnEnd(){
        return (std::chrono::high_resolution_clock::now() - time).count();
    }
    
    
};


#endif /* Profiler_h */
