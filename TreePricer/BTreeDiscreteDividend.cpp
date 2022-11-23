//
//  BTreeDiscreteDividend.cpp
//  TreePricer
//
//  Created by 王明森 on 11/23/22.
//

#include "BTreeDiscreteDividend.hpp"
#include <cmath>

BTreeDiscDiv::BTreeDiscDiv(double S0, double sigma, double T, std::size_t steps, double r, double q, const Dividend& proportional, const Dividend& fixed) : BTree(S0, sigma, T, steps, r, q), proportional_(proportional), fixed_(fixed) {
    assert(q == 0.);
    equivalent_S0_ = BTreeDiscDiv::FindEquivalentS0(S0, proportional, fixed, r);
}

BTreeDiscDiv::BTreeDiscDiv(const EuropeanOption& option, std::size_t steps, const Dividend& proportional, const Dividend& fixed) : BTree(option, steps), proportional_(proportional), fixed_(fixed) {
    assert(option.q_ == 0.);
    equivalent_S0_ = BTreeDiscDiv::FindEquivalentS0(option.S_, proportional, fixed, option.r_);
}

double BTreeDiscDiv::FindEquivalentS0(double S0, const Dividend& proportional, const Dividend& fixed, double r) {
    
    double S = S0;
    
    auto tit_prop = proportional.dates.cbegin();
    auto dit_prop = proportional.dividends.cbegin();
    auto tit_fixed = fixed.dates.cbegin();
    auto dit_fixed = fixed.dividends.cbegin();
    
    while ((tit_prop != proportional.dates.cend()) || (tit_fixed != fixed.dates.cend())) {
        if (tit_prop == proportional.dates.cend()) {
            // Proportional dividends exhausted, apply all remaining fixed dividends
            while (tit_fixed != fixed.dates.cend()) {
                S -= (*dit_fixed) * std::exp(-r * (*tit_fixed));
                
                dit_fixed++;
                tit_fixed++;
            }
            break;
        } else if (tit_fixed == fixed.dates.cend()) {
            // Fixed dividends exhausted, apply all remaining proportional dividends
            while (tit_prop != proportional.dates.cend()) {
                S *= (1. - *dit_prop);
                
                dit_prop++;
                tit_prop++;
            }
            break;
        } else {
            // Both types of dividends expected. Apply the earliest dividend possible.
            // Proportional dividends go first if (in the unlikely case that) two dividends coincide in time
            if ((*tit_fixed) < (*tit_prop)) {
                // The fixed dividend is earlier
                S -= (*dit_fixed) * std::exp(-r * (*tit_fixed));
                
                dit_fixed++;
                tit_fixed++;
            } else {
                // The proportional dividend is earlier
                S *= (1. - *dit_prop);
                
                dit_prop++;
                tit_prop++;
            }
        }
    }

//    // The "naive" algorithm
//    for (auto dit = proportional.dividends.cbegin(); dit != proportional.dividends.cend(); dit++) {
//        S *= (1. - *dit);
//    }
//
//    for (auto tit = fixed.dates.cbegin(), dit = fixed.dividends.cbegin(); tit != fixed.dates.cend() && dit != fixed.dividends.cend(); tit++, dit++) {
//        S -= (*dit) * std::exp(-r * (*tit));
//    }
    
    return S;
}

TreeResult BTreeDiscDiv::PathIndependentOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const {
    // Since the option is path independent, we can simply construct a continuous-dividend binomial tree with adjusted S0 for pricing.
    BTree continuous_dividend_tree(equivalent_S0_, sigma_, T_, steps_, r_, q_);
    return continuous_dividend_tree.PathIndependentOption(payoff, modifier);
}
