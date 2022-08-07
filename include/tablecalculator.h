#ifndef TABLECALCULATOR
#define TABLECALCULATOR

#include "signalcalculator.h"
#include "kdbnode.h"

class presortedtablesplitteer:public generalcalculator
{
public:
static std::string PRESORTTABLESPLITTEER;
static std::string Description;

    presortedtablesplitteer(kdbdailydatamultithread *tablegenerator, std::string splitcol)
    :generalcalculator(PRESORTTABLESPLITTEER, CalculationLevel::Date, ExecutorPhase::Preloop,  std::vector<CalcValueNodes>(),std::vector<CalcValueNodes>(), Description)
    {
        

    }






}













#endif