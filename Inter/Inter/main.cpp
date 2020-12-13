//
//  main.cpp
//  Inter
//
//  Created by Alex Zabrodskiy on 1/20/18.
//  Copyright © 2018 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>

void reverseWords(char* str);

void reverseWord(char* begin, char* end);

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    std::string strMsg = "Hello world what up!";
    
    char* msg = (char*) strMsg.c_str();
    
    reverseWords(msg);
    std::cout << msg << std::endl;
    
    return 0;
}

void reverseWords(char* string){
    
    if(!string) return;
    char* end = string;
    
    while(*(end+1)) ++end;
    
    char* begin = string;

    reverseWord(begin, end); //First reverse whole sentence
    
    //Then individual words
    
    end = begin;
    
    //get to first alphanumeric character
    
    while(*begin){
        
        if(!isalnum(*begin)) ++begin;
        
        else{
            end = begin + 1;
            while(*end && isalnum(*end)){++end;}
            --end;
            reverseWord(begin, end);
            begin = end+1;
            
        }
    
    }
    
}

void reverseWord(char* begin, char* end){
    char temp;
    while(end > begin){
        temp = *begin;
        *begin = *end;
        *end = temp;
        ++begin;
        --end;
    
    }
    
    
    
    
}



