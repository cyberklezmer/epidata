#ifndef TOOLTOOLS_HPP
#define TOOLTOOLS_HPP

#include "orpp/csv.hpp"
#include <limits.h>
#include <time.h>
#include "districts.hpp"

using namespace std;
using namespace orpp;

constexpr int maxint = numeric_limits<int>::max();

enum cohorts { c0, c20, c65, c80, numcohorts};


cohorts v2cohort(unsigned v)
{
    if(v < 20)
        return c0;
    else
    {
        if(v<65)
            return c20;
        else
        {
            if(v < 80)
                return c65;
            else
            {
                if(v < 105)
                    return c80;
                else
                    return numcohorts;
            }
        }
    }
};


struct counter
{
    unsigned none = 0;
    unsigned wrong = 0;
    unsigned under = 0;
    unsigned over = 0;
};


int date2int(const string s)
{
    struct tm ti;
    ti.tm_sec=0;
    ti.tm_min=0;
    ti.tm_hour=0;
    ti.tm_mday= stoi(s.substr(8,2));
    ti.tm_mon = stoi(s.substr(5,2))-1;
    ti.tm_year=stoi(s.substr(0,4))-1900;

    time_t res = mktime(&ti);
    int di = round((res + 86400 *0.5) / 86400);
    return di;
}

string int2date(int date)
{
    time_t t = date * 86400;
    tm* ti = localtime(&t);
    unsigned y = ti->tm_year + 1900;
    unsigned m = ti->tm_mon+1;
    unsigned d = ti->tm_mday;

    ostringstream o;
    o << y << "-";
    if(m<10)
        o << "0";
    o << m << "-";
    if(d<10)
        o << "0";
    o << d;
    return o.str();
}

int date2int(const string s, int zerodate, int lastdate, counter& c)
{
    if(s=="")
    {
        c.none++;
        return maxint;
    }
    if(s.size() != 10)
    {
        c.wrong++;
        return maxint;
    }
    int di = date2int(s);
    if(di < zerodate)
    {
        c.under++;
        return maxint;
    }
    else if(di > lastdate)
    {
        c.over++;
        return maxint;
    }
    else
        return di;
}

int str2district(const string& okres_lau_kod)
{
   for(unsigned i=0; i<districts.size(); i++)
   {
       if(okres_lau_kod==districts[i].code)
           return i;
   }
   return maxint;
}

int orp2district(const string& orpname)
{
   for(unsigned i=0; i<orps.size(); i++)
   {
       if(orps[i].orp==orpname)
       {
           assert(districts[orps[i].distnum].name==orps[i].district);
           return orps[i].distnum;
       }
   }
   return maxint;
}


int zerodate = date2int("2020-02-24");

#endif // TOOLTOOLS_HPP
