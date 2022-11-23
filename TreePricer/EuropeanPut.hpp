//
//  EuropeanPut.hpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#ifndef EuropeanPut_hpp
#define EuropeanPut_hpp

#include "TreePricer.hpp"
#include "EuropeanOption.hpp"

class EuropeanPut {
private:
    EuropeanOption option_;
    
public:
    EuropeanPut(double S, double K, double T, double sigma, double r, double q);
    EuropeanPut(const EuropeanOption& option);
    
    TreeResult BinomialTree(std::size_t steps, const TreeModifier& modifier) const;
    TreeResult TrinomialTree(std::size_t steps, const TreeModifier& modifier) const;
};

#endif /* EuropeanPut_hpp */
