#include "ktypeoperator.h"
#include "k.h"

I timetype(int h, int m, int s,int mili){
    return ((((h*60)+m)*60)+s)*1000 + mili;
};

I datetype(int y, int m, int d){
    return ymd(y,m,d);
};
