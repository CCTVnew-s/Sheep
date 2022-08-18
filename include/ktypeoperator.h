/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ktypeoperator.h
 * Author: xiez
 *
 * Created on August 11, 2022, 7:52 AM
 */

#ifndef KTYPEOPERATOR_H
#define KTYPEOPERATOR_H

#include <string>
#include <regex>
#include "k.h"

namespace CALC{


// string like

class symbolfilter{
public:
    symbolfilter(std::string p):pattern(p){};

    virtual bool filter(std::string can){
        return std::regex_match(can, std::regex(pattern));
    };
    std::string pattern;
};




// int ,long, double, real, common operators, +-*/ etc.
// need to handle NULL, Inf, Inf


// Time related operators
template <class T1, class T2>
bool winthin(T1& x, T2& lb, T2&ub) {return (x>=lb) && (x<=ub);};


I timetype(int h=0, int m=0, int s=0,int mili=0){
    return ((((h*60)+m)*60)+s)*1000 + mili;
};

I datetype(int y, int m, int d){
    return ymd(y,m,d);
};




};







#endif /* KTYPEOPERATOR_H */

