#include "variablestore.h"


bool INSERTENV(ENV *t, std::string c, std::string a, int p, KFC val){
    if (t->find(c) == t->end())
        t->insert(std::make_pair(c,new std::map<std::string, std::map<int, KFC>*>()));
    auto mapc = t->at(c);
    
    if (mapc->find(a) == mapc->end())
        mapc->insert(std::make_pair(a, new std::map<int, KFC>()));
    auto mapa = mapc ->at(a);

    if (mapa->find(p) != mapa->end())
    {
        std::cout << "duplicated config" << std::endl;
        return false;
    }
    else{
        mapa->insert(std::make_pair(p,val));
        return true;
    }
}



KFC GETENV(ENV *t, std::string c, std::string a, int p, std::iostream& out){
    if (t->find(c)== t->end()){
        out << "config not found key " << c << std::endl;
        return EMPTYKFC;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        out << "config not found key " << a  << std::endl;
        return EMPTYKFC;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
        out << "config not found key" << p  << std::endl;
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
        std::cout << "duplicated config" << std::endl;
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
        std::cout << "duplicated config" << std::endl;
        return false;
    }
    else{
        mapa->insert(std::make_pair(p,funvariant{0,val}));
        return true;
    }
}



IterExecFunction GETENVFuncf1(ENVFunc *t, std::string c, std::string a, int p, std::iostream &out){
    if (t->find(c)== t->end()){
        out << "config not found key " << c << std::endl;
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        out << "config not found key " << a  << std::endl;
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
        out << "config not found key" << p  << std::endl;
        return EMPTYITER;
    }
    return mapa->at(p).f1;
}



GetChildTaskFunc GETENVFuncf2(ENVFunc *t, std::string c, std::string a, int p,std::iostream &out){
    if (t->find(c)== t->end()){
       out << "config not found key " << c << std::endl;
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        out << "config not found key " << a  << std::endl;
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    if (mapa->find(p) == mapa->end()){
        out << "config not found key" << p  << std::endl;
        return EMPTYITER;
    }
    return mapa->at(p).f2;
}


 std::map<int, IterExecFunction> * GETENVFuncMapf1(ENVFunc *t, std::string c, std::string a, std::iostream &out){
    if (t->find(c)== t->end()){
        out << "config not found key " << c << std::endl;
        return EMPTYITER;
    }
    auto mapc = t->at(c);
    if (mapc->find(a) == mapc->end()){
        out << "config not found key " << a  << std::endl;
        return EMPTYITER;
    }
    auto mapa = mapc->at(a);
    auto rtn = new  std::map<int, IterExecFunction>();
    for(auto iter:*mapa)
        rtn->insert(std::make_pair(iter.first, iter.second.f1));
    return rtn;
};




std::ostream& operator<<(std::ostream& os, const KFC& f)
{
    switch (f->t)
    {
    case KI:
        /* code */
        os << f->i;
        break;
    case KS:
        os << f->s;
        break;
    default:
        break;
    }
    return os;
};
