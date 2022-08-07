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
        std::string book = "{[d] `symbol xasc (select date, SeqNo ,time: UpdateTime, LocalTime, symbol: SecurityID ,Status: InstruStatus, TotalTradeNum: TradNumber, TotalTradeVol: `long$TradVolume, AskPrice1,AskVolume1,AskPrice2,AskVolume2,AskPrice3,AskVolume3,AskPrice4,AskVolume4,AskPrice5,AskVolume5,AskPrice6,AskVolume6,AskPrice7,AskVolume7,AskPrice8,AskVolume8,AskPrice9,AskVolume9,AskPrice10,AskVolume10,BidPrice1,BidVolume1,BidPrice2,BidVolume2,BidPrice3,BidVolume3,BidPrice4,BidVolume4,BidPrice5,BidVolume5,BidPrice6,BidVolume6,BidPrice7,BidVolume7,BidPrice8,BidVolume8,BidPrice9,BidVolume9,BidPrice10,BidVolume10,NumOrdersB1,NumOrdersB2,NumOrdersB3,NumOrdersB4,NumOrdersB5,NumOrdersB6,NumOrdersB7,NumOrdersB8,NumOrdersB9,NumOrdersB10,NumOrdersS1,NumOrdersS2,NumOrdersS3,NumOrdersS4,NumOrdersS5,NumOrdersS6,NumOrdersS7,NumOrdersS8,NumOrdersS9,NumOrdersS10 from hdb_Snapshot_SS where date = d),\
          (select date, SeqNo ,time: UpdateTime, LocalTime, symbol: SecurityID ,Status: TradingPhaseCode, TotalTradeNum: TurnNum, TotalTradeVol: Volume, AskPrice1,AskVolume1,AskPrice2,AskVolume2,AskPrice3,AskVolume3,AskPrice4,AskVolume4,AskPrice5,AskVolume5,AskPrice6,AskVolume6,AskPrice7,AskVolume7,AskPrice8,AskVolume8,AskPrice9,AskVolume9,AskPrice10,AskVolume10,BidPrice1,BidVolume1,BidPrice2,BidVolume2,BidPrice3,BidVolume3,BidPrice4,BidVolume4,BidPrice5,BidVolume5,BidPrice6,BidVolume6,BidPrice7,BidVolume7,BidPrice8,BidVolume8,BidPrice9,BidVolume9,BidPrice10,BidVolume10,NumOrdersB1,NumOrdersB2,NumOrdersB3,NumOrdersB4,NumOrdersB5,NumOrdersB6,NumOrdersB7,NumOrdersB8,NumOrdersB9,NumOrdersB10,NumOrdersS1,NumOrdersS2,NumOrdersS3,NumOrdersS4,NumOrdersS5,NumOrdersS6,NumOrdersS7,NumOrdersS8,NumOrdersS9,NumOrdersS10 from hdb_Snapshot_SZ where date = d) }";

        std::string order = "{[d] `symbol xasc (select date, SeqNo, symbol: SecurityID, time:OrderTime, LocalTime, OrderID: OrderNO, BSFlag: OrderBSFlag , Price: OrderPrice, Quantity: Balance, Type: `Limit from hdb_Order_SS where date = d,OrderType = `A), \
          (select  date, SeqNo, symbol: SecurityID, time:TransactTime, LocalTime, OrderID: ApplSeqNum,  BSFlag: ?[Side=`$\"49\";`S;`B], Price:Price, Quantity: OrderQty, Type: OrdType from   hdb_Order_SZ  where date = d) }";

        std::string trans = "{[d] `symbol xasc (select date, SeqNo, time:TradTime, LocalTime, symbol: SecurityID, Price: TradPrice, Quantity:TradVolume, BOrderID: TradeBuyNo, SOrderID: TradeSellNo, BSFlag: TradeBSFlag from hdb_Transaction_SS where date = d), \
          (select date, SeqNo, time:TransactTime, LocalTime, symbol: SecurityID, Price: LastPx, Quantity:LastQty, BOrderID: BidApplSeqNum, SOrderID: OfferApplSeqNum, BSFlag: ?[TransactTime within(09:29:59.999, 14:59:59);?[BidApplSeqNum<OfferApplSeqNum;`S;`B];`N] from hdb_Transaction_SZ where date = d, ExecType = `$\"70\")  }";

        std::string cxl   = "{[d] `symbol xasc ( select date, SeqNo,  time:OrderTime, LocalTime, symbol: SecurityID, OrderID: OrderNO, Balance from hdb_Order_SS where date = d,OrderType = `D) , \
           (select date, SeqNo,  time:TransactTime, LocalTime, symbol: SecurityID, OrderID: ?[BidApplSeqNum=0;OfferApplSeqNum;BidApplSeqNum], Balance: LastQty from hdb_Transaction_SZ where date = d, ExecType = `$\"52\")}";

        auto bookexecutor = tablequeryexecutor(book, tablequeryexecutor::bookdata);
        auto ordexecutor  = tablequeryexecutor(order, tablequeryexecutor::orderdata);
        auto tradexecutor = tablequeryexecutor(trans, tablequeryexecutor::tradedata);
        auto cxlexecutor  = tablequeryexecutor(cxl, tablequeryexecutor::cxldata);
        rtn.push_back(bookexecutor);
        rtn.push_back(ordexecutor);
        rtn.push_back(tradexecutor);
        rtn.push_back(cxlexecutor);
    }

    return rtn;
}

std::string kdbdailydatamultithread::Description = "query data in asyced manners";


std::string kdbdailydatamultithread::DAILYDATAQUERIER= "DAILYDATAQUERIER";