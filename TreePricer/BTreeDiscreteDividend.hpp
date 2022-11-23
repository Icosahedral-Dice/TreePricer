//
//  BTreeDiscreteDividend.hpp
//  TreePricer
//
//  Created by 王明森 on 11/23/22.
//

#ifndef BTreeDiscreteDividend_hpp
#define BTreeDiscreteDividend_hpp

#include "BTree.hpp"

class BTreeDiscDiv : public BTree {
    // Binomial tree pricer
    // Underlying asset follows a lognormal distribution
    // Allows discrete dividends
    // PI: path independent
    // EE: early exercise
protected:
    Dividend proportional_;
    Dividend fixed_;
    double equivalent_S0_;
    
public:
    BTreeDiscDiv(double S0, double sigma, double T, std::size_t steps, double r, double q, const Dividend& proportional, const Dividend& fixed);
    BTreeDiscDiv(const EuropeanOption& option, std::size_t steps, const Dividend& proportional, const Dividend& fixed);
    ~BTreeDiscDiv() = default;
    
    TreeResult PathIndependentOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const;
    
    TreeResult PIVanilla(const std::function<double (double, double)>& payoff) const;
    
protected:
    static double FindEquivalentS0(double S0, const Dividend& proportional, const Dividend& fixed, double r);
    
};

#endif /* BTreeDiscreteDividend_hpp */
