//
//  BTree.cpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#include "BTree.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

BTree::BTree(double S0, double sigma, double T, std::size_t steps, double r, double q) : S0_(S0), sigma_(sigma), T_(T), steps_(steps), r_(r), q_(q),
    dt_(T / steps),
    u_(std::exp(sigma * std::sqrt(dt_))),
    d_(1. / u_),
    r_disc_(std::exp(-r * dt_)),
    p_((std::exp((r - q) * dt_) - d_) / (u_ - d_)),
    disc_p(r_disc_ * p_),
    disc_1p(r_disc_ * (1. - p_))
{}

BTree::BTree(const EuropeanOption& option, std::size_t steps) : BTree(option.S_, option.sigma_, option.T_, steps, option.r_, option.q_) {}

TreeResult BTree::PathIndependentOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const {
    switch (modifier) {
        case TreeModifier::vanilla:
            // Vanilla tree
            return this->PIVanilla(payoff);
            break;
        case TreeModifier::average:
            // Average tree
            return this->PIAvg(payoff);
            break;
        case TreeModifier::BS:
            // Black-Scholes tree
            return this->PIBS(payoff);
            break;
        case TreeModifier::BSR:
            // Black-Scholes tree with Richardson extrapolation
            return this->PIBSR(payoff);
            break;
        default:
            // Default to vanilla tree
            return this->PIVanilla(payoff);
            break;
    }
}

TreeResult BTree::EarlyExerciseOption(const std::function<double (double, double)>& payoff, const TreeModifier& modifier) const {
    switch (modifier) {
        case TreeModifier::vanilla:
            // Vanilla tree
            return this->EEVanilla(payoff);
            break;
        case TreeModifier::average:
            // Average tree
            return this->EEAvg(payoff);
            break;
        case TreeModifier::BS:
            // Black-Scholes tree
            return this->EEBS(payoff);
            break;
        case TreeModifier::BSR:
            // Black-Scholes tree with Richardson extrapolation
            return this->EEBSR(payoff);
            break;
        default:
            // Default to vanilla tree
            return this->EEVanilla(payoff);
            break;
    }
}

void BTree::BacktrackPI(std::vector<double>& V_mesh) const {
    for (auto Vit = V_mesh.begin(); Vit + 1 != V_mesh.end(); Vit++) {
        // Get value of the last node by taking the expectation and discounting
        // V = disc(p_u * V_u + p_d * V_d)
        *Vit = disc_p * (*Vit) + disc_1p * (*(Vit + 1));
    }
    
    // Shrink V since there are fewer nodes needed.
    V_mesh.pop_back();
}

void BTree::BacktrackEE(std::vector<double>& V_mesh, std::array<std::deque<double>, 2>& S_mesh, size_t curr_step, const std::function<double (double)>& payoff_T) const {
    // We maintain two vectors of S and we use them alternatingly.
    
    // The current S[i] always has (curr_step + 1) elements.
    // The other has (curr_step) elements.
    
    size_t which_S = (S_mesh[1].size() == (curr_step + 1));
    
    // Find the payoff if we exercise the option at this step
    std::vector<double> exercise_payoff(S_mesh[which_S].cbegin(), S_mesh[which_S].cend());
    std::transform(exercise_payoff.cbegin(), exercise_payoff.cend(), exercise_payoff.begin(), payoff_T);
    
    auto payoff_it = exercise_payoff.cbegin();
    for (auto Vit = V_mesh.begin(); Vit + 1 != V_mesh.end(); Vit++) {
        // Get value of the last node by taking the expectation and discounting
        // V = disc(p_u * V_u + p_d * V_d)
        *Vit = disc_p * (*Vit) + disc_1p * (*(Vit + 1));
        
        // If early exercise is more profitable, replace value with early exercise payoff.
        if ((*payoff_it) > (*Vit)) {
            *Vit = *payoff_it;
        }
        
        payoff_it++;
    }
    
    // Shrink V since there are fewer nodes needed.
    V_mesh.pop_back();
    
    // Also shrink the current S since we will have less steps in the next two iterations.
    if (S_mesh[which_S].size() >= 2) {
        S_mesh[which_S].pop_back();
        S_mesh[which_S].pop_front();
    }
}

TreeResult BTree::PIVanilla(const std::function<double (double, double)>& payoff) const {
    
    // Generate asset price at maturity
    std::vector<double> S({S0_ * std::pow(u_, static_cast<double>(steps_))});
    double d2 = d_ * d_;
    for (std::size_t i = 0; i < steps_; i++) {
        S.push_back(S.back() * d2);
    }
    
    auto payoff_T = std::bind(payoff, std::placeholders::_1, T_);
    
    // Generate derivative value at maturity
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff_T);
    
    // Backtrack until t = 2 * dt (the resulting sub-tree is used for Greek calculation)
    std::size_t curr_step = steps_;
    while (curr_step > 2) {
        this->BacktrackPI(V);
        curr_step--;
    }
    
    // Store nodes of Step 2
    double V20 = V[0];
    double V21 = V[1];
    double V22 = V[2];
    
    // Backtrack
    this->BacktrackPI(V);
    
    // Store nodes of Step 2
    double V10 = V[0];
    double V11 = V[1];
    
    // Backtrack
    this->BacktrackPI(V);
    double V00 = V[0];
    
    double delta = (V10 - V11) / (S0_ * (u_ - d_));
    double gamma = ((V20 - V21) / (S0_ * (u_ * u_ - 1.)) - ((V21 - V22) / (S0_ * (1. - d_ * d_)))) / (.5 * S0_ * (u_ * u_ - d_ * d_));
    double theta = (V21 - V00)/ (2. * dt_);
    
    return TreeResult({V00, delta, gamma, theta});
}

TreeResult BTree::EEVanilla(const std::function<double (double, double)>& payoff) const {
    // Generate asset prices
    // S[0]: Asset price at maturity
    // S[1]: Asset price dt before maturity
    std::array<std::deque<double>, 2> S;
    S[0].push_back(S0_ * std::pow(u_, static_cast<double>(steps_)));
    S[1].push_back(S0_ * std::pow(u_, static_cast<double>(steps_) - 1.));
    
    double d2 = d_ * d_;
    for (std::size_t i = 0; i < steps_ - 1; i++) {
        S[0].push_back(S[0].back() * d2);
        S[1].push_back(S[1].back() * d2);
    }
    S[0].push_back(S[0].back() * d2);
    // Now, S[0] has steps_ + 1 elements and S[1] has steps_ elements
    // The current S[i] always has (curr_step + 1) elements.
    
    std::size_t curr_time = T_;
    std::size_t curr_step = steps_;
    std::vector<double> V(S[0].size());
    auto payoff_T = std::bind(payoff, std::placeholders::_1, curr_time);
    std::transform(S[0].cbegin(), S[0].cend(), V.begin(), payoff_T);
    S[0].pop_back();
    S[0].pop_front();
    curr_step--;
    curr_time -= dt_;
    
    while (curr_step >= 2) {
        payoff_T = std::bind(payoff, std::placeholders::_1, curr_time);
        // During backtracking, curr_step and curr_time are exactly the current step (0-indexed) and the current time
        this->BacktrackEE(V, S, curr_step, payoff_T);
        curr_step--;
        curr_time -= dt_;
    }
    
    // Store nodes of Step 2
    double V20 = V[0];
    double V21 = V[1];
    double V22 = V[2];
    
    // Backtrack
    payoff_T = std::bind(payoff, std::placeholders::_1, curr_time);
    this->BacktrackEE(V, S, curr_step, payoff_T);
    curr_step--;
    curr_time -= dt_;
    
    // Store nodes of Step 1
    double V10 = V[0];
    double V11 = V[1];
    
    // Backtrack
    payoff_T = std::bind(payoff, std::placeholders::_1, curr_time);
    this->BacktrackEE(V, S, curr_step, payoff_T);
    curr_step--;
    curr_time -= dt_;

    double V00 = V[0];
    double delta = (V10 - V11) / (S0_ * (u_ - d_));
    double gamma = ((V20 - V21) / (S0_ * (u_ * u_ - 1.)) - ((V21 - V22) / (S0_ * (1. - d_ * d_)))) / (.5 * S0_ * (u_ * u_ - d_ * d_));
    double theta = (V21 - V00)/ (2. * dt_);
    
    return TreeResult({V00, delta, gamma, theta});
}




