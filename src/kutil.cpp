#include "kutil.h"
#include <iostream>
#include <cstring>
// borrow the types from kq, shouldnt do table or dict, suppose


#define KBpair 1 , char  
#define UUpair 2 , U 
#define KGpair 4 , char
#define KHpair 5 , short 
#define KIpair 6 , int   
#define KJpair 7 , long 
#define KEpair 8 , float
#define KFpair 9 , double 
#define KCpair 10, char
#define KSpair 11, char* 

#define KPpair 12, long
#define KMpair 13, int 
#define KDpair 14, int

#define KNpair 16, long
#define KUpair 17, int
#define KVpair 18, int
#define KTpair 19, int

#define KZpair 15, double


#define EXPAND_FUN_OVER_KTYPES(fun, ...) \
      fun(KBpair, __VA_ARGS__) \
      fun(KGpair, __VA_ARGS__) \
      fun(KHpair, __VA_ARGS__) \
      fun(KIpair, __VA_ARGS__) \
      fun(KJpair, __VA_ARGS__) \
      fun(KEpair, __VA_ARGS__) \
      fun(KFpair, __VA_ARGS__) \
      fun(KCpair, __VA_ARGS__) \
      fun(KSpair, __VA_ARGS__) \
      fun(KPpair, __VA_ARGS__) \
      fun(KMpair, __VA_ARGS__) \
      fun(KDpair, __VA_ARGS__) \
      fun(KNpair, __VA_ARGS__) \
      fun(KUpair, __VA_ARGS__) \
      fun(KVpair, __VA_ARGS__) \
      fun(KTpair, __VA_ARGS__)


// support any simple type generic functions

#define KCONVERT(typeid, type, x, offset) case typeid: x = (void *)((type *)x  + offset); break;
#define KCONVEREXPAND(KID,x,offset)  KCONVERT(KID, x, offset)





void * tableview:: offsetcolptr(std::string col, int offset ){
    void *rtn = table.at(col);
    switch (tablemeta.at(col))
    {
        EXPAND_FUN_OVER_KTYPES(KCONVEREXPAND, rtn, offset)
    default:
        LOGAndCOUT(CRITICAL,OffsetTableView, "Error, no matchable type info is find for col" << col << "swtiched to mixed type" <<  std::endl);
        rtn = (void *)((K *)rtn  + offset);
        break;
    }
    return rtn;
};

#define EQUALPREVIOUS(typeid, type, x, index ,rtnbool) case typeid: rtnbool = !( ((type*) x)[index] == ((type*) x)[index - 1] )       ; break;
#define EQUALPREVIOUSKID(KID,x, index ,rtnbool) EQUALPREVIOUS(KID,x, index ,rtnbool)


std::vector<tableview> splitsortedtable(tableview t, std::string splitcol){
    int cursor = 1;
    int currenttablestart = 0;
    void *splitcolarray = t.table.at(splitcol);

    std::vector<tableview> rtn;
    while(cursor<t.length){
        bool newtable = false;
        // override for KS
        if (t.tablemeta.at(splitcol) == KS){
            newtable = !( std::string(((char**)splitcolarray)[cursor]) ==  std::string(((char**)splitcolarray)[cursor - 1])   );
        }
        else{
            switch (t.tablemeta.at(splitcol))
            {
             EXPAND_FUN_OVER_KTYPES(EQUALPREVIOUSKID, splitcolarray, cursor,  newtable) 
            default:
                // can't handle mixed type like list of lists
                LOGAndCOUT(ERROR,TALEVIEWSPLITSORTTABLE, "Debug, no matchable type info is find for col" << splitcol << std::endl);
                break;
            }
        }

        if(newtable){
            rtn.push_back(tableview(t,currenttablestart, cursor - currenttablestart));
            currenttablestart = cursor;
            cursor = cursor + 1;
        }
        else{
            cursor = cursor + 1;
        }
    }

    // last table;
    if (cursor != currenttablestart)
        rtn.push_back(tableview(t,currenttablestart, cursor - currenttablestart));
    
    return rtn;
}



K extendAxisTable(K &axis, std::map<std::string, int> extendcolmeta){
    K tablecol = kK(axis->k)[0];
    K tableval = kK(axis->k)[1];
    J length = kK(tableval)[0]->n;
    for(auto col: extendcolmeta){
        char * cstr = new char [col.first.length()+1];
        std::strcpy (cstr, col.first.c_str());
        tablecol = js(& tablecol, (S)cstr);
        tableval = jk(& tableval, ktn(col.second, length));

        //std::cout << "table col  # " << tablecol->n << std::endl <<  col.first.c_str();
        // std::cout << "table col  # " << tableval->n << std::endl;
    }
    return xT(xD(tablecol, tableval));
}



