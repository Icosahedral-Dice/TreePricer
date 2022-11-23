//
//  main.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include <iostream>
#include "AmericanPut.hpp"

void TestBasicBTree() {
    EuropeanOption option(0., 54., 50., 1., .29, .0375, .01);
    AmericanPut ameriput(option);
    
    TreeResult res = ameriput.BinomialTree(4000, BSR, true);
    res.Print();    // 3.66838    -0.314151    0.0234312    -2.269
}

int main(int argc, const char * argv[]) {
    
//    TestBasicBTree();
    
    return 0;
}
