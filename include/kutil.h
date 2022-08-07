#ifndef  TABLEVIEW
#define TABLEVIEW

#include "k.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "logging.h"
#include "memorymanager.h"
// translate kdb table





class tableview{

public:
std::map<std::string, void*> table;
std::map<std::string, int> tablemeta;
int length;

tableview(const tableview& t){
    table = std::map<std::string, void*> (t.table);
    tablemeta = std::map<std::string, int> (t.tablemeta);
    length = t.length;
};

// simple contructor from kdb object
tableview(const K &kt){
    table = std::map<std::string, void*>();
    tablemeta = std::map<std::string, int>();
    K colnames = kK(kt->k)[0];
    K cols = kK(kt->k)[1];
    length = kK(cols)[0]->n;
    LOGAndCOUT(INFO, BuildTableView, "table loaded with length" << length << std::endl);
    for (int coli=0;coli<colnames->n; coli++ ){
      std::string colstr = kS(colnames)[coli];
      table.insert(std::make_pair(colstr, (void *)kK(cols)[coli]->G0));
      tablemeta.insert(std::make_pair(colstr, int(kK(cols)[coli]->t)));
      // debug
      std::cout << "col " << colstr << "type " << int((kK(cols)[coli])->t) << std:: endl;
    }    
}
// create sub view, what if length of 0
tableview( tableview &  t, int startindex, int lensub){
    table = std::map<std::string, void*> ();
    tablemeta  =  std::map<std::string, int> (t.tablemeta);
    length = lensub;
    for (auto c: t.table){
      table.insert( std::make_pair(c.first, t.offsetcolptr(c.first, startindex)));
    }
};

// allow direct construction
tableview(std::map<std::string, void*> t, std::map<std::string, int> m,int l)
:table(t),tablemeta(m), length(l){};

// present

std::string tostring(){
   std::stringstream ss;
   ss << "table with length" << length << std::endl;
   ss << "table with column: " << std::endl ;
   for (auto cs: table) {
      ss << "col " << cs.first << " type: " << tablemeta.at(cs.first) <<  std::endl;
   }
   return ss.str();
};

K buildKTable(){
  return K(0);
}





// multi type printting
void printtable(std::ostream& o){
  for (int i =0; i<length;i++){
    o << "row " << i << ": " ;
    for (auto col: tablemeta){
      switch (col.second)
      {
      case KS:
        o <<  std::string(((S*) table.at(col.first))[i]);
        break;
      case KI:
          o << ((I*) table.at(col.first))[i];
        break;
      case KF:
          o << ((F*) table.at(col.first))[i];
        break;
      case KE:
          o << ((E*) table.at(col.first))[i];
        break;
      case KD:
          o << "2000.01.01 + "  << ((I*) table.at(col.first))[i];
        break;
      default:
          o << "UnkownVal";
        break;
      }
      o << " " ; 
    }
    o << std::endl;
  }
};


private:

void * offsetcolptr(std::string col, int offset );

};


// split table to sub views, e.g. symbols etc., 
std::vector<tableview> tosplitsortedtable(tableview t, std::string splitcol);


// extend base table with signal columns
K extendAxisTable(K &axis, std::map<std::string, int> extendcolmeta);


// eventually result will be a K Table, we will either set back to kdb q process, or store it as hdb

class KDestructor: public destroyer{
public:
  KDestructor(): _imp(0) {};
  KDestructor(K x) :_imp(x) {};
  K _imp;

  void setK(K x){
    _imp = x;
  };

   ~ KDestructor(){
    std::cout << _imp->r << "remove K, K number \n" ;
    if (_imp != 0)
      r0(_imp);
  };

};





#endif
