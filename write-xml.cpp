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

string typeOfConection="iperf-tcp";
string tcp_cc = "reno";
string udp_bw="1m";
string user_bw = "150Mbps";
string server_bw = "10Gbps";
string delay = "2ms";
int ErrorModel = 1;
string http = "1";
string errRate = "0.01";
string modeOperation = "download";

string jitter ="1";
string alpha = "0.3";
string k = "100";
string tetha = "2";

string tcp_mem_user_min = "4096";
string tcp_mem_user_def = "8192";
string tcp_mem_user_max = "8388608";

string tcp_mem_server_min = "4096";
string tcp_mem_server_def = "8192";
string tcp_mem_server_max = "8388608";



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

TiXmlElement * modeOp = new TiXmlElement( "ModeOperation" );
root->LinkEndChild( modeOp );
modeOp->LinkEndChild( new TiXmlText( modeOperation.c_str() ) );

TiXmlElement * errR = new TiXmlElement( "ErrorRate" );
root->LinkEndChild( errR );
errR->LinkEndChild( new TiXmlText( errRate.c_str() ) );

TiXmlElement * jittParam = new TiXmlElement( "JitterParam" );
root->LinkEndChild( jittParam );
jittParam->SetAttribute("jitter", jitter.c_str());
jittParam->SetAttribute("alpha", alpha.c_str());
jittParam->SetAttribute("k", k.c_str());
jittParam->SetAttribute("tetha", tetha.c_str());

TiXmlElement * usr_band = new TiXmlElement( "UserBandwidth" );
root->LinkEndChild( usr_band  );
usr_band ->LinkEndChild( new TiXmlText( user_bw.c_str() ) );

TiXmlElement * svr_band = new TiXmlElement( "ServerBandwidth" );
root->LinkEndChild( svr_band  );
svr_band->LinkEndChild( new TiXmlText( server_bw.c_str() ) );

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
