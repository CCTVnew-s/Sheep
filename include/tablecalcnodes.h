#ifndef TABLECALCNODES
#define TABLECALCNODES

#include "signalcalculator.h"
#include "memorymanager.h"
#include "tools.h"
#include <assert.h>
#include <regex>
#include "arraycalc.h"
#include "ktypeoperator.h"
using ARRAY::varx; // this should be defaul, since our vec operator core is this varx definition





// BELOW 3 CLASSES are for AXIS generation
// before splitting we will need to first generate AXIS/Observation table on Date level
// then this table will also need to be split ~ 
// also each signal or any calcouputs, need to be stored here

// Centralized the outputs ~~ what
// we are doing here?? abstraction, multiple to one ~ Node to Pointer ~ 
// or say, we're creating special table(tableview), its construction needs provide extra column mapping info
// mean while it will keep this extra mapping info, when it's used as inputs for other calculation, need the previous mapping info to get column
// don't make it static, since it can be resused


// need specify what outputs sigs, value, type, whether is K object
// mapping from calcnode to return column name

// need some global exposure, this board is equal to the calc table itself
class OutputBoard{
public:
    static std::string MISSING;
    OutputBoard(std::string name):tablename(name),outputmaping(std::map<CalcValueNode*,std::tuple<std::string, int>>()),sequence(std::map<int, CalcValueNode*> ()){};

    bool registernode(CalcValueNode *n,  std::string col, int metacol){ // regiester for table, col, name 
        if (outputmaping.find(n)!= outputmaping.end()){
            return false;
        }
        else{
            outputmaping.insert(std::make_pair(n, std::make_tuple(col,metacol)));
            sequence.insert(std::make_pair(sequence.size(), n));
            return true;
        }
    };
    // find the column name
    std::string getColMatching(CalcValueNode *n){
        if (outputmaping.find(n)==outputmaping.end())
            return MISSING;
        else
            return std::get<0>(outputmaping.at(n));
    };
    // find the col metatype
    int getColMatchingType(CalcValueNode *n){
        if (outputmaping.find(n)==outputmaping.end())
            return -999;
        else
            return std::get<1>(outputmaping.at(n));
    };

    std::string tablename;
    std::map<CalcValueNode*,std::tuple<std::string, int>> outputmaping;   // table, col, Ktype'
    // keep some build consequence, map sequence is too random
    std::map<int, CalcValueNode*> sequence;

};


// we want to fix the output here, every new output modify it here
class GlobalOutputBoards{
public:
    static std::string SymO;
    static std::string AxisO;
    static std::map<std::string,OutputBoard*> boards;

    static bool initiate(){
        boards.insert(std::make_pair(SymO, new OutputBoard(SymO)));
        boards.insert(std::make_pair(AxisO, new OutputBoard(AxisO)));
    }; // 

    static OutputBoard* getOutput(std::string name){
        if (boards.find(name) == boards.end())
            return nullptr;
        else
            return boards.at(name);
    };
    
};



class Calnodeextendtable{
public:
    Calnodeextendtable(tableview* tx,OutputBoard* boardx):table(tx),board(boardx){};

    // build the table at loop run time
    bool buildtable(MemoryManagerSingle* mem){
        for(auto colid: board->sequence){
            auto colname = board->getColMatching(colid.second);
            auto coltype = board->getColMatchingType(colid.second);
            if (colname == OutputBoard::MISSING){
                // handle wrong col
                continue;
            }
            void *colptr = mem->allocate(ktypetosize(coltype)*table->length);
            table->appendcol(colname, coltype, colptr);
        }
        return true;
    };

    std::string getColMatching(CalcValueNode *n){
        return board->getColMatching(n);
    };

    tableview* table;    
    OutputBoard* board;
};



// AXIS determined by filter functions, F on table row; also need to reserve space for final outputs ,e g. signalbook1, singalautoion2, singlaliquidity etc., such bridge should be done by a class
// current filter structure : 1. symbol -> regex filter 2. time within interval
class AxisFilters:public ARRAY::vecopr_2 {
public:

    static std::string AXISTIMECOL;
    static std::string AXISSYMCOL;
    static std::vector<std::pair<I,I>> DEFAULTINTERVAL;

    AxisFilters(symbolfilter *f1, std::vector<std::pair<I,I>> intvls=DEFAULTINTERVAL):fs(f1),tmintvls(intvls){}; 
    // shall we do it line by row,,, can answer it here
    ARRAY::var<int> operator()(ARRAY::var<int> rtn, ARRAY::var<S> sym, ARRAY::var<I> time){
        int nextloc = 0;
        for(int i=0;i<sym.l;i++)
            if( fs->filter(sym[i])  && timefilter(time[i])){
                rtn[nextloc] = i;
                nextloc++;
            }
        return rtn;
    };

    static const bool rtnsingle=false;    // these 2 determines the return size
    EXTENDOPERATOR(2) // normal extension, froced

    bool timefilter(I& x){ 
        for (auto intvalx:tmintvls)
        {
            if ((intvalx.first<=x) && (intvalx.second>=x))
               return true; 
            else
                continue;
        }
        return false;
       };

    ARRAY::varx filtertable(tableview* book, MemoryManagerSingle* mem){
        ARRAY::var<int> time((I*)book->table.at(AXISTIMECOL),book->length);
        ARRAY::var<S> sym((S*)book->table.at(AXISSYMCOL),book->length);
        return evaluatenotype(time,sym,mem); // mmg will take care of the memory, though we have a bit of waste
    };

    symbolfilter* fs;
    std::vector<std::pair<I,I>> tmintvls;

};



class dailyoutputtables: public generalcalculator{
public:
    static std::string FINALOUTPUTTABLE; // 
    static std::string Description; // axis and reserved space for calculated signals
    

    dailyoutputtables(generalcalculator* dailydata, std::string bookdata, std::vector<std::string> copythroughcols, AxisFilters* axisfilter)  //((char*)cursor) - ((char*)mem)
    :generalcalculator(FINALOUTPUTTABLE,CalculationLevel::Date, ExecutorPhase::Preloop, std::vector<CalcValueNodes>(),std::vector<CalcValueNodes>(), Description){
    // how to create     
    

        
    };
  
   // some filters
    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask){
  
    }
    
    CalcValueNode input;    
};

class splitsortedtable: public generalcalculator
{
public:
static std::string DATETABLESPLITTER;
static std::string Description;
static std::string SPLITSTARTINDEX;
static std::string SPLITENDINDEX;
static std::string SPLITINFO; // this is the real output 

public:

    static std::vector<std::pair<std::string, Param>> enrichemptyparam(std::vector<std::string> tablenames){
         std::vector<std::pair<std::string, Param>> rtn;
         for (auto name : tablenames)
            rtn.push_back(std::make_pair(name, Param()));
        return rtn;
    };

    static std::string keysplitinfo(std::string tablename){
        return tablename + "_" + SPLITINFO;
    };


    splitsortedtable(generalcalculator* tablegenerator, std::vector<std::pair<std::string, Param>> tableids,std::string s)
    :generalcalculator(DATETABLESPLITTER,CalculationLevel::Date, ExecutorPhase::Preloop, std::vector<CalcValueNodes>(),std::vector<CalcValueNodes>(), Description),splitcol(s), 
    tablenodes(std::vector<CalcValueNode>()), splittablenodes(   std::vector<CalcValueNode>()){
      for (auto t: tableids)
      {
        CalcValueNode nt = std::make_tuple(tablegenerator, t.first , t.second);
        tablenodes.push_back(nt); 
        CalcValueNode splitnt = std::make_tuple(this, keysplitinfo(t.first) , t.second);
        splittablenodes.push_back(splitnt);
      }
      inputvars.push_back(CalcValueNodes(tablenodes));
      outputvars.push_back(CalcValueNodes(splittablenodes));
    };

    splitsortedtable(generalcalculator* tablegenerator, std::vector<std::string> tablenames,std::string splitcol)
    :splitsortedtable(tablegenerator,enrichemptyparam(tablenames),splitcol)
    {    
    };

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask){
        MemoryManagerSingle * mgrcur = mgr.at(MemoryLifeCyle::LoopTask);        
        for (auto tnode: tablenodes){
            auto start = std::chrono::high_resolution_clock::now();
            simpletimer( std::get<1>(tnode) + std::string(" table splitting"));
            tableview *tpointer =  (CalcValueNodeUtil::getvalue(variablecache, tnode))->k;
            tableview *splitinfo = generatesplittable(tpointer, mgrcur, splitcol);
            this->setvariableincache(variablecache, keysplitinfo(std::get<1>(tnode)), Param() ,buildKFC(splitinfo, KTABLEVIEW));
            LOGAndCOUT(DEBUG,SPLITTABLE,"table split is done for " << std::get<1>(tnode) << std::endl);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::nanoseconds elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            LOGAndCOUT(INFO,TIMER, "" << std::get<1>(tnode) + std::string("table splitting action has take ")<< elapsed.count() << " nano seconds" << std::endl);
            mgrcur->addtoclean(new simpledestroyer<tableview>(splitinfo));
        }
        return true;
    };

    static tableview* generatesplittable(tableview *tbl, MemoryManagerSingle *mgr, std::string splitcol){
        // count symbols
        void *splitcolarray = tbl->table.at(splitcol);
        
        int symbolcount = 1;
        int cursor = 1;
        
        while(cursor<tbl->length){
            bool newtable = false;
        // override for KS
           //  assert(tbl->tablemeta.at(splitcol) == KS);
            newtable = !( ((char**)splitcolarray)[cursor] ==  ((char**)splitcolarray)[cursor - 1]   );
            if (newtable)
                symbolcount ++ ;
            cursor ++;
        }
        // symbol count we need to allocate array
        S* symcol =  new S[symbolcount]; // (S*)  mgr->allocate( symbolcount * sizeof(S));
        int *startindexcol = new int[symbolcount]; // (int*) mgr->allocate( symbolcount * sizeof(int));
        int* endindexcol = new int[symbolcount]; //  (int*) mgr->allocate( symbolcount * sizeof(int));
        
        cursor = 1;
        int currentablestart=0;
        int curtblidx = 0;
        while(cursor<tbl->length){
            bool newtable = false;
        // override for KS
          //  assert(tbl->tablemeta.at(splitcol) == KS);
            newtable = !( ((char**)splitcolarray)[cursor] ==  ((char**)splitcolarray)[cursor - 1]   );
            if (newtable){
                symcol[curtblidx] = ((char**)splitcolarray)[cursor - 1];
                startindexcol[curtblidx] = currentablestart;
                endindexcol[curtblidx] = cursor;
                currentablestart = cursor;
                curtblidx++;
            }
            cursor ++;
        }

        symcol[curtblidx] = ((char**)splitcolarray)[cursor - 1];
        startindexcol[curtblidx] = currentablestart;
        endindexcol[curtblidx] = cursor;
        




        std::map<std::string, void *> tablecols = {{splitcol, (void *)symcol},\
           {std::string(SPLITSTARTINDEX),(void *)startindexcol}, {std::string(SPLITENDINDEX),(void *)endindexcol} };
        std::map<std::string, int> tablemeta = {{splitcol,KS}, {std::string(SPLITSTARTINDEX),KI}, {std::string(SPLITENDINDEX),KI}};

        return new tableview(tablecols, tablemeta, symbolcount, std::vector<std::string>({splitcol,std::string(SPLITSTARTINDEX),std::string(SPLITENDINDEX)}));
    };



    virtual bool serializecalctrace(){return true;};


    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
    
       KFC ds = CalcValueNodeUtil::getvalue(variablecache, tablenodes.at(0));
       assert( 0!= ds);

       LOGAndCOUT(DEBUG,TABLESPLIT, "we get this first table view:" << ds);

       KFCStore context;
       auto task0 = buildKFC(0,KD);
       std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Date, task0}};
       calculate(variablecache, &context, *(mgr->getMemoryManagerSet(CalculationLevel::Date)), curtask);      

       auto splitinfo = CalcValueNodeUtil::getvalue(variablecache, std::make_tuple(this,keysplitinfo(std::get<1>(tablenodes.at(0))), Param()));
       LOGAndCOUT(DEBUG,TABLESPLIT, "we get split tables results for first table" << splitinfo);
       // we need to print out, to be very confident
      // splitinfo->k->printtable(std::cout);


    };


    std::vector<CalcValueNode> tablenodes;
    std::vector<CalcValueNode> splittablenodes;
    generalcalculator* tablegenerator;
    std::string splitcol;
};

// at each symbol level, trying to generate sub views from task SYMBOL 000001 e.g.
class splittablesubview:public generalcalculator{
public:

static std::string SUBTABLEFROMSPLIT;
static std::string Description; 
static std::string SUBATSYMBOL;

    static std::string keysubviewtable(std::string tablename){
        return tablename + "_" + SUBATSYMBOL;
    };


    // suppose table has no Param
    splittablesubview(generalcalculator* originaltablegen, generalcalculator* splitter, std::vector<std::string> tablenames, std::string splitcolname, std::string startindex, std::string endindex)
    :generalcalculator(SUBTABLEFROMSPLIT,CalculationLevel::Symbol, ExecutorPhase::Preloop, std::vector<CalcValueNodes>(),std::vector<CalcValueNodes>(), Description),tables(tablenames),
    splitcol(splitcolname),startindexcol(startindex), endindexcol(endindex)
    {
        for(auto t:tablenames){
            CalcValueNode origt = std::make_tuple(originaltablegen, t , Param());
            CalcValueNode splitinfot = std::make_tuple(splitter, splitsortedtable::keysplitinfo(t), Param());
            splitinfo.push_back(splitinfot);
            originaltable.push_back(origt);
        }
        inputvars.push_back(CalcValueNodes(splitinfo));
        inputvars.push_back(CalcValueNodes(originaltable));
    };

    virtual bool calculate(KFCStore * variablecache, KFCStore *taskcontext, MemoryManagerSet &mgr, std::map<CalculationLevel,KFC> curtask){
        S cursym = curtask.at(CalculationLevel::Symbol)->s;

        MemoryManagerSingle * mgrcur = mgr.at(MemoryLifeCyle::LoopTask);        
        for(int i=0;i<splitinfo.size();i++){
            assert(  (CalcValueNodeUtil::getvalue(variablecache, originaltable.at(i))) != NULL );
            assert(  (CalcValueNodeUtil::getvalue(variablecache, splitinfo.at(i))) != NULL );

            
            tableview *to =  (CalcValueNodeUtil::getvalue(variablecache, originaltable.at(i)))->k;
            tableview *ts =  (CalcValueNodeUtil::getvalue(variablecache, splitinfo.at(i)))->k;
            
            tableview *subtable = getsubview(to, ts, cursym, tables.at(i));
            this->setvariableincache(variablecache, keysubviewtable(tables.at(i)), Param() ,buildKFC(subtable, KTABLEVIEW));
            mgrcur->addtoclean(new simpledestroyer<tableview>(subtable));
        }
        return true;
    };

    tableview* getsubview(tableview *origt, tableview *splitinfo, S sym, std::string tablename){
        // strict string equal
        S* lookupcol = (S*) splitinfo->table.at(splitcol);
        int loc=0;
        for(;loc<splitinfo->length;loc++)
            if (std::string(sym) == std::string(lookupcol[loc]))
                break;
        if(loc==splitinfo->length){
            LOG(DEBUG,GETSUBTABLEBYSYMBOL,"can't find " << sym << " in table " << tablename << "return 0 length table" << std::endl);
            return new tableview(*origt,0,0);
        }
        else{
            int start = ((int *) splitinfo->table.at(startindexcol))[loc];
            int endnotinclude = ((int *) splitinfo->table.at(endindexcol))[loc];
            LOG(DEBUG,GETSUBTABLEBYSYMBOL,"find " << sym << " in table " << tablename << "from "<< start << " to  " << endnotinclude << std::endl);
            return new tableview(*origt,start,endnotinclude - start);
        }
    }

    virtual bool serializecalctrace(){return true;};



    virtual bool unittest(KFCStore * variablecache, MemoryManager *mgr){
       KFC tbl1 = CalcValueNodeUtil::getvalue(variablecache, splitinfo.at(0));
       assert( 0!= tbl1);
       KFC tbl2 = CalcValueNodeUtil::getvalue(variablecache, originaltable.at(0));
       assert( 0!= tbl2);
       KFCStore context;
       auto task0d = buildKFC(0,KD);
       auto task0s = buildKFC("noexist",KS);
       std::map<CalculationLevel,KFC> curtask = {{CalculationLevel::Date, task0d},{CalculationLevel::Symbol,task0s}};

       std::cout << "start testing" << std::endl;

       
       //calculate(variablecache, &context, *(mgr->getMemoryManagerSet(CalculationLevel::Symbol)), curtask);
       // test not exist
       //KFC tblsub0 = CalcValueNodeUtil::getvalue(variablecache,std::make_tuple(this,keysubviewtable(tables.at(0)), Param()));
       //assert(tblsub0->k->length == 0);

       
       auto task1s = buildKFC("600000",KS);
       curtask = {{CalculationLevel::Date, task0d},{CalculationLevel::Symbol,task1s}};
       calculate(variablecache, &context, *(mgr->getMemoryManagerSet(CalculationLevel::Symbol)), curtask);
       KFC tblsub1 = CalcValueNodeUtil::getvalue(variablecache,std::make_tuple(this,keysubviewtable(tables.at(0)), Param()));
       // tblsub1->k->printtable(std::cout);
    }

    std::vector<CalcValueNode> splitinfo;
    std::vector<CalcValueNode>  originaltable;
    std::vector<std::string> tables;
    std::string splitcol;
    std::string startindexcol;
    std::string endindexcol;
};






#endif