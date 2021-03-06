#ifndef DISTRICTS_HPP
#define DISTRICTS_HPP

#include <vector>
#include <string>

struct districtrecord
{
    std::string code;
    std::string name;
    unsigned region;
    unsigned inhabitants;
};

std::vector<districtrecord> districts =
{
    {"CZ0201","Benešov",1,99414},
    {"CZ0202","Beroun",1,95058},
    {"CZ0203","Kladno",1,166483},
    {"CZ0204","Kolín",1,102623},
    {"CZ0205","Kutná Hora",1,75828},
    {"CZ0206","Mělník",1,109302},
    {"CZ0207","Mladá Boleslav",1,130365},
    {"CZ0208","Nymburk",1,100886},
    {"CZ0209","Praha-východ",1,185178},
    {"CZ020A","Praha-západ",1,149338},
    {"CZ020B","Příbram",1,115104},
    {"CZ020C","Rakovník",1,55562},
    {"CZ0311","České Budějovice",2,195903},
    {"CZ0312","Český Krumlov",2,61556},
    {"CZ0313","Jindřichův Hradec",2,90692},
    {"CZ0314","Písek",2,71587},
    {"CZ0315","Prachatice",2,50978},
    {"CZ0316","Strakonice",2,70772},
    {"CZ0317","Tábor",2,102595},
    {"CZ0321","Domažlice",3,62062},
    {"CZ0322","Klatovy",3,86405},
    {"CZ0323","Plzeň-město",3,194280},
    {"CZ0324","Plzeň-jih",3,63488},
    {"CZ0325","Plzeň-sever",3,79979},
    {"CZ0326","Rokycany",3,49349},
    {"CZ0327","Tachov",3,54336},
    {"CZ0411","Cheb",4,91634},
    {"CZ0412","Karlovy Vary",4,114818},
    {"CZ0413","Sokolov",4,88212},
    {"CZ0421","Děčín",5,129542},
    {"CZ0422","Chomutov",5,124946},
    {"CZ0423","Litoměřice",5,119668},
    {"CZ0424","Louny",5,86691},
    {"CZ0425","Most",5,111708},
    {"CZ0426","Teplice",5,129072},
    {"CZ0427","Ústí nad Labem",5,119338},
    {"CZ0511","Česká Lípa",6,103300},
    {"CZ0512","Jablonec nad Nisou",6,90667},
    {"CZ0513","Liberec",6,175626},
    {"CZ0514","Semily",6,74097},
    {"CZ0521","Hradec Králové",7,164283},
    {"CZ0522","Jičín",7,80045},
    {"CZ0523","Náchod",7,109958},
    {"CZ0524","Rychnov nad Kněžnou",7,79383},
    {"CZ0525","Trutnov",7,117978},
    {"CZ0531","Chrudim",8,104613},
    {"CZ0532","Pardubice",8,175441},
    {"CZ0533","Svitavy",8,104333},
    {"CZ0534","Ústí nad Orlicí",8,138275},
    {"CZ0631","Havlíčkův Brod",9,94915},
    {"CZ0632","Jihlava",9,113628},
    {"CZ0633","Pelhřimov",9,72302},
    {"CZ0634","Třebíč",9,110810},
    {"CZ0635","Žďár nad Sázavou",9,118158},
    {"CZ0641","Blansko",10,109136},
    {"CZ0642","Brno-město",10,381346},
    {"CZ0643","Brno-venkov",10,224642},
    {"CZ0644","Břeclav",10,116291},
    {"CZ0645","Hodonín",10,153943},
    {"CZ0646","Vyškov",10,92280},
    {"CZ0647","Znojmo",10,114351},
    {"CZ0711","Jeseník",11,37968},
    {"CZ0712","Olomouc",11,235472},
    {"CZ0713","Prostějov",11,108646},
    {"CZ0714","Přerov",11,129512},
    {"CZ0715","Šumperk",11,120417},
    {"CZ0721","Kroměříž",12,105343},
    {"CZ0722","Uherské Hradiště",12,142226},
    {"CZ0723","Vsetín",12,143334},
    {"CZ0724","Zlín",12,191652},
    {"CZ0801","Bruntál",13,91597},
    {"CZ0802","Frýdek-Místek",13,214660},
    {"CZ0803","Karviná",13,246324},
    {"CZ0804","Nový Jičín",13,151577},
    {"CZ0805","Opava",13,176236},
    {"CZ0806","Ostrava-město",13,320145},
    {"CZ0100","Praha",0,1324277}
};

struct orprecord
{
    std::string district;
    std::string orp;
    unsigned distnum;
};

std::vector<orprecord> orps =
{
    {"Benešov","Benešov",0},
    {"Benešov","Vlašim",0},
    {"Benešov","Votice",0},
    {"Beroun","Beroun",1},
    {"Beroun","Hořovice",1},
    {"Kladno","Kladno",2},
    {"Kladno","Slaný",2},
    {"Kolín","Kolín",3},
    {"Kolín","Český Brod",3},
    {"Kutná Hora","Kutná Hora",4},
    {"Kutná Hora","Čáslav",4},
    {"Mělník","Mělník",5},
    {"Mělník","Kralupy nad Vltavou",5},
    {"Mělník","Neratovice",5},
    {"Mladá Boleslav","Mladá Boleslav",6},
    {"Mladá Boleslav","Mnichovo Hradiště",6},
    {"Nymburk","Nymburk",7},
    {"Nymburk","Lysá nad Labem",7},
    {"Nymburk","Poděbrady",7},
    {"Praha-východ","Brandýs nad Labem-Stará Boleslav",8},
    {"Praha-východ","Říčany",8},
    {"Praha-západ","Černošice",9},
    {"Příbram","Příbram",10},
    {"Příbram","Dobříš",10},
    {"Příbram","Sedlčany",10},
    {"Rakovník","Rakovník",11},
    {"České Budějovice","České Budějovice",12},
    {"České Budějovice","Trhové Sviny",12},
    {"České Budějovice","Týn nad Vltavou",12},
    {"Český Krumlov","Český Krumlov",13},
    {"Český Krumlov","Kaplice",13},
    {"Jindřichův Hradec","Dačice",14},
    {"Jindřichův Hradec","Jindřichův Hradec",14},
    {"Jindřichův Hradec","Třeboň",14},
    {"Písek","Milevsko",15},
    {"Písek","Písek",15},
    {"Prachatice","Prachatice",16},
    {"Prachatice","Vimperk",16},
    {"Strakonice","Blatná",17},
    {"Strakonice","Strakonice",17},
    {"Strakonice","Vodňany",17},
    {"Tábor","Soběslav",18},
    {"Tábor","Tábor",18},
    {"Domažlice","Domažlice",19},
    {"Domažlice","Horšovský Týn",19},
    {"Klatovy","Horažďovice",20},
    {"Klatovy","Klatovy",20},
    {"Klatovy","Sušice",20},
    {"Plzeň-jih","Blovice",22},
    {"Plzeň-jih","Nepomuk",22},
    {"Plzeň-jih","Přeštice",22},
    {"Plzeň-jih","Stod",22},
    {"Plzeň-město","Plzeň",21},
    {"Plzeň-sever","Kralovice",23},
    {"Plzeň-sever","Nýřany",23},
    {"Rokycany","Rokycany",24},
    {"Tachov","Stříbro",25},
    {"Tachov","Tachov",25},
    {"Cheb","Aš",26},
    {"Cheb","Cheb",26},
    {"Cheb","Mariánské Lázně",26},
    {"Karlovy Vary","Karlovy Vary",27},
    {"Karlovy Vary","Ostrov",27},
    {"Sokolov","Kraslice",28},
    {"Sokolov","Sokolov",28},
    {"Děčín","Děčín",29},
    {"Děčín","Rumburk",29},
    {"Děčín","Varnsdorf",29},
    {"Chomutov","Chomutov",30},
    {"Chomutov","Kadaň",30},
    {"Litoměřice","Litoměřice",31},
    {"Litoměřice","Lovosice",31},
    {"Litoměřice","Roudnice nad Labem",31},
    {"Louny","Louny",32},
    {"Louny","Podbořany",32},
    {"Louny","Žatec",32},
    {"Most","Litvínov",33},
    {"Most","Most",33},
    {"Teplice","Bílina",34},
    {"Teplice","Teplice",34},
    {"Ústí nad Labem","Ústí nad Labem",35},
    {"Česká Lípa","Česká Lípa",36},
    {"Česká Lípa","Nový Bor",36},
    {"Jablonec nad Nisou","Jablonec nad Nisou",37},
    {"Jablonec nad Nisou","Tanvald",37},
    {"Jablonec nad Nisou","Železný Brod",37},
    {"Liberec","Frýdlant",38},
    {"Liberec","Liberec",38},
    {"Semily","Jilemnice",39},
    {"Semily","Semily",39},
    {"Semily","Turnov",39},
    {"Hradec Králové","Hradec Králové",40},
    {"Hradec Králové","Nový Bydžov",40},
    {"Jičín","Hořice",41},
    {"Jičín","Jičín",41},
    {"Jičín","Nová Paka",41},
    {"Náchod","Broumov",42},
    {"Náchod","Jaroměř",42},
    {"Náchod","Náchod",42},
    {"Náchod","Nové Město nad Metují",42},
    {"Rychnov nad Kněžnou","Dobruška",43},
    {"Rychnov nad Kněžnou","Kostelec nad Orlicí",43},
    {"Rychnov nad Kněžnou","Rychnov nad Kněžnou",43},
    {"Trutnov","Dvůr Králové nad Labem",44},
    {"Trutnov","Trutnov",44},
    {"Trutnov","Vrchlabí",44},
    {"Chrudim","Hlinsko",45},
    {"Chrudim","Chrudim",45},
    {"Pardubice","Holice",46},
    {"Pardubice","Pardubice",46},
    {"Pardubice","Přelouč",46},
    {"Svitavy","Litomyšl",47},
    {"Svitavy","Moravská Třebová",47},
    {"Svitavy","Polička",47},
    {"Svitavy","Svitavy",47},
    {"Ústí nad Orlicí","Česká Třebová",48},
    {"Ústí nad Orlicí","Králíky",48},
    {"Ústí nad Orlicí","Lanškroun",48},
    {"Ústí nad Orlicí","Ústí nad Orlicí",48},
    {"Ústí nad Orlicí","Vysoké Mýto",48},
    {"Ústí nad Orlicí","Žamberk",48},
    {"Havlíčkův Brod","Havlíčkův Brod",49},
    {"Havlíčkův Brod","Chotěboř",49},
    {"Havlíčkův Brod","Světlá nad Sázavou",49},
    {"Jihlava","Jihlava",50},
    {"Jihlava","Telč",50},
    {"Pelhřimov","Humpolec",51},
    {"Pelhřimov","Pacov",51},
    {"Pelhřimov","Pelhřimov",51},
    {"Třebíč","Moravské Budějovice",52},
    {"Třebíč","Náměšť nad Oslavou",52},
    {"Třebíč","Třebíč",52},
    {"Žďár nad Sázavou","Bystřice nad Pernštejnem",53},
    {"Žďár nad Sázavou","Nové Město na Moravě",53},
    {"Žďár nad Sázavou","Velké Meziříčí",53},
    {"Žďár nad Sázavou","Žďár nad Sázavou",53},
    {"Blansko","Blansko",54},
    {"Blansko","Boskovice",54},
    {"Brno-město","Brno",55},
    {"Brno-venkov","Ivančice",56},
    {"Brno-venkov","Kuřim",56},
    {"Brno-venkov","Pohořelice",56},
    {"Brno-venkov","Rosice",56},
    {"Brno-venkov","Šlapanice",56},
    {"Brno-venkov","Tišnov",56},
    {"Brno-venkov","Židlochovice",56},
    {"Břeclav","Břeclav",57},
    {"Břeclav","Hustopeče",57},
    {"Břeclav","Mikulov",57},
    {"Hodonín","Hodonín",58},
    {"Hodonín","Kyjov",58},
    {"Hodonín","Veselí nad Moravou",58},
    {"Vyškov","Bučovice",59},
    {"Vyškov","Slavkov u Brna",59},
    {"Vyškov","Vyškov",59},
    {"Znojmo","Moravský Krumlov",60},
    {"Znojmo","Znojmo",60},
    {"Jeseník","Jeseník",61},
    {"Olomouc","Litovel",62},
    {"Olomouc","Olomouc",62},
    {"Olomouc","Šternberk",62},
    {"Olomouc","Uničov",62},
    {"Prostějov","Konice",63},
    {"Prostějov","Prostějov",63},
    {"Přerov","Hranice",64},
    {"Přerov","Lipník nad Bečvou",64},
    {"Přerov","Přerov",64},
    {"Šumperk","Mohelnice",65},
    {"Šumperk","Šumperk",65},
    {"Šumperk","Zábřeh",65},
    {"Bruntál","Bruntál",70},
    {"Bruntál","Krnov",70},
    {"Bruntál","Rýmařov",70},
    {"Frýdek-Místek","Frýdek-Místek",71},
    {"Frýdek-Místek","Frýdlant nad Ostravicí",71},
    {"Frýdek-Místek","Jablunkov",71},
    {"Frýdek-Místek","Třinec",71},
    {"Karviná","Bohumín",72},
    {"Karviná","Český Těšín",72},
    {"Karviná","Havířov",72},
    {"Karviná","Karviná",72},
    {"Karviná","Orlová",72},
    {"Nový Jičín","Bílovec",73},
    {"Nový Jičín","Frenštát pod Radhoštěm",73},
    {"Nový Jičín","Kopřivnice",73},
    {"Nový Jičín","Nový Jičín",73},
    {"Nový Jičín","Odry",73},
    {"Opava","Hlučín",74},
    {"Opava","Kravaře",74},
    {"Opava","Opava",74},
    {"Opava","Vítkov",74},
    {"Ostrava-město","Ostrava",75},
    {"Kroměříž","Bystřice pod Hostýnem",66},
    {"Kroměříž","Holešov",66},
    {"Kroměříž","Kroměříž",66},
    {"Uherské Hradiště","Uherský Brod",67},
    {"Uherské Hradiště","Uherské Hradiště",67},
    {"Vsetín","Rožnov pod Radhoštěm",68},
    {"Vsetín","Valašské Meziříčí",68},
    {"Vsetín","Vsetín",68},
    {"Zlín","Luhačovice",69},
    {"Zlín","Otrokovice",69},
    {"Zlín","Valašské Klobouky",69},
    {"Zlín","Vizovice",69},
    {"Zlín","Zlín",69},
    {"Praha","Praha",76}
};

static constexpr double czinhabitants = 10690000;

#endif // DISTRICTS_HPP
