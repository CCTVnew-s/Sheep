#ifndef MEMORYMANAGER
#define MEMORYMANAGER

#include <map>
#include <vector>

#include "consts.h"


class MemoryManager;
typedef std::map<int, MemoryManager*> MemoManagerByThreads;

class MemoryManagerSingle{
public:

    MemoryManagerSingle( long s){
        mem = malloc(s * sizeof(char));
        cursor = mem;
        sizebytes = s;
        used = 0;
        overfillmem = std::vector<void *> ();
    }

    void reset(){
        used = 0;
        cursor = mem;
        while(overfillmem.size()>0){
            free(overfillmem.back());
            overfillmem.pop_back();
        }
    }


    void * allocate(long size){
        if (sizebytes - used > size){
            void *memtmp = malloc(size* sizeof(char));
            overfillmem.push_back(memtmp);
            return memtmp;
        }
        else{
            used = used + size;
            void *rtn = cursor;
            cursor = (void *)((char *)cursor + size);
            return rtn;
        }
    }

    // issue is that, we need to update every pointer, who refer here
    void resize(long long s){
        if(sizebytes > 0 && sizebytes < s){
            ; // can't be supported for now
        }
        else if( sizebytes > 0 && sizebytes >= s) {
            return;
        }
        else{
            mem = malloc(s);
            sizebytes = s;
            used = 0;
        }
    }

private:
void *mem;
void *cursor;
long sizebytes;
long used;
std::vector<void *> overfillmem;

};


class MemoryManager{
public:
    std::map<std::pair<CalculationLevel, MemoryLifeCyle>, MemoryManagerSingle*> childmgrs;

    MemoryManager(std::map<std::pair<CalculationLevel, MemoryLifeCyle>, long> sizeconfig){
        childmgrs = std::map<std::pair<CalculationLevel, MemoryLifeCyle>, MemoryManagerSingle*>();
        for (auto mconfig: sizeconfig)
            childmgrs.insert(std::make_pair(mconfig.first, new  MemoryManagerSingle(mconfig.second)));
    }

    MemoryManagerSingle* getChildMgr(CalculationLevel l, MemoryLifeCyle c){
        if (childmgrs.find(std::make_pair(l,c)) == childmgrs.end())
            return NULL;
        else
            return childmgrs.at(std::make_pair(l,c));
    }
    

};


MemoManagerByThreads buildnmemorymgr(int n, std::map<std::pair<CalculationLevel, MemoryLifeCyle>, long> sizeconfig){
        MemoManagerByThreads rtn;
        for(int i=0;i<n;i++)
            rtn.insert(std::pair<int,MemoryManager*>(i,new MemoryManager(sizeconfig)));
        return rtn;
};







#endif