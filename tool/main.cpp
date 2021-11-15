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

void mzcr2mzcr(const string& horizon,  const string& start16)
{
    int s16 = date2int(start16);
    int lastdate = date2int(horizon);
    int numdates = lastdate - zerodate + 1;

    counter ocounter;
    unsigned inconsistento = 0;


    vector<vector<unsigned>> I(numdates,vector<unsigned>(numcohorts,0));
    vector<vector<unsigned>> R(numdates,vector<unsigned>(numcohorts,0));

    csv<','> osoby("/home/martin/Documents/s/covid/data/mzcr/osoby.csv");

    cout << "Importing osoby" << endl;

    for(unsigned i=1; i<osoby.r(); i++)
    {
        enum {datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod, nakaza_v_zahranici,nakaza_zeme_csu_kod};

        string ds = osoby(i,datum);
        int d = date2int(ds, zerodate,lastdate, ocounter);

        if(d == maxint)
            inconsistento++;
        else
        {
            const unsigned noage = 1000;
            unsigned v = noage;
            try
            {
                v = osoby.getunsigned(i,vek);
            }
            catch(...)
            {
                clog << "Bad age " << osoby(i,vek) << endl;
                inconsistento++;
            }
            if(v != noage)
            {
                cohorts c = v2cohort(v);

                if(c==numcohorts)
                    inconsistento++;
                else
                {
                    R[d-zerodate][c]++;
                    if(osoby(i,nakaza_v_zahranici)=="1")
                        I[d-zerodate][c]++;
                }
            }
        }
    }
    cout << inconsistento << " inconsistent records: "
         << ocounter.wrong << " wrong dates,"
         << ocounter.over << " dates over,"
         << ocounter.under << " dates under" << endl;

    counter ucounter;
    unsigned inconsistentu = 0;
    vector<vector<unsigned>> D(numdates,vector<unsigned>(numcohorts,0));

    csv<','> umrti("/home/martin/Documents/s/covid/data/mzcr/umrti.csv");

    cout << "Importing umrti" << endl;

    for(unsigned i=1; i<umrti.r(); i++)
    {
        enum labels {datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod};

        string ds = umrti(i,datum);
        int d = date2int(ds, zerodate,lastdate, ucounter);

        if(d == maxint)
            inconsistentu++;
        else
        {
            unsigned v = umrti.getunsigned(i,vek);
            cohorts c = v2cohort(v);
            if(c==numcohorts)
                inconsistentu++;
            else
                D[d-zerodate][c]++;
        }
    }
    cout << inconsistentu << " inconsistent records: "
         << ucounter.wrong << " wrong dates,"
         << ucounter.over << " dates over,"
         << ucounter.under << " dates under" << endl;

    counter occounter;
    unsigned inconsistentoc = 0;
    vector<vector<unsigned>> O(numdates,vector<unsigned>(numcohorts * 2,0));

    csv<','> ockovani("/home/martin/Documents/s/covid/data/mzcr/ockovani.csv");

    cout << "Importing ockovani" << endl;

    for(unsigned i=1; i<ockovani.r(); i++)
    {
        enum labels {datum,vakcina,kraj_nuts_kod,kraj_nazev,vekova_skupina,
                     prvnich_davek,druhych_davek,celkem_davek};

        string ds = ockovani(i,datum);
        int d = date2int(ds, zerodate,lastdate, occounter);

        if(d == maxint)
            inconsistentoc++;
        else
        {
            string cs = ockovani(i,vekova_skupina);
            string ls;
            unsigned age;
            for(unsigned j=0; j<cs.size() && cs[j] != '-' && cs[j] != '+'; j++)
                ls += cs[j];
            try
            {
                age = stoul(ls);
            }
            catch (...)
            {
                clog << "Cannot convert'" << ls << "' to unsigned" << endl;
                inconsistentoc++;
                continue;
            }

            cohorts c;
            if(age >= 80)
                c = c80;
            else if(age >= 65)
                c = c65;
            else if(age >= 20)
                c = c20;
            else if(age == 18)
            {
                if(d <= s16)
                    c = c20;
                else
                {
                    static unsigned dice = 18;
                    if(dice < 20)
                        c = c0;
                    else
                        c = c20;
                    if(++dice == 25)
                        dice = 18;
                }
            }
            else
            {
                assert(age == 0);
                c = c0;
            }
            unsigned first = ockovani.getunsigned(i,prvnich_davek);
            unsigned secondadj = ockovani.getunsigned(i,druhych_davek);
            if(ockovani(i,vakcina)=="COVID-19 Vaccine Janssen")
                secondadj += first;
            O[d-zerodate][c] += first;
            O[d-zerodate][c+numcohorts] += secondadj;
        }
    }
    cout << inconsistentoc << " inconsistent records: "
         << occounter.wrong << " wrong dates,"
         << occounter.over << " dates over,"
         << occounter.under << " dates under" << endl;

    string mname("mzcr.csv");

    ofstream out(sys::outputfolder()+mname);
    if(!out)
            throw("Cannot create "+mname);

    out << "date, I0,I20,I65,I80,X0,X20,X65,X80,D0,D20,D65,D80,First0,First20,First65,First80,Second0,Second20,Second65,Second80" << endl;

    for(unsigned i=0; i<numdates; i++)
    {
        out << int2date(i+zerodate) << ",";
        out << I[i][0] << "," << I[i][1] << ","
             << I[i][2] << "," << I[i][3] << ",";
        out << R[i][0] << "," << R[i][1] << ","
             << R[i][2] << "," << R[i][3] << ",";
        out << D[i][0] << "," << D[i][1] << ","
             << D[i][2] << "," << D[i][3] << ",";
        out << O[i][0] << "," << O[i][1] << ","
             << O[i][2] << "," << O[i][3] << ",";
        out << O[i][4] << "," << O[i][5] << ","
             << O[i][6] << "," << O[i][7] << endl;
    }
}





void uzis2uzis(const string& horizon)
{
    enum labels {vek_kat,pohlavi,kraj_bydliste,kraj_prvni_nemocnice,
                 datum_priznaku,datum_odberu,datum_positivity,stav_dle_khs,
                 zahajeni_hosp,
                 posledni_zaznam,stav_posledni_zaznam,posledni_hosp_zaznam,nejtezsi_stav,
                 tezky_stav,tezky_stav_pocatek,dni_tezky_stav,tezky_stav_posledni,
                 jip,jip_pocatek,dni_jip,jip_posledni,
                 kyslik,kyslik_pocatek,dni_kyslik,kyslik_posledni,
                 upv,upv_pocatek,dni_upv,upv_posledni,
                 ecmo,ecmo_pocatek,dni_ecmo,ecmo_posledni,
                 umrti,datum_umrti,numlabels};


    enum gender {man, woman, other, numgenders};

    enum dateinds {hosp, release, death, numdateinds};

    int lastdate = date2int(horizon);
    int numdates = lastdate - zerodate + 1;

    vector<counter> cntr(numdateinds); // i know the first will be unused
    unsigned inconsistent = 0;



    vector<vector<double>> h(numcohorts,vector<double>(numdates,0));
    vector<vector<double>> r(numcohorts,vector<double>(numdates,0));
    vector<vector<double>> dhosp(numcohorts,vector<double>(numdates,0));

//    const unsigned numdurs = 100;
//    vector<vector<unsigned>> durhist(2*3,vector<unsigned>(numdurs,0));
//    vector<vector<unsigned>> hospdurhist(2*3,vector<unsigned>(numdurs,0));

//    enum efromis { efideath, efihosp, efidet};

    csv<';'> src("/home/martin/Documents/s/covid/data/epidemie/modely_05_hospitalizovani_analyza.csv");

    assert(src.c(0)==numlabels);

    for(unsigned i=1; i<src.r(); i++)
    {
        string cs = src(i,vek_kat);
        cohorts c;
        string ls;
        unsigned age;
        for(unsigned j=0; j<cs.size() && cs[j] != '-' && cs[j] != '+'; j++)
            ls += cs[j];
        try
        {
            age = stoul(ls);
        }
        catch (...)
        {
            clog << "Cannot convert'" << ls << "' to unsigned" << endl;
            inconsistent++;
            continue;
        }
        if(age >= 80)
            c = c80;
        else if(age >= 65)
            c = c65;
        else if(age >= 20)
            c = c20;
        else
            c = c0;

        gender g;
        auto pohlstr = src(i,pohlavi);
        if(pohlstr == "M")
            g = man;
        else if(pohlstr == "Z")
            g = woman;
        else
            g = other;

        vector<int> res(numlabels);
        int dh = date2int(src(i,zahajeni_hosp),zerodate,lastdate,cntr[hosp]);
        int dlr = date2int(src(i,posledni_zaznam),zerodate,lastdate,cntr[release]);
        int dd = date2int(src(i,datum_umrti),zerodate,lastdate,cntr[death]);

        if(dh < maxint)
            h[c][dh-zerodate]++;
        if(dd < maxint)
            dhosp[c][dd-zerodate]++;
        else if(dlr < maxint && src(i,posledni_hosp_zaznam)[0] != 'h') // hospitalizace okracuje
            r[c][dlr-zerodate]++;

        if(dh==maxint)
        {
            clog << "record without hosp date" << endl;
            inconsistent++;
        }
        else if(dlr==maxint)
        {
            clog << "record without last record or " << endl;
            inconsistent++;
        }
    }



    for(unsigned j=0; j<numdateinds; j++)
    {
        cout << j << ": " << cntr[j].wrong << " wrong,"
               << cntr[j].over << " over,"
               << cntr[j].under << " under" << endl;
    }

    cout << inconsistent << " inconsistent records." << endl;
    cout << endl;

    string mname = "uzis.csv";
    ofstream out(sys::outputfolder()+mname);
    if(!out)
            throw("Cannot create "+mname);


    out << "date,H0,H20,H65,H80,R0,R20,R65,R80,DH0,DH20,DH65,DH80" << endl;
    for(int i=0; i<numdates; i++)
    {
        out << int2date(i+zerodate) << ",";
        for(unsigned j=0; j<numcohorts; j++)
            out << h[j][i] << ",";
        for(unsigned j=0; j<numcohorts; j++)
            out << r[j][i] << ",";
        for(unsigned j=0; j<numcohorts; j++)
            out << dhosp[j][i] << ",";
        out << endl;
    }

}


void mzcr2districts(const string& horizon)
{
    int lastdate = date2int(horizon);
    int numdates = lastdate - zerodate + 1;
//    int firstwnum = 9;
    int numweeks = (numdates + 6) / 7;
    int lastweekinoutput = 72;
    if(lastweekinoutput < numweeks)
        throw "too much weeks";
    counter ocounter;
    unsigned inconsistento = 0;

    unsigned firstschool = 3;
    unsigned numclasses = 3+9+4+1;
    unsigned numdistricts = districts.size();


    vector<vector<unsigned>> C(numdistricts,vector<unsigned>(numweeks,0));
    vector<vector<vector<unsigned>>> E(numdistricts,
             vector<vector<unsigned>>(numdates,vector<unsigned>(numclasses,0)));
    vector<unsigned> Y(numweeks,0);
    vector<unsigned> YM(numweeks,0);

    csv<','> osoby("/home/martin/Documents/s/covid/data/mzcr/osoby.csv");

    cout << "Importing osoby" << endl;

    for(unsigned i=1; i<osoby.r(); i++)
    {
        enum {datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod, nakaza_v_zahranici,nakaza_zeme_csu_kod};

        string ds = osoby(i,datum);
        int d = date2int(ds, zerodate,lastdate, ocounter);

        if(d == maxint)
        {
            clog << "Invalid or out of range date " << ds << endl;
            inconsistento++;
        }
        else
        {
            int w = (d-zerodate) / 7;
            const unsigned noage = 1000;
            unsigned v = noage;
            try
            {
                v = osoby.getunsigned(i,vek);
            }
            catch(...)
            {
                clog << "Bad age " << osoby(i,vek) << endl;
                inconsistento++;
            }
            if(v != noage)
            {
                int distr = str2district(osoby(i,okres_lau_kod));
                if(distr == maxint)
                {
                    clog << "Unknown district code " << osoby(i,okres_lau_kod) << endl;
                    inconsistento++;
                }
                else
                {
                    if(v>=firstschool && v < firstschool + numclasses)
                        E[distr][w][v-firstschool]++;
                    else
                        YM[w]++;
                    C[distr][w]++;
                    Y[w]++;
                }
            }
        }
    }
    cout << inconsistento << " records: "
         << ocounter.wrong << " wrong dates,"
         << ocounter.over << " dates over,"
         << ocounter.under << " dates under" << endl;

    ofstream o(sys::outputfolder()+"schools.csv");
    if(!o)
    {
        cerr << "Cannot open " + sys::outputfolder()+"schools.csv" << endl;
        throw;
    }

    o << "D,W,Y,YM,YD,I,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,C16,C17,C18,C19" << endl;
    for(unsigned i=0; i<numdistricts; i++)
    {
        int j=1;
        for(; j<numweeks; j++) // we start on 2.3.
        {
            o << i << "," << j << "," << Y[j] << "," << YM[j] << "," << C[i][j] << "," << districts[i].inhabitants;
            for(unsigned k=0; k<numclasses; k++)
                o << "," << E[i][j][k];
            o << endl;
        }
        for(; j<=lastweekinoutput; j++)
            o << i << "," << j << endl;
    }
}


void mzcrocko2districts(const string& horizon)
{
    bool vekfilter = true;
    unsigned minvek = 12;
    unsigned maxvek = 24;
    vector<string> vks = { "12-15", "16-17", "18-24" };

    vector<double> excessdeaths = {
                            113.550907658245	,
                            167.239271615032	,
                            149.638490189589	,
                            208.107390181053	,
                            380.824449134195	,
                            209.056504007324	,
                            198.880843254775	,
                            257.623869151039	,
                            209.044172774148	,
                            122.432455802473	,
                            135.86240417894	,
                            134.491738591199	,
                            117.389707973709	,
                            161.967214214216};


    int lastdate = date2int(horizon);
    int numdates = lastdate - zerodate + 1;
//    int firstwnum = 9;
    int numweeks = (numdates + 6) / 7;
    counter ocounter;
    unsigned inconsistento = 0;

    unsigned numdistricts = districts.size();


    vector<vector<unsigned>> C(numdistricts,vector<unsigned>(numweeks,0));
    vector<unsigned> Y(numweeks,0);

    csv<','> osoby("/home/martin/Documents/s/covid/data/mzcr/osoby.csv");

    cout << "Importing osoby" << endl;

    for(unsigned i=1; i<osoby.r(); i++)
    {
        enum {datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod, nakaza_v_zahranici,nakaza_zeme_csu_kod};

        bool vekok = true;
        if(vekfilter)
        {
           try
           {
              unsigned v = osoby.getunsigned(i,vek);
              if(v < minvek || v > maxvek)
                  vekok = false;
           }
           catch(...) {}
        }

        if(vekok)
        {
            string ds = osoby(i,datum);
            int d = date2int(ds, zerodate,lastdate, ocounter);

            if(d == maxint)
            {
                clog << "Invalid or out of range date " << ds << endl;
                inconsistento++;
            }
            else
            {
                int w = (d-zerodate) / 7;
                int distr = str2district(osoby(i,okres_lau_kod));
                if(distr == maxint)
                {
                    clog << "Unknown district code " << osoby(i,okres_lau_kod) << endl;
                    inconsistento++;
                }
                else
                {
                    C[distr][w]++;
                    Y[w]++;
                }
            }
        }
    }

    cout << inconsistento << " inconsistent records: "
         << ocounter.wrong << " wrong dates,"
         << ocounter.over << " dates over,"
         << ocounter.under << " dates under" << endl;

    counter ucounter;
    unsigned inconsistentu = 0;
    vector<vector<unsigned>> D(numdistricts,vector<unsigned>(numweeks,0));

    csv<','> umrti("/home/martin/Documents/s/covid/data/mzcr/umrti.csv");

    cout << "Importing umrti" << endl;

    for(unsigned i=1; i<umrti.r(); i++)
    {
        enum labels {datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod};
        bool vekok = true;
        if(vekfilter)
        {
           try
           {
              unsigned v = osoby.getunsigned(i,vek);
              if(v < minvek || v > maxvek)
                  vekok = false;
           }
           catch(...) {}
        }

        if(vekok)
        {

            string ds = umrti(i,datum);
            int d = date2int(ds, zerodate,lastdate, ucounter);

            if(d == maxint)
            {
                clog << "Invalid or out of range date " << ds << endl;
                inconsistentu++;
            }
            else
            {
                int w = (d-zerodate) / 7;

                int distr = str2district(umrti(i,okres_lau_kod));
                if(distr == maxint)
                {
                    clog << "Unknown district code " << umrti(i,okres_lau_kod) << endl;
                    inconsistentu++;
                }
                else
                    D[distr][w]++;
            }
        }
    }
    cout << inconsistentu << " inconsistent records: "
         << ucounter.wrong << " wrong dates,"
         << ucounter.over << " dates over,"
         << ucounter.under << " dates under" << endl;


    counter pcounter;
    unsigned inconsistentp = 0;

    vector<vector<unsigned>> O(numdistricts,vector<unsigned>(numweeks,0));
    vector<unsigned> V(numweeks,0);

    csv<','> profese("/home/martin/Documents/s/covid/data/mzcr/ockovani-profese.csv");

    cout << "Importing profese" << endl;

    unsigned firstshots = 0;
    unsigned secshots = 0;
    for(unsigned i=1; i<profese.r(); i++)
    {
        enum {datum,vakcina,kraj_nuts_kod,kraj_nazev,zarizeni_kod,zarizeni_nazev,poradi_davky,indikace_zdravotnik,indikace_socialni_sluzby,indikace_ostatni,indikace_pedagog,indikace_skolstvi_ostatni,indikace_bezpecnostni_infrastruktura,indikace_chronicke_onemocneni,vekova_skupina,orp_bydliste,orp_bydliste_kod,prioritni_skupina_kod,pohlavi,zrizovatel_kod,zrizovatel_nazev,vakcina_kod,ukoncujici_davka};
        bool vekok;
        if(!vekfilter)
            vekok = true;
        else
        {
            vekok = false;
            string v=profese(i,vekova_skupina);
            for(unsigned j=0; j<vks.size(); j++)
                if(v==vks[j])
                {
                    vekok = true;
                    break;
                }
        }

        if(vekok)
        {
//            cout << "vek: " << profese(i,vekova_skupina) << endl;

            if(profese(i,poradi_davky)=="2" || profese(i,vakcina_kod)=="CO04")
            {
                secshots++;
                string ds = profese(i,datum);
                int d = date2int(ds, zerodate,lastdate, pcounter);

                if(d == maxint)
                {
                    clog << "Invalid or out of range date " << ds << endl;
                    inconsistentp++;
                }
                else
                {
                    int w = (d-zerodate) / 7;
                    {
                        string orp = profese(i,orp_bydliste);
                        int distr = orp2district(orp);
                        if(distr == maxint)
                        {
                            if(orp != "")
                                clog << "Unknown orp code " << orp << endl;
                            inconsistentp++;
                        }
                        else
                        {
                            O[distr][w]++;
                            V[w]++;
                        }
                    }
                }
            }
            else
                firstshots++;
        }
    }

    cout << "First/sec:" << firstshots << "/" << secshots << endl;
    cout << inconsistentp << " inconsistent records: "
         << pcounter.wrong << " wrong dates,"
         << pcounter.over << " dates over,"
         << pcounter.under << " dates under" << endl;

    ofstream o(sys::outputfolder()+(vekfilter ? "districtsyoung.csv" : "districts.csv"));
    if(!o)
    {
        cerr << "Cannot open " + sys::outputfolder()+"distrocts.csv" << endl;
        throw;
    }

    o << "District,Week,Y,Id,v,i,d,e" << endl;
    for(unsigned i=0; i<numdistricts; i++)
    {
        int j=1;
        unsigned totald=0;
        unsigned totalg=0;
        unsigned totalvd=0;
        unsigned totalvg=0;
        unsigned totalud=0;
        for(; j<numweeks; j++) // we start on 2.3.
        {
            totalg += Y[j];
            totald += C[i][j];
            totalud += D[i][j];
            totalvg += V[j];
            totalvd += O[i][j];
            const double k=100000;

            o << i << "," << j << ","
              << Y[j]/czinhabitants*k << ","
              << static_cast<double>(C[i][j])/districts[i].inhabitants*k  << ","
//              << totalvg / czinhabitants << ","
              << static_cast<double>(totalvd) / districts[i].inhabitants << ","
//              << totalg / czinhabitants << ","
              << static_cast<double>(totald) / districts[i].inhabitants << ","
              << static_cast<double>(totalud) / districts[i].inhabitants * k<< ","
              << excessdeaths[districts[i].region]
            << endl;
        }
    }
}



enum evaccine { Comirnaty, SPIKEVAX, VAXZEVRIA, Janssen, numvaccines };

evaccine vac2vac(const string& vstr )
{
    static vector<string> vaccnames = {"Comirnaty", "SPIKEVAX", "VAXZEVRIA", "COVID-19 Vaccine Janssen"};

    unsigned i = 0;
    for(;i<numvaccines; i++)
        if(vstr == vaccnames[i])
            break;
    return static_cast<evaccine>(i);
}

void ockodata2districts() // rozdelane
{
    string lastdatestr = "2021-11-05";
//    string firstextrastr = "2021-09-20";
    string firstvaccdatestr = "2021-12-27";

//    unsigned firstextra = date2int(firstextrastr);
    unsigned firstvaccdate = date2int(firstvaccdatestr);

//    unsigned firstextrarel = firstextra - zerodate ;
    unsigned firstvaccdaterel = firstvaccdate - zerodate;

    enum egender { emale, efemale, enumgenders };
    enum evk  { v1215, v1617, v1824, v2529, v3034, v3539, v4044,v4549, v5055,v5559, v6054,v6559, v7074,v7579, v80plus, vunknown, numvks };


    int lastdate = date2int(lastdatestr);
    int numdates = lastdate - zerodate + 1;

    int numweeks = (numdates + 6) / 7;

    unsigned inconsistento = 0;

    struct survivalrecord
    {
        bool extra = false;
        vector<unsigned> delta;
        vector<unsigned> events;
    };

    unsigned noagerecords = 0;
//    unsigned earlyextras = 0;

    vector<vector<vector<vector<survivalrecord>>>> Y(numdates,
        vector<vector<vector<survivalrecord>>>(numvaccines,
        vector<vector<survivalrecord>>(enumgenders,
            vector<survivalrecord>(numvks))));

    for(unsigned i=firstvaccdaterel; i<=lastdate; i++)
        for(unsigned j=0; j<numvaccines; j++)
            for(unsigned k=0; k<enumgenders; k++)
                for(unsigned l=0; l<numvks; l++)
                {
                    Y[i][j][k][l].delta.resize(numdates-i,0);
                    Y[i][j][k][l].events.resize(numdates-i,0);
                }

//    auto H = Y;
//    auto J = Y;
//    auto D = Y;

    csv<','> data("/home/martin/Documents/s/covid/data/epidemie/xaa.csv");//data.csv");

    cout << "Importing ockodata" << endl;

    counter vcounter;
    counter icounter;
    counter ecounter;

    for(unsigned i=1; i<data.r(); i++)
    {
        enum {pohlavi,vek,GEO_KrajKod,GEO_ORPKod,DatumPrvnihoPriznakuOKmin,
              DatumOdberuOKmin,DatumVysledkuOKmin,DatumHlaseniOKmin,KHS_IsolaceDatum,
              VYL_datum_vyleceni_final,ZEM_datum_umrti_fin,ZEM_umrti_bin,
              prvnidavka,druhadavka,ukoncene_ockovani,extra_davka,
              OckovaciLatka,OckovaciLatka1,OckovaciLatka2,OckovaciLatka3,
              nakaza_po_dokonc,vek_H,vek_kat,pohlavi_H,kraj_bydliste,kraj_prvni_nemocnice,datum_positivity,zahajeni_hosp,posledni_zaznam,datum_priznaku,datum_odberu,stav_dle_khs,stav_posledni_zaznam,posledni_hosp_zaznam,nejtezsi_stav,tezky_stav,tezky_stav_pocatek,dni_tezky_stav,tezky_stav_posledni,jip,jip_pocatek,dni_jip,jip_posledni,kyslik,kyslik_pocatek,dni_kyslik,kyslik_posledni,upv,upv_pocatek,dni_upv,upv_posledni,ecmo,ecmo_pocatek,dni_ecmo,ecmo_posledni,umrti,datum_umrti,HFNO,HFNO_pocatek,dni_HFNO,HFNO_posledni,DatumHlaseni1,DatumHlaseni2,DatumHlaseni3,DatumPrvnihoPriznaku1,DatumPrvnihoPriznaku2,DatumPrvnihoPriznaku3,DatumOdberu1,DatumOdberu2,DatumOdberu3,DatumVysledku1,DatumVysledku2,DatumVysledku3};

        string vstr = data(i,OckovaciLatka2);
        if(vstr == "")
            continue;
        evaccine v = vac2vac(vstr);
        if(v==numvaccines)
            throw "unknown vaccine " + vstr;

        evk vk;
        string vs = data(i,vek);
        if(vs=="")
        {
            noagerecords++;
            vk = vunknown;
        }
        else
        {
            try
            {
                unsigned age = stoul(vs);
                switch(age)
                {
                    case 12:
                        vk=v1215;
                        break;
                    case 16:
                         vk=v1617;
                         break;
                    case 18:
                         vk=v1824;
                         break;
                    case 25:
                         vk=v2529;
                         break;
                    case 30:
                         vk=v3034;
                         break;
                    case 35:
                         vk=v3539;
                         break;
                    case 40:
                         vk=v4044;
                         break;
                    case 45:
                         vk=v4549;
                         break;
                    case 50:
                         vk=v5055;
                         break;
                    case 55:
                         vk=v5559;
                         break;
                    case 60:
                         vk=v6054;
                         break;
                    case 65:
                         vk=v6559;
                         break;
                    case 70:
                         vk=v7074;
                         break;
                    case 75:
                         vk=v7579;
                         break;
                    case 80:
                         vk=v80plus;
                         break;
                    default:
                    {
                        clog << "bad age " << age << endl;
                        inconsistento++;
                        noagerecords++;
                        continue;
                    }
                }
            }
            catch (...)
            {
                clog << "Cannot convert '" << vs << "' to unsigned" << endl;
                noagerecords++;
                inconsistento++;
                continue;
            }
         }

         string gstr = data(i,pohlavi);
         egender g;
         if(gstr=="M")
             g = emale;
         else if(gstr=="Z")
             g = efemale;
         else
         {
             clog << "Unknown gender '" << gstr << "'" << endl;
             inconsistento++;
             continue;
         }


         string vaccdatestr = data(i,ukoncene_ockovani);
         if(vaccdatestr == "")
         {
             clog << "missing ukoncene_ockovani" << endl;
             inconsistento++;
             continue;
         }

         int vaccdate = date2int(vaccdatestr, firstvaccdate,lastdate, vcounter);
         if(vaccdate == maxint)
         {
             clog << "Invalid or out of range vacc date " << vaccdatestr << endl;
             inconsistento++;
             continue;
         }

         string infstring = data(i,datum_priznaku);
         if(infstring == "")
         {
               infstring = data(i,datum_positivity);
               if(infstring == "")
                   infstring = data(i,datum_odberu);
         }
         if(infstring == "")
         {
             clog << "Unknown infection date " << endl;
             inconsistento++;
             continue;
         }

         int infdate = date2int(infstring, zerodate,lastdate, icounter);
         if(infdate == maxint)
         {
             clog << "Invalid or out of range infection date " << vaccdatestr << endl;
             inconsistento++;
             continue;
         }

         if(infdate < vaccdate)
         {
             clog << "infection date less than vacc date " << endl;
             inconsistento++;
             continue;
         }


         int extradate = maxint;
         string extradatestr = data(i,extra_davka);
         if(extradatestr != "")
         {
             extradate = date2int(extradatestr,vaccdate,lastdate,ecounter);

             if(extradate == maxint)
             {
                 clog << "Invalid or out of range extra date " << extradatestr << endl;
                 inconsistento++;
                 continue;
             }

         }

         int t = vaccdate - firstvaccdate;
         assert(i >= 0 || i<Y.size());
         survivalrecord& sr = Y[t][v][g][vk];
         assert(sr.events.size() > infdate - vaccdate);
         assert(infdate-vaccdate >= 0);
         sr.events[infdate-vaccdate]++;

         if(extradate != maxint)
         {
            assert(sr.delta.size() > extradate - vaccdate);
            assert(extradate-vaccdate >= 0);
            sr.delta[extradate-vaccdate]--;
         }
    }
    cout << inconsistento << " inconsistent records: "
         << "Vaccination dates: " << endl
         << vcounter.wrong << " wrong dates,"
         << vcounter.over << " dates over,"
         << vcounter.under << " dates under" << endl
         << "Infection dates : " << endl
         << icounter.wrong << " wrong dates,"
         << icounter.over << " dates over,"
         << icounter.under << " dates under" << endl
         << "Extra dates : " << endl
         << ecounter.wrong << " wrong dates,"
         << ecounter.over << " dates over,"
         << ecounter.under << " dates under" << endl;
    cout << endl << noagerecords << " records without age" << endl;

    counter pcounter;
    unsigned inconsistentp = 0;


    csv<','> profese("/home/martin/Documents/s/covid/data/mzcr/ockovani-profese.csv");

    cout << "Importing profese" << endl;

    unsigned pnoagerecords = 0;

    for(unsigned i=1; i<profese.r(); i++)
    {
        enum {datum,vakcina,kraj_nuts_kod,kraj_nazev,zarizeni_kod,zarizeni_nazev,poradi_davky,indikace_zdravotnik,indikace_socialni_sluzby,indikace_ostatni,indikace_pedagog,indikace_skolstvi_ostatni,indikace_bezpecnostni_infrastruktura,indikace_chronicke_onemocneni,vekova_skupina,orp_bydliste,orp_bydliste_kod,prioritni_skupina_kod,pohlavi,zrizovatel_kod,zrizovatel_nazev,vakcina_kod,ukoncujici_davka};

        string vcstr=profese(i,vakcina_kod);
        if(profese(i,poradi_davky)=="2" || vcstr=="CO04")
        {
            evk vk;
            string cs = profese(i,vekova_skupina);
            string ls;
            unsigned age;
            for(unsigned j=0; j<cs.size() && cs[j] != '-' && cs[j] != '+'; j++)
                ls += cs[j];
            try
            {
                age = stoul(ls);
                switch(age)
                {
                case 12:
                    vk=v1215;
                    break;
                case 16:
                     vk=v1617;
                     break;
                case 18:
                     vk=v1824;
                     break;
                case 25:
                     vk=v2529;
                     break;
                case 30:
                     vk=v3034;
                     break;
                case 35:
                     vk=v3539;
                     break;
                case 40:
                     vk=v4044;
                     break;
                case 45:
                     vk=v4549;
                     break;
                case 50:
                     vk=v5055;
                     break;
                case 55:
                     vk=v5559;
                     break;
                case 60:
                     vk=v6054;
                     break;
                case 65:
                     vk=v6559;
                     break;
                case 70:
                     vk=v7074;
                     break;
                case 75:
                     vk=v7579;
                     break;
                case 80:
                     vk=v80plus;
                     break;
                default:
                    clog << "bad age " << age << endl;
                    inconsistentp++;
                    pnoagerecords++;
                }

            }
            catch (...)
            {
                clog << "Cannot convert'" << ls << "' to unsigned" << endl;
                inconsistentp++;
                continue;
            }


            evaccine v;
            if(vcstr == "CO01")
                v = Comirnaty;
            else if(vcstr == "CO02")
                v = SPIKEVAX;
            else if(vcstr == "CO03")
                v = VAXZEVRIA;
            else if(vcstr == "CO04")
                v =  Janssen;
            else
            {
                clog << "Unknown vaccine code " << vcstr << endl;
                inconsistentp++;
                continue;
            }

            string gstr = profese(i,pohlavi);
            egender g;
            if(gstr=="M")
                g = emale;
            else if(gstr=="Z")
                g = efemale;
            else
            {
                clog << "Unknown gender '" << gstr << "'" << endl;
                inconsistento++;
                continue;
            }

            string ds = profese(i,datum);
            int d = date2int(ds, zerodate,lastdate, pcounter);

            if(d == maxint)
            {
                clog << "Invalid or out of range date " << ds << endl;
                inconsistentp++;
                continue;
            }
            else
            {
                int t = d - firstvaccdate;
                assert(t>=0 && t < Y.size());
                Y[t][v][g][vk].delta[0]++;
            }
        }
    }

    cout << inconsistentp << " inconsistent records: "
         << pcounter.wrong << " wrong dates,"
         << pcounter.over << " dates over,"
         << pcounter.under << " dates under" << endl;

    ofstream o(sys::outputfolder()+"survivalcohorts.csv");
    if(!o)
    {
        cerr << "Cannot open " + sys::outputfolder()+"distrocts.csv" << endl;
        throw;
    }



}


int main()
{
    double startt = time(0);

    try
    {
//        sys::setoutputfolder("../output/");
//        sys::settmpfolder("../tmp/");
      mzcr2mzcr("2021-11-14", "2021-06-04");
      uzis2uzis("2021-11-14");
//
//        mzcr2districts("2021-07-04");
//        mzcrocko2districts("2021-10-31");
    }
    catch (std::exception& e) {
        std::cerr << e.what() << endl;
        return 1;
    }
    catch (const char* m)
    {
           cerr << m << endl;
           return 1;
    }
    catch (const string& m)
    {
           cerr << m << endl;
           return 1;
    }
    catch(...)
    {
        cerr << "unknown exception" << endl;
        return 1;
    }


    clog << "time: " << time(0)-startt << " seconds." << endl;

    return 0;
}
