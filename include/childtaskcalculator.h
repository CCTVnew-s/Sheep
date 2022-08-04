#ifndef CHILDTASKCALCULATOR
#define CHILDTASKCALCULATOR

#include "signalplatform.h"
#include "signalcalculator.h"
#include "kdbnode.h"

class businessdatecalculator: public taskgenerator{
public:
    static std::string GETBUSDATE;
    static std::string Description;

    businessdatecalculator(simplecalculator *ksetup):taskgenerator(GETBUSDATE,CalculationLevel::Top, ExecutorPhase::Preloop,std::vector<CalcValueNodes>(), Description)
    {
        datesinrange = std::make_tuple(ksetup,kdbsetup::DATESINRANGE, Param());
        inputvars.push_back(CalcValueNodes(std::vector<CalcValueNode>({datesinrange})));
    };

    virtual KFC* generatechildtasks(KFCStore * variablecache,  std::map<CalculationLevel,KFC> curtask){
        KFC dates = CalcValueNodeUtil::getvalue(variablecache, datesinrange);
        KFC *rtn = new KFC[dates->n + 1];
        for (int i=0;i<dates->n;i++)
            rtn[i] = buildKFC(kI(dates)[i], KD);
        rtn[dates->n] = EMPTYKFC;
        return rtn;
    };

    virtual bool serializecalctrace(){
        return true;
    };

    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
       KFC ds = CalcValueNodeUtil::getvalue(variablecache, datesinrange);
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

    CalcValueNode datesinrange;
};

// split all the tables generated 
class symboltaskcalculator{



};























#endif