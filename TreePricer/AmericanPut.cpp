//
//  AmericanPut.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include "AmericanPut.hpp"
#include "BTree.hpp"

AmericanPut::AmericanPut(const EuropeanOption& option) : option_(option) {}

TreeResult AmericanPut::BinomialTree(std::size_t steps, const TreeModifier &modifier, bool variance_reduction) const {
    
    BTree american_tree(option_, steps);
    
    TreeResult res = american_tree.EarlyExerciseOption(option_.PutPayoff(), modifier);
    
    if (variance_reduction) {
        BTree european_tree(option_, steps);
        
        TreeResult euro_res = european_tree.PathIndependentOption(option_.PutPayoff(), modifier);
        
        res.value -= (euro_res.value - option_.Put());
        res.delta -= (euro_res.delta - option_.DeltaPut());
        res.gamma -= (euro_res.gamma - option_.GammaPut());
        res.theta -= (euro_res.theta - option_.ThetaPut());
    }
    
    return res;
}
