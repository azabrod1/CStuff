//
//  main.cpp
//  Fun
//wwaq2  bhbb  m                                                                                                                                                                                                                                                                                                                                zzc
//  Created by Alex Zabrodskiy on 3/25/18.
//  Copyright © 2018 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include <vector>
#define String std::string

using namespace std;vb b67
    
    cout << *ptr2 << endl;
    
    
    
    
    
    return 0;
}


std::vector<int>* subArray(int* array, int target){
    
    //std::vector<int>* const ret = new std::vector<int>;
    int sum = 0;
    int* left = array;
    
    for(int* right = array; right < array + sizeof(array); ++right){
        sum += *right;

        if(sum == target)
            return new std::vector<int>(left, right+1);
        
        while(sum > target){
            sum -= *(left++);
            cout << sum << endl;
        }
        
        if(sum == target)
            return new std::vector<int>(left, right+1);
        
    }
    
    return nullptr;
    
}


std::string wordReverse(String* const str){
    
    char * newString = new char[str->length()];
    int l = 0;
    
    for(int r = 1; r < str->length(); ++r){

        if((*str)[r] == ' '){
            for(int i = l; i < r; ++i)
                newString[i] = str->data()[r-(i-l)-1];
            newString[r] = ' ';
            l = r+1;
        }
    }
    
    for(int i = l; i < str->length(); ++i){
        newString[i] = str->data()[ str->length()-(i-l)-1];
    }
    //std::cout<< "ree " << (newStrin << "   " << str->length() <<std::endl;
    
    return (std::string) newString;
}
