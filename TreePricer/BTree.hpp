//
//  BTree.hpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#ifndef BTree_hpp
#define BTree_hpp

#include <array>
#include <vector>
#include <functional>
#include <deque>
#include "EuropeanOption.hpp"
#include "TreePricer.hpp"


class BTree {
    // Binomial tree pricer
    // Underlying asset follows a lognormal distribution
    // Continuous dividend
    // PI: path independent
    // EE: early exercise
protected:
    // Constructor takes in those values
    double S0_;     // Spot price at t = 0
    double sigma_;  // Volatility
    double T_;      // Maturity
    double steps_;  // Steps
    double r_;      // Interest rate
    double q_;      // Dividend rate
    
    // Helper values
    double dt_;     // delta t = T / steps
    double u_;      // u = exp(sigma * sqrt(dt))
    double d_;      // d = 1 / u
    double r_disc_; // exp(-r*dt)
    double p_;      // (exp((r - q) * dt) - d) / (u - d)
    double disc_p;  // disc * p
    double disc_1p; // disc * (1 - p)
    
public:
    BTree(double S0, double sigma, double T, std::size_t steps, double r, double q);
    BTree(const EuropeanOption& option, std::size_t steps);
    ~BTree() = default;
    
    TreeResult PathIndependentOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const;
    TreeResult EarlyExerciseOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const;
    
protected:
    void BacktrackPI(std::vector<double>& V_mesh) const;
    void BacktrackEE(std::vector<double>& V_mesh, std::array<std::deque<double>, 2>& S_mesh, size_t curr_step, const std::function<double (double)>& payoff_T) const;
    
    TreeResult PIVanilla(const std::function<double (double, double)>& payoff) const;
    TreeResult PIAvg(const std::function<double (double, double)>& payoff) const;
    TreeResult PIBS(const std::function<double (double, double)>& payoff) const;        // Black-Scholes
    TreeResult PIBSR(const std::function<double (double, double)>& payoff) const;       // Black-Scholes with Richardson extrapolation
    
    TreeResult EEVanilla(const std::function<double (double, double)>& payoff) const;
    TreeResult EEAvg(const std::function<double (double, double)>& payoff) const;
    TreeResult EEBS(const std::function<double (double, double)>& payoff) const;        // Black-Scholes
    TreeResult EEBSR(const std::function<double (double, double)>& payoff) const;       // Black-Scholes with Richardson extrapolation
    
//    TreeResult EEVanilla_VarRed(const std::function<double (double, double)>& payoff) const;
//    TreeResult EEAvg_VarRed(const std::function<double (double, double)>& payoff) const;
//    TreeResult EEBS_VarRed(const std::function<double (double, double)>& payoff) const;
//    TreeResult EEBSR_VarRed(const std::function<double (double, double)>& payoff) const;
};

#endif /* BTree_hpp */
