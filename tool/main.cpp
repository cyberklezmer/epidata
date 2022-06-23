#include "tooltools.hpp"

static int constexpr v1delay = 14;
static int constexpr v2delay = 14;
static int constexpr v3delay = 7;

enum eagegroups { v1215, v1617, v1824, v2529, v3034, v3539, v4044,v4549, v5055,v5559, v6054,v6559, v7074,v7579, v80plus, vunknown, numvks };


string age2group(unsigned age)
{
    static vector<string> g = {		"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"0-11",	"12-15",	"12-15",	"12-15",	"12-15",	"16-17",	"16-17",	"18-24",	"18-24",	"18-24",	"18-24",	"18-24",	"18-24",	"18-24",	"25-29",	"25-29",	"25-29",	"25-29",	"25-29",	"30-34",	"30-34",	"30-34",	"30-34",	"30-34",	"35-39",	"35-39",	"35-39",	"35-39",	"35-39",	"40-44",	"40-44",	"40-44",	"40-44",	"40-44",	"45-49",	"45-49",	"45-49",	"45-49",	"45-49",	"50-54",	"50-54",	"50-54",	"50-54",	"50-54",	"55-59",	"55-59",	"55-59",	"55-59",	"55-59",	"60-64",	"60-64",	"60-64",	"60-64",	"60-64",	"65-69",	"65-69",	"65-69",	"65-69",	"65-69",	"70-74",	"70-74",	"70-74",	"70-74",	"70-74",	"75-79",	"75-79",	"75-79",	"75-79",	"75-79",	"80+"	};
    if(age >= g.size())
       return g[g.size()-1];
    else
       return g[age];
}

enum evaccine { Comirnaty, SPIKEVAX, VAXZEVRIA, Janssen, enumvaccines, eunknownvaccine = enumvaccines };

evaccine vac2vac(const string& vstr )
{
    static vector<string> vaccnames = {"Comirnaty", "SPIKEVAX", "VAXZEVRIA", "COVID-19 Vaccine Janssen"};

    unsigned i = 0;
    for(;i<enumvaccines; i++)
        if(vstr == vaccnames[i])
            break;
    return static_cast<evaccine>(i);
}

#include "tooltools.hpp"



void mzcr2mzcr(const string& horizon,  const string& start16)
{
    int s16 = date2int(start16);
    int lastdate = date2int(horizon);
    int numdates = lastdate - zerodate + 1;

    counter ocounter;
    unsigned inconsistento = 0;


    vector<vector<unsigned>> I(numdates,vector<unsigned>(numcohorts,0));
    vector<vector<unsigned>> R(numdates,vector<unsigned>(numcohorts,0));

    csv<','> osoby("/home/martin/data/mzcr/osoby.csv");

    cout << "Importing osoby" << endl;

    for(unsigned i=1; i<osoby.r(); i++)
    {
        enum {id,datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod, nakaza_v_zahranici,nakaza_zeme_csu_kod};

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

    csv<','> umrti("/home/martin/data/mzcr/umrti.csv");

    cout << "Importing umrti" << endl;

    for(unsigned i=1; i<umrti.r(); i++)
    {
        enum labels {id,datum,vek,pohlavi,kraj_nuts_kod,okres_lau_kod};

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
    vector<vector<unsigned>> O(numdates,vector<unsigned>(numcohorts * 3,0));

    csv<','> ockovani("/home/martin/data/mzcr/ockovani.csv");

    cout << "Importing ockovani" << endl;

    for(unsigned i=1; i<ockovani.r(); i++)
    {
        enum labels {id,datum,vakcina,kraj_nuts_kod,kraj_nazev,vekova_skupina,
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
            unsigned first;
            unsigned second;
            if(ockovani(i,vakcina)=="COVID-19 Vaccine Janssen")
            {
                first = 0;
                second = ockovani.getunsigned(i,prvnich_davek);
            }
            else
            {
                first = ockovani.getunsigned(i,prvnich_davek);
                second = ockovani.getunsigned(i,druhych_davek);
            }
            unsigned boosters = ockovani.getunsigned(i,celkem_davek)
                    - ockovani.getunsigned(i,prvnich_davek)
                    - ockovani.getunsigned(i,druhych_davek);
            O[d-zerodate][c] += first;
            O[d-zerodate][c+numcohorts] += second;
            O[d-zerodate][c+2*numcohorts] += boosters;
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

    out << "date, I0,I20,I65,I80,X0,X20,X65,X80,D0,D20,D65,D80,First0,First20,First65,First80,Final0,Final20,Final65,Final80,Booster0,Booster20,Booster65,Booster80" << endl;

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
             << O[i][6] << "," << O[i][7] << ",";
        out << O[i][8] << "," << O[i][9] << ","
             << O[i][10] << "," << O[i][11] << endl;
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

    csv<';'> src("/home/martin/data/epidemie/modely_05_hospitalizovani_analyza.csv");

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

    csv<','> osoby("/home/martin/data/mzcr/osoby.csv");

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

    csv<','> osoby("/home/martin/data/mzcr/osoby.csv");

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

    csv<','> umrti("/home/martin/data/mzcr/umrti.csv");

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

    csv<','> profese("/home/martin/data/mzcr/ockovani-profese.csv");

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


void ockodata2R() // rozdelane
{
    string lastdatestr = "2021-11-05";
    string firstvaccdatestr = "2021-12-26";

    int firstvaccdate = date2int(firstvaccdatestr);

    enum egender { emale, efemale, enumgenders };

    int lastdate = date2int(lastdatestr);

    int T = lastdate - firstvaccdate;

    unsigned inconsistento = 0;

    enum evaccorder { novacc, firstdose, secdose, booster, numvacorders, enumvaccorders, eunknownvaccorder = enumvaccorders };

    struct vaccrecord {
                      int t = 0;
                      evaccorder vaccorder = eunknownvaccorder;
                      evaccine vac = eunknownvaccine;
     };

    unsigned noagerecords = 0;

    ofstream o(sys::outputfolder()+"toR.csv");
    if(!o)
    {
        cerr << "Cannot open " + sys::outputfolder()+"toR.csv" << endl;
        throw;
    }

    o << "Subject,T1,T2,Inf,InfPrior,AgeGr, Sex" << endl;

    csv<','> data("/home/martin/data/epidemie/part/part.csv");//data.csv");

    cout << "Importing ockodata" << endl;

    static vector<string> labels = {"pohlavi","vek","GEO_KrajKod","GEO_ORPKod","DatumPrvnihoPriznakuOKmin","DatumOdberuOKmin","DatumVysledkuOKmin","DatumHlaseniOKmin","KHS_IsolaceDatum","VYL_datum_vyleceni_final","ZEM_datum_umrti_fin","ZEM_umrti_bin","ock_vek","ock_pohlavi","prvnidavka","druhadavka","ukoncene_ockovani","extra_davka","OckovaciLatka","OckovaciLatka1","OckovaciLatka2","OckovaciLatka3","nakaza_po_dokonc","vek","vek_kat","pohlavi","kraj_bydliste","kraj_prvni_nemocnice","datum_positivity","zahajeni_hosp","posledni_zaznam","datum_priznaku","datum_odberu","stav_dle_khs","stav_posledni_zaznam","posledni_hosp_zaznam","nejtezsi_stav","tezky_stav","tezky_stav_pocatek","dni_tezky_stav","tezky_stav_posledni","jip","jip_pocatek","dni_jip","jip_posledni","kyslik","kyslik_pocatek","dni_kyslik","kyslik_posledni","upv","upv_pocatek","dni_upv","upv_posledni","ecmo","ecmo_pocatek","dni_ecmo","ecmo_posledni","umrti","datum_umrti","HFNO","HFNO_pocatek","dni_HFNO","HFNO_posledni","DatumHlaseni1","DatumHlaseni2","DatumHlaseni3","DatumPrvnihoPriznaku1","DatumPrvnihoPriznaku2","DatumPrvnihoPriznaku3","DatumOdberu1","DatumOdberu2","DatumOdberu3","DatumVysledku1","DatumVysledku2","DatumVysledku3"};

    enum elabels {pohlavi,vek,GEO_KrajKod,GEO_ORPKod,DatumPrvnihoPriznakuOKmin,DatumOdberuOKmin,DatumVysledkuOKmin,DatumHlaseniOKmin,KHS_IsolaceDatum,VYL_datum_vyleceni_final,ZEM_datum_umrti_fin,ZEM_umrti_bin,ock_vek,ock_pohlavi,prvnidavka,druhadavka,ukoncene_ockovani,extra_davka,OckovaciLatka,OckovaciLatka1,OckovaciLatka2,OckovaciLatka3,nakaza_po_dokonc,vek_h,vek_kat_h,pohlavi_h,kraj_bydliste,kraj_prvni_nemocnice,datum_positivity,zahajeni_hosp,posledni_zaznam,datum_priznaku,datum_odberu,stav_dle_khs,stav_posledni_zaznam,posledni_hosp_zaznam,nejtezsi_stav,tezky_stav,tezky_stav_pocatek,dni_tezky_stav,tezky_stav_posledni,jip,jip_pocatek,dni_jip,jip_posledni,kyslik,kyslik_pocatek,dni_kyslik,kyslik_posledni,upv,upv_pocatek,dni_upv,upv_posledni,ecmo,ecmo_pocatek,dni_ecmo,ecmo_posledni,umrti,datum_umrti,HFNO,HFNO_pocatek,dni_HFNO,HFNO_posledni,DatumHlaseni1,DatumHlaseni2,DatumHlaseni3,DatumPrvnihoPriznaku1,DatumPrvnihoPriznaku2,DatumPrvnihoPriznaku3,DatumOdberu1,DatumOdberu2,DatumOdberu3,DatumVysledku1,DatumVysledku2,DatumVysledku3, enumlabels};

    assert(data.c(0) == enumlabels);

    counter do1counter;
    counter do2counter;
    counter v1datecounter;
    counter v2datecounter;
    counter v3datecounter;
    counter ucounter;

    for(unsigned i=1; i<data.r(); i++)
    {
        // infekce

        int deathdate = maxint;
        string ddstr = data(i,ZEM_datum_umrti_fin);
        if(ddstr != "") // no death
        {
            int ddabs = date2int(ddstr, zerodate ,lastdate, ucounter);
            if(ddabs==maxint)
            {
                clog << "ddabs " + ddstr;
                continue;
            }
            if(ddabs > firstvaccdate )
               deathdate = ddabs - firstvaccdate;
            else
               continue;
        }

        int infprior = 0;
        vector<int> infections;

        string okstr = data(i,DatumOdberuOKmin);
        if(okstr == "") // no infection
        {
            string do1str = data(i,DatumOdberu1);
            assert(do1str != "");

            int do1abs = date2int(do1str, zerodate, lastdate, do1counter);
            if(do1abs == maxint)
            {
                clog << "do1abs maxint " << endl;
                continue;
            }
            if(do1abs <= firstvaccdate)
                infprior++;
            else
                infections.push_back(do1abs - firstvaccdate);

            string do2str = data(i,DatumOdberu2);
            if(do2str != "") // no infection
            {
                int do2abs = date2int(do2str, zerodate ,lastdate, do2counter);
                if(do2abs == maxint)
                {
                    clog << "do2abs maxint " << endl;
                    continue;
                }
                if(do2abs <= firstvaccdate)
                    infprior++;
                else
                    infections.push_back( do2abs - firstvaccdate);
            }
            // tbd third reinfection
        }

        // vaccines

        vector<vaccrecord> ve;

        string v1str = data(i,OckovaciLatka1);
        if(v1str != "")
        {
            evaccine v = vac2vac(v1str);
            if(v==eunknownvaccine)
                throw "unknown vaccine " + v1str;

            string v1datestr = data(i,prvnidavka);
            if(v1datestr == "")
            {
                clog << "No v1datestr ";
                inconsistento++;
                continue;
            }

            int v1date = date2int(v1datestr, firstvaccdate,lastdate, v1datecounter);
            if(v1date == maxint)
            {
                clog << "bad v1 date " + v1datestr;
                continue;
            }

            vaccrecord v1;
            v1.t = v1date + v1delay;
            v1.vaccorder = firstdose;
            v1.vac = v;
            ve.push_back(v1);

            string v2str = data(i,OckovaciLatka2);
            if(v2str != "")
            {
                evaccine v = vac2vac(v2str);
                if(v==enumvaccines)
                    throw "unknown vaccine " + v2str;

                string v2datestr = data(i,druhadavka);
                if(v2datestr == "")
                {
                    clog << "No v2datestr ";
                    inconsistento++;
                    continue;
                }

                int v2date = date2int(v2datestr, firstvaccdate,lastdate, v2datecounter);
                if(v2date == maxint)
                {
                    clog << "bad v2 date " + v2datestr;
                    continue;
                }

                vaccrecord v2;
                v2.t = v2date + v2delay;
                v2.vaccorder = secdose;
                v2.vac = v;
                ve.push_back(v2);

                string v3str = data(i,OckovaciLatka3);
                if(v3str != "")
                {
                    evaccine v = vac2vac(v3str);
                    if(v==enumvaccines)
                        throw "unknown vaccine " + v3str;

                    string v3datestr = data(i,extra_davka);
                    if(v3datestr == "")
                    {
                        clog << "No v3datestr ";
                        inconsistento++;
                        continue;
                    }

                    int v3date = date2int(v3datestr, firstvaccdate,lastdate, v3datecounter);
                    if(v3date == maxint)
                    {
                        clog << "bad v3 date " + v3datestr;
                        continue;
                    }

                    vaccrecord v3;
                    v3.t = v3date + v3delay;
                    v3.vaccorder = booster;
                    v3.vac = v;
                    ve.push_back(v3);
                }
            }
        }

        string vs = data(i,vek);
        string agelabel = "unknown";

        if(vs=="")
            vs = data(i,vek_h);
        if(vs=="")
        {
            noagerecords++;
            inconsistento++;
        }
        else
        {
            try
            {
                unsigned age = stoul(vs);
                agelabel = age2group(age);
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
/*         egender g;
         if(gstr=="M")
             g = emale;
         else if(gstr=="Z")
             g = efemale;
         else
         {
             clog << "Unknown gender '" << gstr << "'" << endl;
             inconsistento++;
             continue;
         } */

         enum vaccovariate { eunvacc,
                             e1P, e1M, e1A, e1J,
                             e12_1P, e12_1M, e12_1A, e12_1J,
                             e24_1P, e24_1M, e24_1A, e24_1J,
                             e36_1P, e36_1M, e36_1A, e36_1J,
                             e2P, e2M, e2A,
                             e12_2P, e12_2M, e12_2A,
                             e24_2P, e24_2M, e24_2A,
                             e36_2P, e36_2M, e36_2A,
                             e3P, e3M, e3A, e3J, enumvacccovs,
                             eunknownvacccov = enumvacccovs
                             };

         vector<string> vcovtexts {
             "unvacc",
             "1P", "1M", "1A", "1J",
             "12_1P", "12_1M", "12_1A", "12_1J",
             "24_1P", "24_1M", "24_1A", "24_1J",
             "36_1P", "36_1M", "36_1A", "36_1J",
             "2P", "2M", "2A",
             "12_2P", "12_2M", "12_2A",
             "24_2P", "24_2M", "24_2A",
             "36_2P", "36_2M", "36_2A",
             "3P", "3M", "3A", "3J"};

         unsigned vc=eunvacc;
         int lastvcchangedate = maxint;
         int t1 = 0;
         unsigned infptr = 0;
         unsigned vaccptr = 0;
         int enddate = min(deathdate,T);

         for(;;)
         {
             unsigned newinfprior = infprior;

             // looking for a next preevent
             int nextinfdate = maxint;
             if(infptr < infections.size())
                 nextinfdate = infections[infptr];

             int nextvaccdate = maxint;
             if(vaccptr < ve.size())
                 nextvaccdate = ve[vaccptr].t;

             unsigned newvc = vc;
             int nextupdatedate = maxint;
             unsigned candidatevc = vc;
             if(vc > eunvacc)
             {
                 assert(lastvcchangedate < maxint);
                 if(vc>=e12_1P && vc <= e12_1J)
                 {
                     candidatevc = vc+4;
                     nextupdatedate = lastvcchangedate + 10 * 7;
                 }
                 else if(vc >= e24_1P && vc <= e36_1J)
                 {
                     candidatevc = vc+4;
                     nextupdatedate = lastvcchangedate + 12 * 7;
                 }
                 else if(vc>=e12_2P && vc <= e12_2A)
                 {
                     candidatevc = vc+3;
                     nextupdatedate = lastvcchangedate + 10 * 7;
                 }
                 else if(vc >= e24_2P && vc <= e36_2A)
                 {
                     candidatevc = vc+3;
                     nextupdatedate = lastvcchangedate + 12 * 7;
                 }
                 int t2 = min(nextvaccdate,
                              min(nextinfdate,min(nextupdatedate,enddate)));

                 if(t2==nextupdatedate)
                     newvc = candidatevc;

                 if(t2==nextvaccdate)
                 {
                     vaccrecord e = ve[vaccptr++];
                     assert(e.vaccorder >= firstdose);
                     if(e.vaccorder == firstdose)
                     {
                         assert(vc == novacc);
                         newvc = e1P + e.vac;
                     }
                     else if(e.vaccorder == secdose)
                     {
                         assert(vc < e2P);
                         assert(e.vac != Janssen);
                         newvc = e2P + e.vac;
                     }
                     else if(e.vaccorder == booster)
                     {
                         assert(vc < e3P);
                         newvc = e3P + e.vac;
                     }
                     else
                         assert(0);
                 }
                 int inf;
                 if(t2 == nextinfdate)
                 {
                     newinfprior++;
                     infptr++;
                     inf = 1;
                 }
                 else
                     inf = 0;

                 o << i << "," << t1 << "," << t2 << ","
                   << inf << ","
                   << infprior << ","
                   << vcovtexts[vc] << ","
                   << agelabel << ","
                   << gstr << endl;
                if(t2 >= enddate)
                    break;
                infprior = newinfprior;
                vc = newvc;
                t1 = t2;
             }
         }
    }
    cout << inconsistento << " inconsistent records: "
         << "Vaccination dates first: " << endl
         << v1datecounter.wrong << " wrong do1 dates,"
         << v1datecounter.over << " dates over,"
         << v1datecounter.under << " dates under" << endl
         << "Vaccination dates second: " << endl
         << v2datecounter.wrong << " wrong do1 dates,"
         << v2datecounter.over << " dates over,"
         << v2datecounter.under << " dates under" << endl
         << "Vaccination dates third: " << endl
         << v3datecounter.wrong << " wrong do1 dates,"
         << v3datecounter.over << " dates over,"
         << v3datecounter.under << " dates under" << endl
         << "Infection dates : " << endl
         << do1counter.wrong << " wrong do1 dates,"
         << do1counter.over << " dates over,"
         << do1counter.under << " dates under" << endl
         << "Infection2 dates : " << endl
         << do2counter.wrong << " wrong do1 dates,"
         << do2counter.over << " dates over,"
         << do2counter.under << " dates under" << endl
         << "Death dates : " << endl
         << ucounter.wrong << " wrong do1 dates,"
         << ucounter.over << " dates over,"
         << ucounter.under << " dates under" << endl;
    cout << endl << noagerecords << " records without age" << endl;
}




int main()
{
    double startt = time(0);

    try
    {
//        sys::setoutputfolder("../output/");
//        sys::settmpfolder("../tmp/");
//      mzcr2mzcr("2022-04-24", "2021-06-04");
         uzis2uzis("2021-94-24");
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
