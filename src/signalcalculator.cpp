#include "signalcalculator.h"


// Need to think about test memory, also the task context
// Linking to kdb nowls


  std::set<std::string>  graphnode::allgraphnames = std::set<std::string>();


graphnode::graphnode(std::string n, std::string d){
    name = n;
    description = d;
    valid = allgraphnames.find(n) == allgraphnames.end();
    if (! valid)
        std::string logerr =  "duplicated node names not allowed";
    else
        allgraphnames.insert(n);

}


std::string Param::getParamStr() const{
    std::string rtn = "Param[";
    for (auto p : items)
      rtn = rtn + std::get<2>(p.second);
    rtn = rtn + "]";
    return rtn;
}


Param Param::operator+ (const Param& P1) const{
    Param rtn(*this);
    rtn.status = rtn.status & P1.status;
    if (rtn.Error())
      return rtn;
    for(auto pi : P1.items){
      if(this->items.find(pi.first) != this->items.end()){
        LOG(ERROR,PARAM, "duplicated attributs during addition at " << std::get<2>(pi.second) << std::endl) ;
        rtn.status = false;
        return rtn;
      }
      rtn.items.insert(pi);
    }
    return rtn;
}

 ParamSets ParamSets::operator*(const ParamSets &Ps){
  std::vector<Param> rtn;
  bool status = true;
  if (this->getParams().size() == 0)
    return Ps;
  if (Ps.getParams().size() == 0)
    return *this;

  for (Param p1:this->getParams()){
    for (Param p2: Ps.getParams()){
      Param px = p1 + p2;
      if (px.Error())
        status = false;
      else
        rtn.push_back(px);
    }
  }  
  if (!status){
    ParamSets Prtn = ParamSets();
    Prtn.status = false;
    return Prtn;
  }else
    return ParamSets(rtn);
 }

std::string CalcValueNodeUtil::getStoreKey(CalcValueNode n){
  std::string namecalc = std::get<0>(n)->name;
  std::string params   = std::get<2>(n).getParamStr();
  std::string varname  = std::get<1>(n);
  // how to combine
  return std::string("<") + namecalc + "|" + params + "|" + varname + ">";
}


// for curent time being, let's simply implement it like a key map
KFC CalcValueNodeUtil::getvalue(KFCStore *variablecache, const CalcValueNode &node){
  auto key = getStoreKey(node);
  if ( !(variablecache->find(key)==variablecache->end())){
      return variablecache->at(key);
  }
  else{
      LOG(ERROR,CALCVALNODESEARCH, "failed to find from store " << key << std::endl);
      return EMPTYKFC; 
  }
}

bool CalcValueNodeUtil::setvalue(KFCStore *variablecache, const CalcValueNode &node, KFC &val){
  auto key = getStoreKey(node);
  if ( (variablecache->find(key)==variablecache->end())){
      variablecache->insert(std::make_pair(key,val));
      return true;
  }
  else{
      LOG(ERROR,CALCVALNODEINSERT, "value already exist from store " << key << std::endl);
      return false; 
  }
}


CalcValueNodes::CalcValueNodes(simplecalculator * calc, std::string valname, Param p){
  _impl = std::vector<CalcValueNode>();
  _impl.push_back(std::make_tuple(calc, valname, p));
}



CalcValueNodes::CalcValueNodes(simplecalculator * calc, std::string valname, ParamSets &ps){
  auto params = ps.getParams();
  if (params.size() == 0)
      _impl.push_back(std::make_tuple(calc, valname, Param()));
  else{
    for (auto pi : params)
      _impl.push_back(std::make_tuple(calc, valname, pi));
  }
}


simplecalculator::simplecalculator(std::string name, CalculationLevel level, ExecutorPhase phase, std::string description):graphnode(name,description),calclevel(level),calcphase(phase){  
  // this->setupstreamnodes
  
  this->inputvars = std::vector<CalcValueNodes>();
  this->outputvars = std::vector<CalcValueNodes>();
  this->upstreams =  std::vector<simplecalculator*> ();
}

bool simplecalculator::checkdependency(KFCStore * variablecache){
  bool status=true;
  for (auto nodes: inputvars){
    for (auto node: nodes.expandnodes(variablecache)){
        status = CalcValueNodeUtil::getvalue(variablecache, node) != EMPTYKFC;
        if(!status)
          break;
    }
    if (!status)
      break;
  }
  return status;
}

bool simplecalculator::setvariableincache(KFCStore * variablecache, std::string rtnname, Param p, KFC val){
  CalcValueNode n = std::make_tuple(this,rtnname,p);
  return CalcValueNodeUtil::setvalue(variablecache,n, val);
}


bool simplecalculator::setvariableincache(KFCStore * variablecache,CalcValueNode &n, KFC val){
  return CalcValueNodeUtil::setvalue(variablecache,n, val);
}


bool simplecalculator::setvariablesincache(KFCStore * variablecache, std::string rtnname, ParamSets p, std::vector<KFC> val){
  bool rtn = true;
  auto nodes = p.getParams();
  if (val.size() != nodes.size()){
    rtn = false;
    LOG(ERROR,SIMPLECALCULATORSETVARS, "nodes and values size not matching");
    return rtn;
  }
  for (int i = 0; i< val.size();i++)
    rtn &= setvariableincache( variablecache,  rtnname, nodes.at(i) , val.at(i));
  return rtn;
}


