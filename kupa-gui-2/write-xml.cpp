// basic file operations
#include <iostream>
#include <string>

#include <string>
#include <sstream>
#include <map>
#include "generate-html-file.h"
#include "tinyxml.h"

typedef std::map<std::string,std::string> MessageMap;
using namespace std;

int main () {

    string typeOfConection="tcp";
	string tcp_cc = "reno";
	string tcp_mem_user_min = "4096";
	string tcp_mem_user_def = "8192";
	string tcp_mem_user_max = "8388608";

	string tcp_mem_server_min = "4096";
	string tcp_mem_server_def = "8192";
	string tcp_mem_server_max = "8388608";

	string udp_bw="1m";
	string delay = "2ms";
	int ErrorModel = 1;
	string http = "1";
	
 // Make xml: <?xml ..><Hello>World</Hello>
	TiXmlDocument doc;
	TiXmlElement * msg;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	TiXmlElement * root = new TiXmlElement( "SimulationParameters" );  
	doc.LinkEndChild( root ); 

	TiXmlElement * typOfCon = new TiXmlElement( "TypeOfConnection" );  
	root->LinkEndChild( typOfCon );
	typOfCon->LinkEndChild( new TiXmlText( typeOfConection.c_str() ));

	TiXmlElement * cc = new TiXmlElement( "congestionControl" ); 
	root->LinkEndChild( cc );
	cc->LinkEndChild( new TiXmlText( tcp_cc.c_str() ) );

	TiXmlElement * udpBand = new TiXmlElement( "UDPBandwidth" ); 
	root->LinkEndChild( udpBand );
	udpBand->LinkEndChild( new TiXmlText( udp_bw.c_str() ) );

	TiXmlElement * delayPar = new TiXmlElement( "Delay" ); 
	root->LinkEndChild( delayPar );
	delayPar->LinkEndChild( new TiXmlText( delay.c_str() ) );

	TiXmlElement * errMod = new TiXmlElement( "ErrorModel" ); 
	root->LinkEndChild( errMod );
	string cerrMod = to_string(ErrorModel);
	errMod->LinkEndChild( new TiXmlText( cerrMod.c_str() ) );

	TiXmlElement * httpSize = new TiXmlElement( "SizeOfHttpFile" ); 
	root->LinkEndChild( httpSize );
	httpSize->LinkEndChild( new TiXmlText( http.c_str() ) );

	TiXmlElement * userMem = new TiXmlElement( "UserMemory" );
	root->LinkEndChild( userMem );
	userMem->SetAttribute("min", tcp_mem_user_min.c_str());
	userMem->SetAttribute("default", tcp_mem_user_def.c_str());
	userMem->SetAttribute("max", tcp_mem_user_max.c_str());

	TiXmlElement * serverMem = new TiXmlElement( "ServerMemory" );
	root->LinkEndChild( serverMem );
	serverMem->SetAttribute("min", tcp_mem_server_min.c_str());
	serverMem->SetAttribute("default", tcp_mem_server_def.c_str());
	serverMem->SetAttribute("max", tcp_mem_server_max.c_str());
	doc.SaveFile( "inputDCE.xml" );

}
