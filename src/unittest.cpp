#include "consts.h"
#include "kutil.h"
#include "k.h"
#include <iostream>
#include "testlist.h"
#include "kdbnode.h"
#include <ios>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <future>
#include "childtaskcalculator.h"
#include "tablecalcnodes.h"
#include "arraycalc.h"
#include "ktypeoperator.h"
#include "extendarraycalc.h"

int testtableview(I datahandle, RUNENV e){
    if (e == RUNENV::h1){
    k(datahandle, "\\l /datassd/qhdb",K(0));
    K table = k(datahandle, "`symbol xasc select from book where date = last date",K(0));
    tableview t(table);
    auto tableseries = tosplitsortedtable(t,"symbol");
    std::cout << "we split by symbol created " << tableseries.size() << " tables";
    for (auto subt: tableseries){
        std::cout << "table with symbol" <<  ((char**) subt.table.at("symbol"))[0] << std::endl;
        std::cout << "mixed type col" << "BuyQueue" << "1000th element " << kI(((K*) subt.table.at("BuyQueue"))[1000])[0]  << std::endl;
    }

    K axis = k(datahandle, "select time, AskPrice1, BidPrice1 from book where date = last date, symbol = `000001 ", K(0));
    std::map<std::string, int> meta;

    MemoryManagerSingle memtest(1024*1024*100);

  
    tableview test1(axis);
    test1.appendcol("Sigcol1",1,&memtest);
    test1.appendcol("Sigcol2",6,&memtest);
    test1.appendcol("Sigcol3",8,&memtest);
    // add from here    
    ((I *)test1.table.at("Sigcol2"))[0] = 2130;
    ((E *)test1.table.at("Sigcol3"))[10] = 100050.0;

    k(datahandle, "{ testy2 :: x}", test1.buildKTable(), K(0));



    std::vector<std::string> subcol = {"time", "AskPrice1", "Sigcol2"};
    int loc[5] = {0,100,200,300,400};
    tableview t2(test1,subcol,loc,5,&memtest);
    std::cout << t2.tostring();
    K newtable2 = t2.buildKTable();
    k(datahandle, "{ `testy3 set x}", newtable2, K(0));


    }
    
    else{
    K table = k(datahandle, "`SecurityID xasc select from hdb_Snapshot_SZ where date = last date, SecurityID in `600000`000001`000002",K(0));
    tableview t(table);
    auto tableseries = tosplitsortedtable(t,"SecurityID");
    std::cout << "we split by SecurityID created " << tableseries.size() << " tables";
    for (auto subt: tableseries){
        std::cout << "table with SecurityID" <<  ((char**) subt.table.at("SecurityID"))[0] << std::endl;
        std::cout << "simple type col" << "NumOrdersS9" << "1000th element " << ((I*) subt.table.at("NumOrdersS9"))[1000]  << std::endl;
    }

    K axis = k(datahandle, "select LocalTime, AskPrice1, BidPrice1 from hdb_Snapshot_SZ where date = last date, SecurityID = `000001 ", K(0));
    std::map<std::string, int> meta;

    // output result
    meta.insert(std::make_pair("Sigcol1",1));
    meta.insert(std::make_pair("Sigcol2",6));
    meta.insert(std::make_pair("Sigcol3",8));
    tableview test1(axis);
    
    K axisex = extendAxisTable(axis, meta);
    tableview tk(axisex);
    ((I *)tk.table.at("Sigcol2"))[0] = 2130;
    ((E *)tk.table.at("Sigcol3"))[10] = 100050.0;
    

    K newtable = tk.buildKTable();
    
    k(datahandle, "{ `testy2 set x}", newtable, K(0));
    
    k(datahandle, "{ `testy set x}", axisex, K(0));
    
    }







    return 1;
}



void mem_usage(double& vm_usage, double& resident_set) {
   vm_usage = 0.0;
   resident_set = 0.0;
   std::ifstream stat_stream("/proc/self/stat",std::ios_base::in); //get info from proc directory
   //create some variables to get info
   std::string pid, comm, state, ppid, pgrp, session, tty_nr;
   std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   std::string utime, stime, cutime, cstime, priority, nice;
   std::string O, itrealvalue, starttime;
   unsigned long vsize;
   long rss;
   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
   >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
   >> utime >> stime >> cutime >> cstime >> priority >> nice
   >> O >> itrealvalue >> starttime >> vsize >> rss; // don't careabout the rest
   stat_stream.close();
   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configuredto use 2MB pages
   vm_usage = vsize / 1024.0;
   resident_set = rss * page_size_kb;
};


bool getsleep(){
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return true;
}



int testcalculator(I handle, RUNENV e, MemoryManager *m){

   // std::cout << "start to test future" << std::endl;
   //  std::future<bool> f= std::async(getsleep);
   //  std::cout << "get future" << f.get() << std::endl;



    KFCStore cachefortest;
    I sd = e == RUNENV::h1 ? ymd(2021,1,1): ymd(2021,8,1);
    I ed = e == RUNENV::h1 ? ymd(2021,2,1): ymd(2021,9,1);
    generalcalculator *g1 = new kdbsetup(e,handle,sd,ed); 
    g1->unittest(&cachefortest, m);
    g1->serializecalctrace();
    
    businessdatecalculator *ch1 = new businessdatecalculator(g1);
    
    // test querier
    //auto qrys = dailyquerybuilder(h1);
    // KDestructor * kdes = new KDestructor[qrys.size()];
    // int i = 0;
    // for (auto q:qrys){
    //   auto x = q.execquery(handle,ymd(2021,2,1));
    //   std::cout << "current ref" << x->r << "\n";
    //   auto y = tableview(x);
    //   kdes[i].setK(x);
    //   i++;
    // }
    // //
    // double vm_usage, resident_set;
    //  mem_usage(vm_usage, resident_set) ;
    // std::cout << "vm_usage " << vm_usage << " | rsident set" << resident_set << "\n";
    // delete [] kdes;
    // std::cout << "vm_usage " << vm_usage << " | rsident set" << resident_set << "\n";

    // test date query calculator
    auto g2 = new kdbdailydatamultithread(e, handle,dailyquerybuilder(e), g1, 3);
    g2->unittest(&cachefortest, m);


    auto c1 = new businessdatecalculator(g1);
    c1->unittest(&cachefortest, m);

    splitsortedtable* splitday = new splitsortedtable(g2 , g2->tablenames, "symbol");
    splitday->unittest(&cachefortest, m);;

    symbolfilter *filter = new symbolfilter("(00)(.*)");
    auto c2 = new symboltaskcalculator(splitday, tablequeryexecutor::bookdata,filter);
    c2->unittest(&cachefortest, m);


    splittablesubview* subtable = new splittablesubview(g2, splitday, g2->tablenames, splitday->splitcol, splitsortedtable::SPLITSTARTINDEX,splitsortedtable::SPLITENDINDEX);
    subtable->unittest(&cachefortest, m);

    
    return 1;
}



bool testarraycalculator(){
    auto where = ARRAY::Where();
    std::cout << "we're testing array calc" << std::endl;
    bool x[5] = {true,false,true,false,true};
    int  y[5] = {};
    auto rtn = where( ARRAY::buildvar(y,5) ,ARRAY::buildvar(x,5));

    MemoryManagerSingle mgr(1024*1024*1024);
    mgr.DEBUG = true;
    MemoryManagerSingle draftmgr(1024*1024*1024);
    draftmgr.DEBUG = true;


    std::cout << ARRAY::buildvar(x,5).l<< " size";
    auto rtn2 = where.evaluatenotype(ARRAY::buildvar(x,5),&mgr);
    ARRAY::var<int> rtn3(rtn2);

    for(int i=0;i<rtn3.l;i++)
        std::cout << rtn3[i] << std::endl;

    // test operand
    ARRAY::calcstack testcacl = ARRAY::calcstack(&where) + ARRAY::buildvar(x,5);    
    std::cout << testcacl._lists.size() <<  std::endl;
    auto proxy = testcacl.evaluate(&draftmgr,&mgr);
    ARRAY::var<int> rtn4(proxy );
    for(int i=0;i<rtn4.l;i++)
        std::cout << rtn4[i] << std::endl;
    std::cout << "size of return unit " << proxy.unitsize << std::endl;
}

int ktoounittest(RUNENV e){
    // testarraycalculator();
     I testhandle = khpunc("localhost", e==RUNENV::h1?8939:9002, "",60000, 1);    
    testtableview(testhandle, e);

    return 1;
}