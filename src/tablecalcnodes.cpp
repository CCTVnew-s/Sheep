#include "tablecalcnodes.h"
#include "businessconst.h"




std::string OutputBoard::MISSING = "OUTPUTCOLMISSING";
std::string GlobalOutputBoards::SymO = "SigOutputBySymbol";
std::string GlobalOutputBoards::AxisO = "SigOutputByAxis";
 std::map<std::string,OutputBoard*> GlobalOutputBoards::boards =  std::map<std::string,OutputBoard*>();


std::string AxisFilters::AXISSYMCOL = "symbol";
std::string AxisFilters::AXISTIMECOL = "time";
std::vector<std::pair<I,I>> AxisFilters::DEFAULTINTERVAL = {std::make_pair(EXCH::AMCTSTART - 500, EXCH::AMCTEND + 500), std::make_pair(EXCH::PMCTOPEN, EXCH::PMCTEND)};


std::string dailyoutputtables::FINALOUTPUTTABLE ="FINALSIGNALOUTPUTNODE";
std::string dailyoutputtables::Description = "Each signal register the signal column in the board, here the calc generate the table with space waiting for filling";




std::string splitsortedtable::DATETABLESPLITTER = "DATETABLESPLITTER";
std::string splitsortedtable::Description = "split the daily data tables by stock ";
std::string splitsortedtable::SPLITSTARTINDEX = "SPLITSTARTINDEX";
std::string splitsortedtable::SPLITENDINDEX = "SPLITENDINDEX";
std::string splitsortedtable::SPLITINFO = "SPLITINFOTABLE";


std::string splittablesubview::SUBTABLEFROMSPLIT = "SUBTABLEFROMSPLIT";
std::string splittablesubview::SUBATSYMBOL = "SUBVIEWATSYMBOL";

std::string splittablesubview::Description = "Trying to get the sub table of certain symbol";