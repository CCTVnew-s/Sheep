#ifndef KDBNODE
#define KDBNODE


#include "consts.h"
#include "variablestore.h"
#include "logging.h"
#include "k.h"
#include "kutil.h"
#include "signalcalculator.h"
#include <vector>


// starts with simple caclulator

// (std::string name, CalculationLevel calclevel, ExecutorPhase phase, std::vector<CalcValueNodes> inputvars, std::vector<CalcValueNodes> outputvars, std::string description):

class kdbsetup:public generalcalculator
{
static std::string DATESINRANGE;
static std::string Description;
public:
    kdbsetup(RUNENV e, I handle, I sd, I ed):generalcalculator("KDBSetup",CalculationLevel::Top, ExecutorPhase::Preloop, std::vector<CalcValueNodes>(),std::vector<CalcValueNodes>(), Description),
    handle(handle),e(e),sd(sd),ed(ed){
        this->description = "Set up kdb dabase,util function based on env, get dates in range";
        this->outputvars.push_back(CalcValueNodes(this,DATESINRANGE));
        if (e == RUNENV::h1)
            query = "{[x;y] exec date from select distinct date from book where date within (x,y)}";
        else
            query = "{[x;y] exec date from select distinct date from snapshot_SS where date within (x,y)}";
    }

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask){
        if (e == RUNENV::h1)
            k(handle,"\\l /datassd/qhdb", K(0));
         K rtnp = k(handle, (S) query.c_str(), kd(sd), kd(ed), K(0));
        // from this K to our KFC type
         KFC rtn = buildsKFCs(rtnp->G0, KD, rtnp->n);
        return this->setvariableincache(variablecache, DATESINRANGE, Param(),rtn);
    }

    virtual bool serializecalctrace(){
        std::cout << "send out query \\l /datassd/qhdb";
        std::cout << "query result from "<< query << "  with " << sd <<" and " << ed << std::endl;
        return true;  
    };

    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
        KFCStore taskcontext;
        std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Top, buildKFC("unitest",KS)}};

        bool status =  calculate( variablecache, &taskcontext, *(mgr->getMemoryManagerSet(CalculationLevel::Top)), curtask);
        if (status){
            std::cout << "unit test for set up kdb \n";
            std::cout << "we have set " << CalcValueNodeUtil::getStoreKey(outputvars.at(0).nodes().at(0)) << "with value \n";
            std::cout << CalcValueNodeUtil::getvalue(variablecache, outputvars.at(0).nodes().at(0));
        }
        return status;
    }

I handle;
RUNENV e;
I sd,ed;
std::string query;
};


class tablequeryexecutor{


};

class kdbdailydatamultithread:public generalcalculator
{
    kdbdailydatamultithread(RUNENV e, I handle, std::vector<tablequeryexecutor>, generalcalculator *kdbsetupcalc)





}



#endif