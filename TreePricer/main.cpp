//
//  main.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include <iostream>
#include "AmericanPut.hpp"
#include <iomanip>

void TestBasicBTree() {
    EuropeanOption option(0., 54., 50., 1., .29, .0375, .01);
    AmericanPut ameriput(option);
    
    ameriput.BinomialTree(1280, vanilla, true).Print();
    ameriput.BinomialTree(1280, average, true).Print();
    ameriput.BinomialTree(1280, BS, true).Print();
    ameriput.BinomialTree(1280, BSR, true).Print();
}

void TestBasicTTree() {
    EuropeanOption option(0., 41., 39., 1., .25, .03, .005);
    AmericanPut ameriput(option);
    
    ameriput.TrinomialTree(1280, vanilla, true).Print();
    ameriput.TrinomialTree(1280, BS, true).Print();
    ameriput.TrinomialTree(1280, BSR, true).Print();
}

int main(int argc, const char * argv[]) {
    
    std::cout << std::fixed << std::setprecision(6);
//    TestBasicBTree();
    TestBasicTTree();
    
    return 0;
}
