/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   arraycalc.cpp
 * Author: xiez
 * 
 * Created on August 17, 2022, 3:09 PM
 */

#include "arraycalc.h"

template <typename T>
CALC::var<T> CALC::getv(T& ax){
    return var<T>(ax);
}

template <typename T>
CALC::var<T> CALC::getv(T* ax){
    return var<T>(ax);
}
