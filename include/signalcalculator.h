#ifndef SIGNALCALCULATOR
#define SIGNALCALCULATOR

#include <vector>
#include <string>
#include <set>
#include <tuple>

#include "variablestore.h"
#include "kutil.h"
#include "signalplatform.h"



// ID, and relation 
// Very Beginning node, kdb source data -> next level ...



class graphnode{

public:

graphnode(std::vector<graphnode*> upstreamnode,std::string name, std::string description = "");

// avoid duplicated graph names
static std::set<std::string> allgraphnames;
std::string name;
std::string description;
std::vector<graphnode*> upstreamnode;
bool valid; // not valid, if name duplicated

};

 

// param design, to support cross product of params
// <param item ID, value, lexical expression

typedef std::tuple<std::string, KFC, std::string> SingleParamItem;

class Param{
public:
    
    Param(const Param& p):items(p.items){};
    Param(std::map<std::string, SingleParamItem> i):items(i) {};

    // sorted, as long as items the same, order doesn't matter
    std::map<std::string, SingleParamItem>  items;
    // sorted by name,
    std::string getParamID();
    KFC getValue(std::string name) {return std::get<1>(items.at(name));}

    Param operator + (Param& p1);

    bool IsNull() {return items.size() == 0;} 

};


class ParamSets{
public:
    ParamSets(std::vector<Param> params);
    virtual std::vector<Param> getParamSeries();

};

// complete cross product, if you want some specific rules, has to do it manually
class CrossProductParamSets: public ParamSets{
public:
    CrossProductParamSets(ParamSets& s1);

    CrossProductParamSets(ParamSets& s1, ParamSets& s2);

    CrossProductParamSets(CrossProductParamSets& c1, ParamSets& s1);

    CrossProductParamSets(CrossProductParamSets& c1, CrossProductParamSets& s2);


    std::vector<ParamSets> internals;
    virtual std::vector<Param> getParamSeries();

};


// this is value nodes, it means one result, or  one column of results, or one set ofcolums, but all meanning the same thing, based on different configs (ParamSets)
// One calc node can generate multiple output nodes, 
// If calc node need inputs of CalcValueNode, it might explicityly specify what exactly it needs
typedef std::tuple<simplecalculator *, std::string, ParamSets*> CalcValueNode;
// if there is no param dependent, ParamSet * could be set to 0
// As long as ParamSets* generate the same  getParamSeries(), they're the same params 




// calculator dependency  1) node to node 2) variable to variable 
class simplecalculator:private graphnode{

public:
    simplecalculator(CalculationLevel calclevel, std::vector<CalcValueNode> inputvars, std::vector<CalcValueNode> outputvars, std::string description);

    bool checkdependency(KFCStore * variablecache);

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManager * memmgr) = 0;

    std::string getCalcID() { return std::string("name:") + name + std::string("|Calclevel:") + std::to_string(int(calclevel)) +  std::string("|") ;};

    virtual bool serializecalctrace() = 0;

private:
    CalculationLevel calclevel;
    std::vector<CalcValueNode> inputvars;
    std::vector<CalcValueNode> outputvars;
    std::string description;

    std::vector<simplecalculator *> dependencynode;
};

//all type of calculators 1) source node 2) task generator 3) calculator 4) dump node


class taskgenerator:public simplecalculator{

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManager * memmgr) ;

    virtual KFC* generatechildtasks(KFCStore * variablecache, KFCStore *taskcontext, MemoryManager * memmgr) = 0;

};




// build or extend graph from nodes

class graph{

public: 

    graph():nodes(std::map<CalculationLevel, std::vector<simplecalculator *>>()){};
    
    bool addnodes(simplecalculator * node);

    std::map<CalculationLevel, std::vector<simplecalculator *>> nodes;

    bool toCalculationMap();

}

 





#endif