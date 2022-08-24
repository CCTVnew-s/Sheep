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




#define DEFINETYPEBUILD(type)  ARRAY::var<type> ARRAY::buildvar(type &x,int l){ return var<type>(x); };      ARRAY::var<type> ARRAY::buildvar(type* x,int l){return var<type>(x,l);};
DEFINETYPEBUILD(bool)
DEFINETYPEBUILD(int)
DEFINETYPEBUILD(short)
DEFINETYPEBUILD(char)
DEFINETYPEBUILD(long)
DEFINETYPEBUILD(double)
DEFINETYPEBUILD(float)