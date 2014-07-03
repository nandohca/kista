/*****************************************************************************

  prueba.cpp
  map example

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/
     
using namespace std;

#include <systemc.h>

#include <map>


int sc_main (int, char *[]) {

   typedef map<string, int> mimap_t;
   mimap_t mimap;
   mimap_t::iterator it;

   string key;
   int value;

   mimap["nando"]=37;
   mimap["maite"]=36;
   mimap["berna"]=35;

   cout << "Hola mundo" << endl;
/*
   for(it = mimap.begin(); it != mimap.end(); ++it )
    {
      key = it->first;
      value = it->second;

      cout << key << " : " << value << endl;

      if(it==mimap.end()) cout << "TRUE" << endl;
      else cout << "FALSE" << endl;

    }
*/
   for(unsigned int i=0; i<3 ; i++) {
   	for(it = mimap.begin(); it != mimap.end(); ++it )
    	{
      	key = it->first;
      	value = it->second;

	cout << key << " : " << value << endl;

	if(it==mimap.end()) cout << "TRUE" << endl;
      	else cout << "FALSE" << endl;

    	}
	if(it==mimap.end()) cout << "OL:TRUE" << endl;
      	else cout << "OL:FALSE" << endl;
    }

/*

    if(it==mimap.end()) cout << "TRUE" << endl;
    else cout << "FALSE" << endl;

    it = mimap.begin();

    key = it->first;
    value = it->second;
    cout << key << " : " << value << endl;

    ++it;

    if(it==mimap.end()) cout << "TRUE" << endl;
    else cout << "FALSE" << endl;

    key = it->first;
    value = it->second;
    cout << key << " : " << value << endl;

    ++it;

    if(it==mimap.end()) cout << "TRUE" << endl;
    else cout << "FALSE" << endl;
		


//    it = mimap.end();

    key = it->first;
    value = it->second;
    cout << key << " : " << value << endl;
*/
/*
   for(unsigned int i=0; i<10; i++)
    {

      if(it==mimap.end()) {
	it=mimap.begin();
        cout << "Retorno al final" << endl;
      } else {
        ++it;
        cout << "suma y sigue" << endl;
      }

      key = it->first;
      value = it->second;

      cout << key << " : " << value << endl;

    }

*/

/*
   for( auto it = mimap.end(); it != mimap.begin(); --it )
    {
      string key = it->first;
      int value = it->second;

      cout << key << " : " << value << endl;
    }
*/
   
   return 0;
   
}
