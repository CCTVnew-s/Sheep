#include "kdbnode.h"


std::string kdbsetup::DATESINRANGE = std::string("DATESINRANGE");
std::string kdbsetup::Description = "load hdb, get the dates of data we would like to process";



std::string tablequeryexecutor::bookdata = "bookdatatable";
std::string tablequeryexecutor::tradedata = "tradedatatable";
std::string tablequeryexecutor::orderdata = "orderdatatable";
std::string tablequeryexecutor::cxldata = "cxldatatable";

std::vector<tablequeryexecutor> dailyquerybuilder(RUNENV e){
    std::vector<tablequeryexecutor> rtn;
    if (e == RUNENV::h1){
        std::string book = "{[d] `symbol xasc select from book where date = d  }";
        std::string order= "{[d] `symbol xasc select from entrust where date = d}";
        std::string trans = "{[d] update BSFlag:?[BOrdID>SOrdID;1h;0b] from (`symbol xasc select from trade where date = d) where TradeTime within(09:29:59.000, 14:57:01)}" ;
        std::string cxl = "{[d] `symbol xasc select time:TransactTime, LocalTime, symbol: SecurityID, OrdID, Quantity, IsBid, SeqNo from cancelorder where date = d}";
        auto bookexecutor = tablequeryexecutor(book, tablequeryexecutor::bookdata);
        auto ordexecutor  = tablequeryexecutor(order, tablequeryexecutor::orderdata);
        auto tradexecutor = tablequeryexecutor(trans, tablequeryexecutor::tradedata);
        auto cxlexecutor  = tablequeryexecutor(cxl, tablequeryexecutor::cxldata);
        rtn.push_back(bookexecutor);
        rtn.push_back(ordexecutor);
        rtn.push_back(tradexecutor);
        rtn.push_back(cxlexecutor);
    }

    else{



    }
    return rtn;
}

std::string kdbdailydatamultithread::Description = "query data in asyced manners";


std::string kdbdailydatamultithread::DAILYDATAQUERIER= "DAILYDATAQUERIER";