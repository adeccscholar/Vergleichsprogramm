/**
 \file
 \brief Hauptdatei für das Leseprojekt
 \details Diese Datei enthält die Funktion "main" des Testprogramms. Hier beginnt und endet die Ausführung des Programms.
 \details Zusätzlich sind noch alle Routinen, Datentypen und Hilfskonstrukte hier
 \author  Volker Hillmann
 \date    09.12.2022
 \copyright Copyright &copy; 2022-2023. adecc Systemhaus GmbH
 \version 1.0
*/


#include "MyProcess.h"
#include "Geodaten.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <locale>

using namespace std::literals;

std::vector<std::tuple<std::string, Location<double>, myMercatorETRS89<double>>> test_coordinates = {
   { "Berlin, Neukölln, Flughafenstraße 44"s,             { 52.481126300, 13.426709160 }, { 1494653.0525, 6887591.0300 } },
   { "Berlin, Kreuberg, Waldemarstraße 19"s,              { 52.504125740, 13.417856830 }, { 1493668.0112, 6891795.6821 } },
   { "Berlin, Altglienicke, Schirnerstraße 10"s,          { 52.413607310, 13.560004810 }, { 1509492.7488, 6875259.6413 } },
   { "Berlin, Steglitz, Spinozastraße 11d"s,              { 52.465269970, 13.306627940 }, { 1481286.8964, 6884693.7256 } },
   { "Berlin, Rudow, Ehrenpreisweg 12"s,                  { 52.426379790, 13.489936020 }, { 1501692.0799, 6877590.2428 } },
   { "Berlin, Biesdorf, Eckermannstraße 120"s,            { 52.524293190, 13.555268650 }, { 1508964.7860, 6895484.5352 } },
   { "Berlin, Lichtenberg, Landsberger Allee 166"s,       { 52.530274610, 13.468511080 }, { 1499307.8579, 6896579.1556 } },
   { "Berlin, Karlshorst, Am Walde 2"s,                   { 52.475621850, 13.510386960 }, { 1503968.4082, 6886583.7992 } },
   { "Berlin, Mitte, Sophienstraße 19"s,                  { 52.525689470, 13.400972300 }, { 1491788.9335, 6895739.6258 } },
   { "Berlin, Reinickendorf, Dambockstraße 40"s,          { 52.604694250, 13.245121960 }, { 1474440.6845, 6910208.8853 } },
   { "Berlin, Prenzlauer Berg, Malmöer Straße 20"s,       { 52.553200760, 13.402762190 }, { 1491988.1250, 6900775.9497 } },
   { "Berlin, Spandau, Heerstraße 359"s,                  { 52.517629600, 13.173104160 }, { 1466422.9390, 6894266.0430 } },
   { "Berlin, Spandau, Gatower Straße 139"s,              { 52.507932270, 13.179993850 }, { 1467189.9873, 6892492.7577 } },
   { "Berlin, Spandau, Gatower Straße 139a"s,             { 52.507950800, 13.180140950 }, { 1467205.8923, 6892496.2200 } },
   { "Berlin, Kladow, Kreutzwaldstraße 3d"s,              { 52.459196610, 13.128127660 }, { 1461415.3565, 6883583.2227 } },
   { "Berlin, Spandau, Aumetzer Weg 8a"s,                 { 52.544082590, 13.131852890 }, { 1461831.2232, 6899106.7709 } },
   { "Brandenburg, Brandenburg, Akazienweg 34a"s,         { 52.373195600, 12.531553600 }, { 1395071.6302, 6867883.2056 } },  // ChatGPT fail, Google
   { "Brandenburg, Potsdam, Rückertstraße 3"s,            { 52.425069200, 12.998004800 }, { 1447217.1807, 6877342.6792 } },
   { "Brandenburg, Kyritz, Bergstraße 1"s,                { 52.950849300, 12.383160500 }, { 1378581.9592, 6973902.6536 } },
   { "Brandenburg, Bad Wilsnack, An der Trift 8"s,        { 52.953796200, 11.947701200 }, { 1330302.1054, 6974450.4095 } },
   { "Brandenburg, Schönefeld, Glasower Allee 5"s,        { 52.379011500, 13.442341800 }, { 1496660.8297, 6868939.0047 } },
   { "Brandenburg, Teltow, Iserstraße 126"s,              { 52.383144400, 13.244641200 }, { 1474600.6891, 6869697.7600 } },
   { "Brandenburg, Bernau, Birkenstraße 15"s,             { 52.650484400, 13.591826500 }, { 1513319.5267, 6918609.4222 } },
   { "Brandenburg, Storkow, Fürstenwalder Straße 55"s,    { 52.275010300, 13.960511600 }, { 1554246.9802, 6849970.4466 } },
   { "Brandenburg, Bad Saarow, Seeallee 31"s,             { 52.239944900, 14.028610500 }, { 1561925.3759, 6843608.3617 } },
   { "Thüringen, Saalfeld/Saale, Stauffenbergstraße 38"s, { 50.653266000, 11.385995100 }, { 1267774.2194, 6560200.9094 } },  // { 32668844.086, 5613998.394 }
   { "Thüringen, Ranis, Lindenstraße 37"s,                { 50.662880000, 11.564565400 }, { 1287652.6084, 6561858.7571 } },  // { 32681428.749, 5615470.388 }
   { "Thüringen, Bad Lobenstein, Poststraße 21"s,         { 50.454684900, 11.637411100 }, { 1295757.9209, 6525395.8957 } },  // { 32687397.563, 5592517.573 }
   { "Thüringen, Rudolstadt, Weinbergstraße 1"s,          { 50.722036900, 11.329881300 }, { 1261518.3643, 6572277.6734 } },  // { 32664631.277, 5621513.36 }
   { "Thüringen, Gera, Schmiedestraße 5"s,                { 50.850573000, 12.025029000 }, { 1338908.9736, 6594901.5238 } },  // { 32713108.831, 5637574.866 }
   { "Thüringen, Bad Köstritz, Am Rosenhügel 60"s,        { 50.928313800, 11.995465000 }, { 1335614.6502, 6608622.7693 } },  // { 32710674.915, 5646133.904 }
   { "Thüringen, Hermsdorf, Schillerstraße 29"s,          { 50.899388200, 11.858601500 }, { 1320378.7811, 6603510.7405 } },  // { 32701183.73, 5642532.995 }
   { "Thüringen, Stadtroda, Hauptstraße 50"s,             { 50.880189900, 11.789461600 }, { 1312763.4475, 6599889.1995 } },  // { 32696460.626, 5640067.11 }
   { "Thüringen, Stadtroda, Bahnhofsweg 1"s,              { 50.881188900, 11.792371600 }, { 1312588.5803, 6600084.4774 } },  // { 32696345.485, 5640185.981 }
   { "Thüringen, Jena, Okenstraße 8"s,                    { 50.922282900, 11.568596000 }, { 1288094.9581, 6607557.6078 } },  // { 32680705.672, 5644330.785 }
   { "Thüringen, Jena, Lucas-Cranach-Allee 1"s,           { 50.929443400, 11.515853000 }, { 1282210.6843, 6608818.5686 } },  // { 32676964.007, 5644996.452 }
   { "Thüringen, Schleiz, Hauptstraße 30"s,               { 50.565980000, 11.901615000 }, { 1325144.8761, 6544844.5559 } },  // { 32705650.068, 5605568.36 }
   { "Thüringen, Greiz, Kirchenring 1"s,                  { 50.613673400, 12.131085100 }, { 1350706.2321, 6553206.6656 } },  // { 32721683.846, 5611531.999 }
   { "Thüringen, Zeulenroda-Triebes, Am Stäudig 13"s,     { 50.642737600, 11.974763100 }, { 1333318.7394, 6558342.5606 } },  // { 32710505.17, 5614329.718 }
   { "Thüringen, Meiningen, Schulgasse 1"s,               { 50.493370000, 10.353983100 }, { 1152934.3638, 6532303.9405 } },  // { 32596248.971, 5594458.272 }
   { "Thüringen, Ilmenau, Kastanienallee 6"s,             { 50.704271300, 11.018474900 }, { 1226870.7580, 6569156.6238 } },  // { 32642718.196, 5618893.154 }
   { "Thüringen, Neuhaus am Rennweg, Oberer Weg 3"s,      { 50.516363300, 11.144990200 }, { 1240925.7661, 6536206.7978 } },  // { 32652237.918, 5598260.611 }
   { "Thüringen, Erfurt, Ludwig-Jahn-Straße 9"s,          { 51.049655100, 11.042637900 }, { 1229536.2354, 6630082.914 } },  // { 32643345.083, 5657337.349 }
   { "Thüringen, Erfurt, Kunemundweg 11"s,                { 51.052289100, 11.037108900 }, { 1228928.0111, 6630554.6886 } },  // { 32642953.948, 5657622.955 }
   { "Thüringen, Weimar, Brehmestraße 4"s,                { 50.988754300, 11.326331800 }, { 1261115.0125, 6619305.3178 } },  // { 32663440.366, 5651156.23 }
   { "Thüringen, Weimar, Buchenhain 4"s,                  { 50.998454900, 11.277165600 }, { 1255681.8514, 6621014.6041 } },  // { 32659982.105, 5652123.678 }
   { "Thüringen, Meuselwitz, Gänsehals 5"s,               { 51.039022200, 12.299616100 }, { 1369501.6539, 6628149.1410 } }, // { 32731511.297, 5659323.398 }
   { "Thüringen, Altenburg, Ahornstraße 6"s,              { 50.982066300, 12.451754700 }, { 1386342.2136, 6618008.1909 } }  // { 32742412.83, 5653438.551 }

   };

int main(int argc, char* argv[]) {
   std::ios_base::sync_with_stdio(false);
   std::cout.setf(std::ios::showpoint);
   std::cout.setf(std::ios::fixed);

   std::string strInput = "D:\\Test"s;
   int iCount = 1;
   if (argc == 2) strInput = argv[1];
   else if(argc == 3) {
      iCount = string_to_int<int>(std::string(argv[1]));
      strInput = argv[2];
      }

  //Rechentest(strInput, iCount);


   std::string strKeyFile = "D:\\Test\\schluessel-th.txt";
   std::string strFile    = "D:\\Test\\Adressen_-_Thüringen.csv";
   std::string strOutput  = "D:\\Test\\Thüringen.txt";
   //auto data = Geodaten::read_keys("D:\\Test\\schluessel-th.txt");
   //std::ofstream ofs(strOutput); // Kontrollieren !!!
  // Geodaten::Read_Th(strKeyFile, strFile, ofs);

   
  
   strKeyFile = "D:\\Test\\schluessel-bb.txt";
   strFile    = "D:\\Test\\Adressen_-_Brandenburg.csv";

   strOutput = "D:\\Test\\Brandenburg.txt";
   std::ofstream ofs(strOutput);
   Geodaten::Read_Bb(strKeyFile, strFile, ofs);
   /*
   for(auto const& [place, location, mercator] : test_coordinates) {
      auto loc = Location<double>::ConvertMercatorETRS89ToWGS84(mercator);
      auto result = Calculate(location, loc);
      std::cout << std::left << std::setw(50) << place << " " 
                << std::setprecision(9) << location
                << " -> " << loc << " = " << std::right 
                << std::setw(10) << std::setprecision(2) << result.first << "m" << std::endl;
      }
   */
   //Compare_Output("e:\\Test", "D:\\Test_Reference");
   
    //t.join();

   #if defined __BORLANDC__
   std::cout << "... press a key ...";
   getchar();
   #endif
   return 0;
}

