#ifndef MEMORYMANAGER
#define MEMORYMANAGER

#include <map>
#include <vector>

#include "consts.h"
#include "logging.h"

class MemoryManager;
typedef std::map<int, MemoryManager*> MemoManagerByThreads;



// provide access to reserved access
// manage crossing phases function

class destroyer{
public:
virtual ~destroyer(){};
};

template <typename T>
class simpledestroyer: public destroyer{
public:
    simpledestroyer(T* &px):p(px){};
    ~simpledestroyer(){
        std::cout << "delete sth from there" << std::endl;
        delete p;
    };
T* p;
};

template <typename T>
class simplearraydestroyer: public destroyer{
public:
    simplearraydestroyer(T* &px):p(px){};
    ~simplearraydestroyer(){
        std::cout << "delete sth from there" << std::endl;
        delete [] p;
    };
T* p;
};



class MemoryManagerSingle{
public:

    MemoryManagerSingle( long s){
        mem = malloc(s * sizeof(char));
        cursor = mem;
        sizebytes = s;
        used = 0;
        overfillmem = std::vector<void *> ();
        tobecleaned = std::vector<destroyer*> ();
    }

    void reset(){
        used = 0;
        cursor = mem;
        while(overfillmem.size()>0){
            free(overfillmem.back());
            overfillmem.pop_back();
        };

        while(tobecleaned.size()>0){
            delete tobecleaned.back();
            tobecleaned.pop_back();
        };
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

    void addtoclean(destroyer *d){
        tobecleaned.push_back(d);
    }

private:
void *mem;
void *cursor;
long sizebytes;
long used;
std::vector<void *> overfillmem;
std::vector<destroyer*> tobecleaned;

};


typedef std::map<MemoryLifeCyle ,MemoryManagerSingle *> MemoryManagerSet;


class MemoryManager{
public:
    std::map<CalculationLevel, MemoryManagerSet> childmgrs;

    MemoryManager(std::map<CalculationLevel,std::map <MemoryLifeCyle, long>> sizeconfig){
        childmgrs = std::map<CalculationLevel, MemoryManagerSet>();
        for (auto levelconfig: sizeconfig){
            MemoryManagerSet levelmgr;
            for (auto cycleconfig: levelconfig.second)
                levelmgr.insert(std::make_pair(cycleconfig.first, new  MemoryManagerSingle(cycleconfig.second)));
            childmgrs.insert(std::make_pair(levelconfig.first, levelmgr));
        }
    }

    MemoryManagerSet* getMemoryManagerSet(CalculationLevel l){
        if (childmgrs.find(l) == childmgrs.end())
        {
            LOG(ERROR,MEMOMGR, "failed to find the mem manger set for level " << int(l) << std::endl);
            return NULL;
        }
        else
            return &childmgrs.at(l);
    }

    bool resetMemoryManager(CalculationLevel l, MemoryLifeCyle c){
        if (childmgrs.find(l) == childmgrs.end())
        {
            LOG(ERROR,MEMOMGR, "failed to find the mem manger set for level " << int(l) << std::endl);
            return false;
        }
        else if (childmgrs.at(l).find(c) == childmgrs.at(l).end())
        {
            LOG(ERROR,MEMOMGR, "failed to find the mem manger set for level " << int(l) << "at cyle"<< int(c) << std::endl);
            return false;
        }
        else{
            childmgrs.at(l).at(c)->reset();
            return true;
        }

    }

};


MemoManagerByThreads buildnmemorymgr(int n,std::map<CalculationLevel,std::map <MemoryLifeCyle, long>>  sizeconfig);


// simple cross-product configure
MemoManagerByThreads buildnmemorymgr(int nthread, std::vector<CalculationLevel> levels, std::vector<MemoryLifeCyle> cycles,long size);




#endif