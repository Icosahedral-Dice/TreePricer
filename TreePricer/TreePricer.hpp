//
//  TreePricer.hpp
//  TreePricer
//
//  Created by 王明森 on 11/22/22.
//

#ifndef TreePricer_hpp
#define TreePricer_hpp

#include <iostream>

struct TreeResult {
    double value;
    double delta;
    double gamma;
    double theta;
    
    void Print() const {
        std::cout << value << '\t' << delta << '\t' << gamma << '\t' << theta << std::endl;
    }
};

enum TreeModifier {
    vanilla,
    average,    // Average tree
    BS,         // Black-Scholes
    BSR         // Black-Scholes with Richardson extrapolation
};


#endif /* TreePricer_hpp */
