#include "memorymanager.h"


MemoManagerByThreads buildnmemorymgr(int n,std::map<CalculationLevel,std::map <MemoryLifeCyle, long>>  sizeconfig){
        MemoManagerByThreads rtn;
        for(int i=0;i<n;i++)
            rtn.insert(std::pair<int,MemoryManager*>(i,new MemoryManager(sizeconfig)));
        return rtn;
};


// simple cross-product configure
MemoManagerByThreads buildnmemorymgr(int nthread, std::vector<CalculationLevel> levels, std::vector<MemoryLifeCyle> cycles,long size){
    std::map<CalculationLevel,std::map <MemoryLifeCyle, long>> sizeconfig;
    for (CalculationLevel l: levels){
        std::map <MemoryLifeCyle, long> levelconfig;
        for (MemoryLifeCyle c: cycles)
            levelconfig.insert(std::make_pair(c,size));
        sizeconfig.insert(std::make_pair(l, levelconfig));
    }
    return buildnmemorymgr(nthread, sizeconfig);
};
