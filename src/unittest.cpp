#include "kutil.h"
#include "k.h"
#include <iostream>


#define KTIMEOUT 600000


int testtableview(I datahandle){
    K table = k(datahandle, "`symbol xasc select from book where date = last date",K(0));
    tableview t(table);
    auto tableseries = splitsortedtable(t,"symbol");
    std::cout << "we split by symbol created " << tableseries.size() << " tables";
    for (auto subt: tableseries){
        std::cout << "table with symbol" <<  ((char**) subt.table.at("symbol"))[0] << std::endl;
        std::cout << "mixed type col" << "BuyQueue" << "1000th element " << kI(((K*) subt.table.at("BuyQueue"))[1000])[0]  << std::endl;
    }

    K axis = k(datahandle, "select time, AskPrice1, BidPrice1 from book where date = last date, symbol = `000001 ", K(0));
    std::map<std::string, int> meta;

    // output result
    meta.insert(std::make_pair("Sigcol1",1));
    meta.insert(std::make_pair("Sigcol2",6));
    meta.insert(std::make_pair("Sigcol3",8));
    K axisex = extendAxisTable(axis, meta);
    tableview tk(axisex);
    ((I *)tk.table.at("Sigcol2"))[0] = 250;
    k(datahandle, "{ `testy set x}", axisex, K(0));


    return 1;
}






int main(){
    I testhandle = khpunc("localhost", 8939, "",KTIMEOUT, 1);
    testtableview(testhandle);

}