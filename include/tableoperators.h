/*
 * Column level operators, similar to kdb, should be based on column calculators
/* 
 * File:   tableoperators.h
 * Author: xiez
 *
 * Created on August 11, 2022, 8:05 AM
 */

#ifndef TABLEOPERATORS_H
#define TABLEOPERATORS_H

#include "k.h"
#include "kutil.h"
#include "arraycalc.h"

template <class T>
class tableoperator{
public:
    virtual bool operator()(tableview*, T* output);
};














#endif /* TABLEOPERATORS_H */

