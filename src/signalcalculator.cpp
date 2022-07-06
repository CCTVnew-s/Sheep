#include "signalcalculator.h"


 std::set<std::string>  graphnode::allgraphnames = std::set<std::string>();


graphnode::graphnode(std::vector<graphnode*> u,std::string n, std::string d){
    upstreamnode = u;
    name = n;
    description = d;
    valid = allgraphnames.find(n) == allgraphnames.end();
    if (! valid)
        std::string logerr =  "duplicated node names not allowed";
    else
        allgraphnames.insert(n);

}


