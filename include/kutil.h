#ifndef  TABLEVIEW
#define TABLEVIEW

#include "k.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "logging.h"
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
      //std::cout << "col " << colstr << "type " << int((kK(cols)[coli])->t) << std:: endl;
    }    
}
// create sub view
tableview(tableview t, int startindex, int length){
    table = std::map<std::string, void*> ();
    tablemeta  =  std::map<std::string, int> (t.tablemeta);
    length = t.length;
    for (auto c: t.table){
      table.insert( std::make_pair(c.first, t.offsetcolptr(c.first, startindex)));
    }
};


private:

void * offsetcolptr(std::string col, int offset );

};


// split table to sub views, e.g. symbols etc., 
std::vector<tableview> splitsortedtable(tableview t, std::string splitcol);


// extend base table with signal columns
K extendAxisTable(K &axis, std::map<std::string, int> extendcolmeta);


// eventually result will be a K Table, we will either set back to kdb q process, or store it as hdb




#endif
