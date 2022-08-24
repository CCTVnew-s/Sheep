#ifndef CHILDTASKCALCULATOR
#define CHILDTASKCALCULATOR

#include "signalplatform.h"
#include "signalcalculator.h"
#include "kdbnode.h"
#include "tablecalcnodes.h"
#include <regex>
#include "ktypeoperator.h"


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
class symboltaskcalculator:public taskgenerator{
public:
    static std::string GETSYMBOLSOFDAYTASK;
    static std::string Description;

    symboltaskcalculator(splitsortedtable *d, std::string tgttable,symbolfilter* symbolfilter):taskgenerator(GETSYMBOLSOFDAYTASK,CalculationLevel::Date, ExecutorPhase::Preloop,std::vector<CalcValueNodes>(), Description),filter(symbolfilter),daysplitter(d)
    {
        splitinfo = std::make_tuple(daysplitter, splitsortedtable::keysplitinfo(tgttable), Param());
        inputvars.push_back(CalcValueNodes(std::vector<CalcValueNode>({splitinfo})));
    };


    virtual KFC* generatechildtasks(KFCStore * variablecache,  std::map<CalculationLevel,KFC> curtask){
        tableview* splitinfoT = CalcValueNodeUtil::getvalue(variablecache, splitinfo)->k;
        std::vector<S> rtnsyms;
        for(int i=0;i<splitinfoT->length;i++){
            if (filter->filter(((S*) splitinfoT->table.at(daysplitter->splitcol))[i])){
                rtnsyms.push_back(((S*) splitinfoT->table.at(daysplitter->splitcol))[i]);
                LOGAndCOUT(DEBUG,SYMBOLCHILD, " symbol is added " <<((S*) splitinfoT->table.at(daysplitter->splitcol))[i] << std::endl);
            }
        }
        KFC *rtn = new KFC[rtnsyms.size() + 1];
        for (int j=0;j<rtnsyms.size();j++)
            rtn[j] = buildKFC( rtnsyms.at(j) ,KS);
        rtn[rtnsyms.size()] = EMPTYKFC;
        return rtn;
    };


   virtual bool serializecalctrace(){
        return true;
    };

    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
       KFC dsplit = CalcValueNodeUtil::getvalue(variablecache, splitinfo);
       assert( 0!= dsplit);
       std::cout << dsplit;

       auto task0 = buildKFC(0,KD);
       std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Date, task0}};
       auto rtn = generatechildtasks(variablecache, curtask);
       std::cout << "following tasks being created" << std::endl;
       auto start = rtn;
       while ((*start)!=EMPTYKFC){
        std::cout << *start << std::endl;
        start++;
       }
        return true;
    };



    symbolfilter* filter;
    CalcValueNode splitinfo;
    splitsortedtable *daysplitter;
};























#endif