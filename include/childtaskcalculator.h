#ifndef CHILDTASKCALCULATOR
#define CHILDTASKCALCULATOR

#include "signalplatform.h"
#include "signalcalculator.h"
#include "kdbnode.h"

class businessdatecalculator: public taskgenerator{
public:
    businessdatecalculator(simplecalculator *ksetup):taskgenerator()
    {
        busdays = std::make_tuple(ksetup, kdbsetup::DATESINRANGE,Param());
    };

    virtual KFC* generatechildtasks(KFCStore * variablecache,  std::map<CalculationLevel,KFC> curtask){
        KFC datesinrange = CalcValueNodeUtil::getvalue(variablecache, busdays);
        KFC *rtn = new KFC[datesinrange->n + 1];
        for (int i=0;i<datesinrange->n;i++)
            rtn[i] = buildKFC(kI(datesinrange)[i], KD);
        rtn[datesinrange->n] = EMPTYKFC;
        return rtn;
    };

    virtual bool serializecalctrace(){
        return true;
    };


    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
       KFC ds = CalcValueNodeUtil::getvalue(variablecache, busdays);
       assert( 0!= ds);

       std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Top, buildKFC("unitest",KS)}};
       auto rtn = generatechildtasks(variablecache, curtask);
       std::cout << "following tasks being created" << std::endl;
       auto start = rtn;
       while ((*start)!=EMPTYKFC){
        std::cout << *start << std::endl;
        start++;
       }
        return true;
    };

    CalcValueNode busdays;

};
























#endif