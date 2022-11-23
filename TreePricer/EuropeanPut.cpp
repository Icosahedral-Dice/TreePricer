//
//  EuropeanPut.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include "EuropeanPut.hpp"
#include "BTree.hpp"
#include "TTree.hpp"
#include "BTreeDiscreteDividend.hpp"

EuropeanPut::EuropeanPut(double S, double K, double T, double sigma, double r, double q) : option_(0., S, K, T, sigma, r, q) {}
EuropeanPut::EuropeanPut(const EuropeanOption& option) : option_(option) {}

TreeResult EuropeanPut::BinomialTree(std::size_t steps, const TreeModifier &modifier) const {
    
    BTree european_tree(option_, steps);
    
    TreeResult res = european_tree.PathIndependentOption(option_.PutPayoff(), modifier);
    
    return res;
}

TreeResult EuropeanPut::TrinomialTree(std::size_t steps, const TreeModifier &modifier) const {
    
    TTree european_tree(option_, steps);
    
    TreeResult res = european_tree.PathIndependentOption(option_.PutPayoff(), modifier);
    
    return res;
}

TreeResult EuropeanPut::BinomialTree(std::size_t steps, const TreeModifier& modifier, const Dividend& proportional, const Dividend& fixed) const {
    
    BTreeDiscDiv european_tree(option_, steps, proportional, fixed);
    
    TreeResult res = european_tree.PathIndependentOption(option_.PutPayoff(), modifier);
    
    return res;
}
