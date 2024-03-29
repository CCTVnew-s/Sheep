#include "variablestore.h"
#include "logging.h"







bool INSERTENV(ENV *t, std::string c, std::string a, int p, KFC val){
    if (t->find(c) == t->end())
        t->insert(std::make_pair(c,new std::map<std::string, std::map<int, KFC>*>()));
    auto mapc = t->at(c);
    
    if (mapc->find(a) == mapc->end())
        mapc->insert(std::make_pair(a, new std::map<int, KFC>()));
    auto mapa = mapc ->at(a);

    if (mapa->find(p) != mapa->end())
    {
        LOGAndCOUT(ERROR,InsertEnv, "duplicated config" << std::endl);
        return false;
    }
    else{
        mapa->insert(std::make_pair(p,val));
        return true;
    }
}



KFC GETENV(ENV *t, std::string c, std::string a, int p){
    if (t->find(c)== t->end()){
        LOGAndCOUT(ERROR,GetEnv, "config not found key " << c << std::endl);
        return EMPTYKFC;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
         LOGAndCOUT(ERROR,GetEnv, "config not found key " << a  << std::endl);
        return EMPTYKFC;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
         LOGAndCOUT(ERROR,GetEnv, "config not found key" << p  << std::endl);
        return EMPTYKFC;
    }
    return mapa->at(p);

}




bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, IterExecFunction val){
    if (t->find(c) == t->end())
        t->insert(std::make_pair(c,new std::map<std::string, std::map<int, funvariant>*>()));
    auto mapc = t->at(c);

    if (mapc->find(a) == mapc->end())
        mapc->insert(std::make_pair(a, new std::map<int, funvariant>()));
    auto mapa = mapc ->at(a);

    if (mapa->find(p) != mapa->end())
    {
        LOGAndCOUT(ERROR,InsertIterFuncEnv , "duplicated config" << std::endl);
        return false;
    }
    else{
        mapa->insert(std::make_pair(p,funvariant{val,0}));
        return true;
    }
}

bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, GetChildTaskFunc val){
    if (t->find(c) == t->end())
        t->insert(std::make_pair(c,new std::map<std::string, std::map<int, funvariant>*>()));
    auto mapc = t->at(c);

    if (mapc->find(a) == mapc->end())
        mapc->insert(std::make_pair(a, new std::map<int, funvariant>()));
    auto mapa = mapc ->at(a);

    if (mapa->find(p) != mapa->end())
    {
        LOGAndCOUT(ERROR,InsertTaskFuncEnv ,  "duplicated config" << std::endl);
        return false;
    }
    else{
        mapa->insert(std::make_pair(p,funvariant{0,val}));
        return true;
    }
}



IterExecFunction GETENVFuncf1(ENVFunc *t, std::string c, std::string a, int p){
    if (t->find(c)== t->end()){
        LOGAndCOUT(ERROR,GetIterFuncEnv, "config not found key " << c << std::endl);
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        LOGAndCOUT(ERROR,GetIterFuncEnv, "config not found key " << a  << std::endl);
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
        LOGAndCOUT(ERROR,GetIterFuncEnv, "config not found key" << p  << std::endl);
        return EMPTYITER;
    }
    return mapa->at(p).f1;
}



GetChildTaskFunc GETENVFuncf2(ENVFunc *t, std::string c, std::string a, int p){ 
    if (t->find(c)== t->end()){
      LOGAndCOUT(ERROR,GetTaskFuncEnv, "config not found key " << c << std::endl);
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
      LOGAndCOUT(ERROR,GetTaskFuncEnv , "config not found key " << a  << std::endl);
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
    LOGAndCOUT(ERROR,GetTaskFuncEnv , "config not found key" << p  << std::endl);
        return EMPTYITER;
    }
    return mapa->at(p).f2;
}


 std::map<int, IterExecFunction> * GETENVFuncMapf1(ENVFunc *t, std::string c, std::string a){
    if (t->find(c)== t->end()){
        LOGAndCOUT(ERROR,GetIterFuncEnv, "config not found key " << c << std::endl);
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        LOGAndCOUT(ERROR,GetIterFuncEnv, "config not found key " << a  << std::endl);
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    auto rtn = new  std::map<int, IterExecFunction>();
    for(auto iter:*mapa)
        rtn->insert(std::make_pair(iter.first, iter.second.f1));
    return rtn;
};

// KFC lexical 
std::ostream& operator<<(std::ostream& os, const KFC& f)
{
    switch (f->t)
    {
    case KI:{
        /* code */
        os << f->i;
        break;}
    case KS:{
        os << std::string(f->s);
        break;}
    case KD:{
        os << "Date 2000.01.01 + " << f->i;
        break;}
    case - KD:{
        os << "Dates array L " << f->n << "\n";
        for (int i=0;i<f->n;i++){
            os << "Date 2000.01.01 + " << ((I*)f->G0)[i] << "\n";
        }
        break;}
    case - KI:{
        os << "Dates array L " << f->n << "\n";
        for (int i=0;i<f->n;i++){
            os <<  ((I*)f->G0)[i] << " \n";
        }
        break;
        }
    case KTABLEVIEW:{
        os << f->k->tostring();
        break;
    }

    // other arrays to be filled
    default:
        os << "sth can't be present yet";
    }
    return os;
};



#define KCFBUILDFACTORY(type,typeloc)        \
KFC buildKFC(type val, int id) {         \
    KFC rtn = new kfc();                     \  
    rtn->t = id;                         \
    rtn->typeloc = val;                      \
    return rtn;}                             \

KCFBUILDFACTORY(I,i);
KCFBUILDFACTORY(S,s);
KCFBUILDFACTORY(H,h);
KCFBUILDFACTORY(J,j);
KCFBUILDFACTORY(E,e);
KCFBUILDFACTORY(F,f);
KCFBUILDFACTORY(void*,p);
KCFBUILDFACTORY(tableview *,k);
