#ifndef  TABLEVIEW
#define TABLEVIEW

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "logging.h"
#include "memorymanager.h"
#include "k.h"
#include <cstring>
#include <stdio.h>
#include "arraycalc.h"
#include "extendarraycalc.h"
// translate kdb table


// try to use K for compound arrays

int ktypetosize(int tofk);



class tableview{

public:
std::map<std::string, void*> table;
std::map<std::string, int> tablemeta;
std::vector<std::string> colseq; // keep the sequence
int length;

tableview(const tableview& t){
    table = std::map<std::string, void*> (t.table);
    tablemeta = std::map<std::string, int> (t.tablemeta);
    length = t.length;
    colseq = t.colseq;
};

// simple contructor from kdb object
tableview(const K &kt){
    table = std::map<std::string, void*>();
    tablemeta = std::map<std::string, int>();
    colseq = std::vector<std::string>();
    K colnames = kK(kt->k)[0];
    K cols = kK(kt->k)[1];
    length = kK(cols)[0]->n;
    LOGAndCOUT(INFO, BuildTableView, "table loaded with length" << length << std::endl);
    for (int coli=0;coli<colnames->n; coli++ ){
      std::string colstr = kS(colnames)[coli];
      table.insert(std::make_pair(colstr, (void *)kK(cols)[coli]->G0));
      tablemeta.insert(std::make_pair(colstr, int(kK(cols)[coli]->t)));
      colseq.push_back(colstr);
      // debug
      std::cout << "col " << colstr << "type " << int((kK(cols)[coli])->t) << std:: endl;
    }    
}
// create sub view, what if length of 0
tableview( tableview &  t, int startindex, int lensub){
    table = std::map<std::string, void*> ();
    tablemeta  =  std::map<std::string, int> (t.tablemeta);
    colseq = std::vector<std::string>(t.colseq);
    length = lensub;
    for (auto c: t.table){
      table.insert( std::make_pair(c.first, t.offsetcolptr(c.first, startindex)));
    }
};


// copy sub view on new memory
tableview( tableview &  t, std::vector<std::string> selectedcol, int* loc, int len, MemoryManagerSingle* mem){
    table = std::map<std::string, void*> ();
    tablemeta  =  std::map<std::string, int> ();
    colseq = std::vector<std::string>();
    length = len;
    for (auto col:selectedcol){
        colseq.push_back(col);
        int coltype = t.tablemeta.at(col);
        int colsinglesize =  ktypetosize(coltype);
        tablemeta.insert(std::make_pair(col,coltype ));
        table.insert(std::make_pair(col,  mem->allocate( len *colsinglesize)));
        // need to copy by bytes
        char* dest = (char*) table.at(col);
        char* source = (char*) t.table.at(col);
        for(int j=0;j<len;j++)
          memcpy(dest+(j*colsinglesize), source + (loc[j]*colsinglesize) ,  colsinglesize);
    }
};


// allow direct construction
tableview(std::map<std::string, void*> t, std::map<std::string, int> m,int l, std::vector<std::string> cs)
:table(t),tablemeta(m), length(l), colseq(cs){};

// present

bool appendcol(std::string colname, int metatype, void* ptr){
  colseq.push_back(colname);
  tablemeta.insert(std::make_pair(colname, metatype));
  table.insert(std::make_pair(colname, ptr));
  return true;
};

//
bool appendcol(std::string colname, int metatype, MemoryManagerSingle* mem){
  int typesize = ktypetosize(metatype);
  void* colval = mem->allocate(typesize*length);
  return appendcol(colname,metatype,colval);
};


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
  K tableheader = ktn(KS,0);
  // append one by one
  K tablevalue = knk(0);
  for (int j=0;j<colseq.size();j++){
   auto p = colseq.at(j);
   char * cstr = new char [p.length()+1];
   std::strcpy (cstr, p.c_str());
   K copycol = ktn(tablemeta.at(p), (J)length);
   void *values = table.at(p);
   // memory copy , but need to get the size
   std::cout << "copying " << tablevalue->n << " th column " << p << "with address" << values << "length is "<< length << std::endl;
   std::cout << "copying to address" << (void *) copycol->G0 << std::endl;
   std::cout << "size of this column unit is " << ktypetosize(tablemeta.at(p)) << " for " << tablemeta.at(p) << std::endl;
   memcpy((void *)copycol->G0, values, length*ktypetosize(tablemeta.at(p)));
   
    jk(&tablevalue, copycol);
    js(& tableheader, (S) cstr);
    r1(copycol);
  }
  // table meta ~~ any special types need to follow K conditions
  return xT(xD(tableheader,tablevalue));
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



// This function is broken
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
