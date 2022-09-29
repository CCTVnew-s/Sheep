
#ifndef SIGNALCALCULATOR
#define SIGNALCALCULATOR

#include <vector>
#include <string>
#include <set>
#include <tuple>

#include "logging.h"
#include "variablestore.h"
#include "kutil.h"
#include "signalplatform.h"






// ID, and relation 
// Very Beginning node, kdb source data -> next level ...

//  Context ~ current task



class graphnode{

public:

graphnode(std::string name, std::string description = "");


bool Error() {return !valid;};
virtual std::string getname() {return name;}
// avoid duplicated graph names
static std::set<std::string> allgraphnames; 
std::string name;
std::string description;
bool valid; // not valid, if name duplicated

};

 

// param design, to support cross product of params
// <param item ID, value, (mainly that ID value pair expression, for key, a=1 etc. category=High) lexical expression


// One Param is actually the ID of one smallest level of result e.g.  win=3,  or mutliple param controlled result  win=3|qtl = 0.3
// Usually this One should be corresponding to < one column ~~ | another question would be we could label ~ result , it could be amend value in place as well? (leave it here)  ~ meanning that sometimes you can't check result to determine dependency, it might did sth not leading to an obvious output

// ParamSet is basically a set above configure values we want to go through in our calculation

// Calculator could use Such Param (S, many single param) in 2 ways    
// 1) Control params, each param control calc logic  2) use upstream calc results, which might be a series of Param corresponding 
// Then there will be multiple param cross (a=1 2;b = x y), or even more complicated, selected combinations --> to result

// 2) For next calc consuming param dependent outputs, we should allow 2 ways of supporting, (*) everything  (a=1,b=1; a=2,b=2), only use these2 for further calculations
// Anyway, this can lead to very complicated stuff


//More deeply, it determines how to organize the variable cache ~  all result somehow keyed, with param, lexically ??? how we make searcher easier

// Problem 1) How to design to support Params in sort/key with VariableCache   2) Non Param cases  3)  wild card search ~ can do later, but need to think about how to better support

// Current from Variable Key, it's hard to wild card selection, might need to have another structure to just store the easy to query correlation



typedef std::tuple<std::string, KFC, std::string> SingleParamItem;

class Param{
public:
    
    Param():items(std::map<std::string, SingleParamItem>()),status(true) {};
    Param(const Param& p):items(p.items),status(true){};
    Param(std::map<std::string, SingleParamItem> i):items(i),status(true) {};
    // simple bulder
    Param(SingleParamItem p1){
        items = std::map<std::string, SingleParamItem> ();
        items.insert(std::make_pair(std::get<0>(p1),p1));
        status = true;
    }
    // sorted, as long as items the same, order doesn't matter
    std::map<std::string, SingleParamItem>  items;
    bool status;
    // sorted by name,

    Param operator=(const Param& p){
        return Param(p);   
    }

    std::string getParamStr() const ;
    
    KFC getValue(std::string name) {return std::get<1>(items.at(name));}

    Param operator + (const Param& p1) const;

    // no param case
    bool Blank() const {return items.size() == 0;}  
    // 
    bool Error() const {return !status;}
};



// this set, what func we deliver

class ParamSets{
public:
    ParamSets():internals(std::vector<Param>()),status(true){};
    ParamSets(const ParamSets &Ps):internals(Ps.internals),status(true){};
    ParamSets(std::vector<Param> params):internals(params),status(true){};

    virtual std::vector<Param> getParams()const {return internals;};
    
    ParamSets operator*(const ParamSets &Ps);

    std::vector<Param> internals;
    bool status;
};

// complete cross product, if you want some specific rules, has to do it manually


// calculator and graph section
// CalcValueNode ~> only (return name + Params) uniquely define the item ,, calculator is for resolve dependency purpose
// We could even make it (CalcName + ReturnName + ParamsName) to identify one result

class simplecalculator;
// this is value nodes, it means one result, or  one column of results, or one set ofcolums, but all meanning the same thing, based on different configs (ParamSets)
// One calc node can generate multiple output nodes, 

// This part needs some more clarification
// As Input/Output you could easily say, generate node with multiple configures ( Vol5min, Vol10min)
// this Input/Output -> is bound to set of params

// but on the other handside, in Variable Cache, each single configure is set store separately
// so when store/search, the natural ways is (Calc;ValueName;Param(no set))
//If you have sets, we could search it for you one by one


// this is more at abstraction level, for calculator specify input/output
// on the other hand, it also need to support group search, mapping from valuenode to Variable (KFC) values to execute the actual calc
// likely we can hide, KFC and map in this way


typedef std::tuple<simplecalculator *, std::string, Param> CalcValueNode;


class CalcValueNodes
{
public:
CalcValueNodes():_impl(std::vector<CalcValueNode> ()){};
CalcValueNodes(std::vector<CalcValueNode> ns):_impl(ns){};

CalcValueNodes(const CalcValueNodes& n): _impl(n._impl){};

CalcValueNodes(simplecalculator * calc, std::string valname, Param p); // single param
CalcValueNodes(simplecalculator * calc, std::string valname): CalcValueNodes(calc,valname,Param() ) {}; // no param
CalcValueNodes(simplecalculator * calc, std::string valname, ParamSets &ps);

CalcValueNodes operator=(const CalcValueNodes& ns){return CalcValueNodes(ns);};

// this API is for calc handle each node separately
std::vector<CalcValueNode> nodes() {return _impl;} // suppose it's quite constnat

// let's make some room here, we might has those wild card matching situation
std::vector<CalcValueNode> expandnodes(KFCStore *variablecache){return _impl;}

// If calc node need inputs of CalcValueNode, it might explicityly specify what exactly it needs
std::vector<CalcValueNode> _impl;

};

namespace CalcValueNodeUtil{

// change this in the future, if you want to have some smart stuff
std::string getStoreKey(CalcValueNode n);

KFC getvalue(KFCStore *variablecache, const CalcValueNode &node);
bool setvalue(KFCStore *variablecache, const CalcValueNode &node, KFC &val);
};
// if there is no param dependent, ParamSet * could be set to 0
// As long as ParamSets* generate the same  getParamSeries(), they're the same params 


// Class calcparamrtnregistertable ~ helper for looking up

// ??  current task, level 
// level is used to help decide trigger hierarchy, infer current active "task"
// we will organize the current task into map<level, KFC) as an attribute, somehow it needs to fit into operator call API

// last part would be to set search key for value node; pay attention to the Empty case

// calculator dependency  1) node to node 2) variable to variable 
class simplecalculator:public graphnode{

public:
    simplecalculator(std::string name, CalculationLevel calclevel, ExecutorPhase phase, std::string description);

    bool checkdependency(KFCStore * variablecache);

    // Variable Cache need to be smart to support Runtime decided outputs
    // problem here is calculator is e.g. shared across all symbolsl, not just for '600000', in such sense, it can't in run time set what exact output is there for different symbols, 
    // let's go with strict way, output must be specified as CalcValueNodes
    // bool setoutputnode(CalcValueNodes);

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask) = 0;

    virtual KFC* generatechildtasks(KFCStore * variablecache,  std::map<CalculationLevel,KFC> curtask) = 0;

    virtual bool serializecalctrace() = 0;

    virtual std::string getname() { return std::string("name:") + name + std::string("@Calclevel:") + std::to_string(int(calclevel)) ;};

    bool setvariableincache(KFCStore * variablecache, std::string rtnname, Param p, KFC val);

    bool setvariablesincache(KFCStore * variablecache, std::string rtnname, ParamSets p, std::vector<KFC> val);
    
    bool setvariableincache(KFCStore * variablecache, CalcValueNode &n, KFC val);

    std::string getDescription(){return description;};

    virtual bool unittest(KFCStore * variablecache,MemoryManager *mgr){return false;}

    virtual bool istaskgenerator(){return false;};

    virtual bool registerupstream(simplecalculator* up){upstreams.push_back(up);};

    virtual CalcValueNode getoutputnode(std::string n, Param p){return CalcValueNode(this, n,p);};


    CalculationLevel calclevel;
    ExecutorPhase calcphase;
    std::vector<CalcValueNodes> inputvars;
    std::vector<CalcValueNodes> outputvars;
    std::set<simplecalculator *> dependencynode;
    // dependency using calculator
    std::vector<simplecalculator*> upstreams;

};

//all type of calculators 1) source node 2) task generator 3) calculator 4) dump node


class taskgenerator:public simplecalculator{
public:

        //no outputs into cache
    taskgenerator(std::string name, CalculationLevel calclevel, ExecutorPhase phase,  std::string description):simplecalculator(name, calclevel, phase, description){};


    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr,  std::map<CalculationLevel,KFC> curtask) {return false;};

    virtual bool istaskgenerator(){return true;};
};


class generalcalculator:public simplecalculator{
public:

    generalcalculator(std::string name, CalculationLevel calclevel, ExecutorPhase phase, std::string description):
    simplecalculator(name, calclevel, phase, description){};

    virtual KFC* generatechildtasks(KFCStore * variablecache,  std::map<CalculationLevel,KFC> curtask) {return NULL;};


};


// build or extend graph from nodes
// maybe graph to replace the function map in calculator, check depenency between cacls, resolve triggerring dependencies
// italso doesn't know about executor phase, preloop, loop, postloop, gpu etc. need that two

class graph{

public: 

    graph():nodes(std::map<CalculationLevel,std:: map<ExecutorPhase, std::vector<simplecalculator *>>> ()){};
    
    bool addnodes(simplecalculator * node, ExecutorPhase phase);

    std::map<CalculationLevel,std:: map<ExecutorPhase, std::vector<simplecalculator *>>> nodes;

    bool varifyValidity();

    bool toCalculationMap();

};



 

 
 #endif