#ifndef KDBNODE
#define KDBNODE


#include "consts.h"
#include "variablestore.h"
#include "logging.h"
#include "k.h"
#include "kutil.h"
#include "signalcalculator.h"
#include <vector>
#include <string>
#include <list>
#include <future>
#include <assert.h>
#include <thread>

// starts with simple caclulator

// (std::string name, CalculationLevel calclevel, ExecutorPhase phase, std::vector<CalcValueNodes> inputvars, std::vector<CalcValueNodes> outputvars, std::string description):

class kdbsetup:public generalcalculator
{
public:
static std::string DATESINRANGE;
static std::string Description;

    kdbsetup(RUNENV e, I handle, I sd, I ed):generalcalculator("KDBSetup",CalculationLevel::Top, ExecutorPhase::Preloop , Description),
    handle(handle),e(e),sd(sd),ed(ed){
        datesinrange = getoutputnode(DATESINRANGE,Param());
        // this->outputvars.push_back(datesinrange);
        
        if (e == RUNENV::h1)
            query = "{[x;y] exec date from select distinct date from book where date within (x,y)}";
        else
            query = "{[x;y] exec date from select distinct date from hdb_Snapshot_SS where date within (x,y)}";
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
            std::cout << "unit test for set up kdb \ns";
            std::cout << "we have set " << CalcValueNodeUtil::getStoreKey(datesinrange) << "with value \n";
            std::cout << CalcValueNodeUtil::getvalue(variablecache, datesinrange);
        }
        return status;
    }

I handle;
RUNENV e;
I sd,ed;
std::string query;
CalcValueNode datesinrange;
};



struct GlbSrcTable{

static std::string bookdata;
static std::string tradedata;
static std::string orderdata;
static std::string cxldata;
};





class tablequeryexecutor{
public:
    std::string query;
    std::string outputname;

    // should all be tableview
    tablequeryexecutor(std::string query1, std::string outputname1):query(query1), outputname(outputname1){};
    K execquery(I handle, I date){
        return k(handle, (S) query.c_str() ,  kd(date), K(0) );
    };

};

// centralize writing it here
std::vector<tablequeryexecutor> dailyquerybuilder(RUNENV e);

// a bit complicated, we need this to be asynchronized executing daily data query
// meanwhile, it also needs to help the destropy of the project
// specify inputs/outputs
// also outputs need to be configurable by the tables we like to get 


// unify date presentation

class kdbdailydatamultithread:public generalcalculator
{
public:
static std::string DAILYDATAQUERIER;
static std::string Description;

   kdbdailydatamultithread(RUNENV e,  I handle, std::vector<tablequeryexecutor> tbls, kdbsetup *kdbsetupcalc, int cachesize)\
   :generalcalculator(DAILYDATAQUERIER,CalculationLevel::Date, ExecutorPhase::Preloop, Description),handle(handle),cachesize(cachesize),tablequries(tbls)
   {
        // This upstream register is mandatory
        registerupstream(kdbsetupcalc);
        datesinrange = kdbsetupcalc->datesinrange;

        // Label here, whether we need this input registeration
        inputvars.push_back(CalcValueNodes(std::vector<CalcValueNode>({datesinrange})));
        
        // some outputs we register
        for (auto q:tablequries){

            CalcValueNode tablenode = std::make_tuple(this, q.outputname, Param());
            tablenodes.push_back(tablenode);
            tablenames.push_back(q.outputname);
            output.insert(std::make_pair(q.outputname,tablenode ));
        }
        cachedutildate = 0;
        cachethreadstart = false;
        cacheddata = std::list<std::pair<I, std::promise<std::vector<K>> *>>();

        // 
   };

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask){
        // start the maintain process
         KFC dates = CalcValueNodeUtil::getvalue(variablecache, datesinrange);

        std::thread* tcache;        
        if (!cachethreadstart){
            tcache = new std::thread (&kdbdailydatamultithread::maintaincache, this,dates);
            cachethreadstart = true;
        }

        // std::cout <<"checking here" << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // blocked thread for today's query result
        int waitcachesize = 0;
        while (waitcachesize==0){
            mtx.lock();
            waitcachesize = cacheddata.size();
            mtx.unlock();
            if (waitcachesize>0)
                break;
            else
                std::cout << "waiting for some creation of caching list" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        auto data = cacheddata.front();
        assert(data.first == curtask.at(CalculationLevel::Date)->i);
        auto vals = data.second->get_future().get();
        LOGAndCOUT(DEBUG,KDBDAILY,"requested data is avaialable @" << curtask.at(CalculationLevel::Date)  << std::endl);
        // convert to Table View
        assert(vals.size() == tablequries.size());
        for (int i=0;i<vals.size();i++)
        {
            K ti = vals.at(i);
            tableview *ti_v = new tableview(ti);  // here is a sth created in KFC, but it holds sth you need to describe it by your self
            this->setvariableincache(variablecache, output.at(tablequries.at(i).outputname)   ,buildKFC(ti_v, KTABLEVIEW) );
            LOGAndCOUT(DEBUG,KDBDAILY,"set table  "<< tablequries.at(i).outputname << "into cache" << std::endl );
            // also need to register the destroy ~ need to centrally manage those "new things" that we need to live across functions

            mgr.at(MemoryLifeCyle::LoopTask)->addtoclean(new KDestructor(ti)); // delete after whole loop task
            mgr.at(MemoryLifeCyle::LoopTask)->addtoclean(new simpledestroyer< tableview>(ti_v));
        }
        // pop from list 
        popfirstpromise();
        return true;
    };

    // just start separate thread to query result, up to certain number
    // how to control caching using single thread ~ simply just cache for one more day  --- No, do it properly
    // can use to promise to get future , set each day's promise sequentially on the single query thread, such that
    // main process can access one by one
    // maybe easy to just 

    bool maintaincache(KFC datesinrange){
        assert(datesinrange->t == -KD);
        int curretloc = 0;
        // let's just start from datesinrange first eletement
        LOG(DEBUG,KDBDAILY, "start to caching data");
        while(curretloc<datesinrange->n){
            if (cacheddata.size() < cachesize){
                cachedutildate = kI(datesinrange)[curretloc];
                LOGAndCOUT(DEBUG,KDBDAILY,"trying to set promise for " << cachedutildate << std::endl);
                // set in the cacheddata
                mtx.lock();
                auto insertpair = std::make_pair(cachedutildate,new std::promise<std::vector<K>>());
                cacheddata.push_back(insertpair);
                mtx.unlock();
                // now we  set value
                auto val = executequeries(cachedutildate);
                insertpair.second->set_value(val);
                LOGAndCOUT(DEBUG,KDBDAILY,"querying and set complete at " << cachedutildate << std::endl);
                curretloc = curretloc + 1;
            }
            else{
                LOG(DEBUG,KDBDAILY, "cached enought data, sleep ");
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            }
        }
    };

    bool popfirstpromise(){
        mtx.lock();
        auto firstele = cacheddata.front();
        int d = firstele.first;
        delete firstele.second;
        cacheddata.pop_front();
        mtx.unlock();
        LOG(DEBUG,KDBDAILY,"remove first element of " << d << std::endl);
    };


    // 
    std::vector<K> executequeries( I qrydate){
        std::vector<K> rtn;
        for (auto q : tablequries){
            rtn.push_back(q.execquery(handle, qrydate));
            std::cout << q.query << "at " << qrydate << std::endl;
        }
        return rtn;
    };

    virtual bool serializecalctrace(){
       return true;  
    };


    // need business date to be set first
    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
       KFC ds = CalcValueNodeUtil::getvalue(variablecache, datesinrange);
       assert( 0!= ds);

      KFCStore context;
      auto task0 = buildKFC(kI(ds)[0],KD);
      std::cout << "current task " <<kI(ds)[0] << " " <<task0<<std::endl;

      std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Date, task0}};
      calculate(variablecache, &context, *(mgr->getMemoryManagerSet(CalculationLevel::Date)), curtask);      
      // pretend calculating sth
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      mtx.lock(); // this is waiting 
      std::cout << "after processing or lastest caching task " << cachedutildate << std::endl;
      std::cout << "current querying task size" << cacheddata.size() << std::endl;
      std::cout << "current first querying task on the list" << cacheddata.front().first << std::endl;
      mtx.unlock();

    };



CalcValueNode datesinrange;
I handle;
int cachesize;

std::vector<tablequeryexecutor> tablequries;
std::vector<CalcValueNode> tablenodes;
std::vector<std::string> tablenames;

std::map<std::string, CalcValueNode> output; // output can be sourced by original name

std::list<std::pair<I, std::promise<std::vector<K>> *>> cacheddata;
std::mutex mtx; // loc for cache data vector
I cachedutildate;
bool cachethreadstart;


};


#endif