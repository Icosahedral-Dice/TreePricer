//
//  main.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include <iostream>
#include "EuropeanPut.hpp"
#include "AmericanPut.hpp"
#include <iomanip>
#include <cmath>
#include "HW11.hpp"

void TestBasicBTree() {
    EuropeanOption option(0., 54., 50., 1., .29, .0375, .01);
    EuropeanPut europut(option);
    AmericanPut ameriput(option);
    
    std::cout << "European" << std::endl;
    europut.BinomialTree(1280, vanilla).Print();
    europut.BinomialTree(1280, average).Print();
    europut.BinomialTree(1280, BS).Print();
    europut.BinomialTree(1280, BSR).Print();
    
    std::cout << "American" << std::endl;
    ameriput.BinomialTree(1280, vanilla, false).Print();
    ameriput.BinomialTree(1280, average, false).Print();
    ameriput.BinomialTree(1280, BS, false).Print();
    ameriput.BinomialTree(1280, BSR, false).Print();
    
    std::cout << "American, variance reduction" << std::endl;
    ameriput.BinomialTree(1280, vanilla, true).Print();
    ameriput.BinomialTree(1280, average, true).Print();
    ameriput.BinomialTree(1280, BS, true).Print();
    ameriput.BinomialTree(1280, BSR, true).Print();
}

void TestBasicTTree() {
    EuropeanOption option(0., 41., 39., 1., .25, .03, .005);
    EuropeanPut europut(option);
    AmericanPut ameriput(option);
    
    std::cout << "European" << std::endl;
    europut.TrinomialTree(1280, vanilla).Print();
    europut.TrinomialTree(1280, BS).Print();
    europut.TrinomialTree(1280, BSR).Print();
    
    std::cout << "American" << std::endl;
    ameriput.TrinomialTree(1280, vanilla, false).Print();
    ameriput.TrinomialTree(1280, BS, false).Print();
    ameriput.TrinomialTree(1280, BSR, false).Print();
    
    std::cout << "American, variance reduction" << std::endl;
    ameriput.TrinomialTree(1280, vanilla, true).Print();
    ameriput.TrinomialTree(1280, BS, true).Print();
    ameriput.TrinomialTree(1280, BSR, true).Print();
}

void TestDiscreteDividendEuropeanPut() {
    // 2% after 2 months
    EuropeanOption option(0., 50., 55.55, .25, .2, .02, 0.);
    EuropeanPut europut(option);
    Dividend proportional({{2./12.}, {.02}});
    Dividend fixed;
    europut.BinomialTree(1536, vanilla, proportional, fixed).Print();
    
    EuropeanOption option2(0., 49., 55.55, .25, .2, .02, 0.);
    EuropeanPut europut2(option2);
    std::cout << option2.Put() << '\t' << option2.DeltaPut() << '\t' << option2.GammaPut() << '\t' << option2.ThetaPut() << std::endl;
    europut2.BinomialTree(1536, vanilla).Print();
}

void TestDiscreteDividendAmericanPut() {
    // 2% after 2 months
    EuropeanOption option(0., 50., 55.55, .25, .2, .02, 0.);
    AmericanPut europut(option);
    Dividend proportional({{2./12.}, {.02}});
    Dividend fixed;
    europut.BinomialTree(6, vanilla, proportional, fixed).Print();
}



int main(int argc, const char * argv[]) {
    
    std::cout << std::fixed << std::setprecision(6);
//    TestBasicBTree();
//    TestBasicTTree();
    
//    TestDiscreteDividendEuropeanPut();
//    TestDiscreteDividendAmericanPut();
    
    HW11();
    
    return 0;
}
