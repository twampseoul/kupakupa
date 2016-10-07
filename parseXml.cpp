#include "tinyxml.h"
#include "parseXml.h"
#include <iostream>
#include <utility>
#include <string>
#include <algorithm>
#include <algorithm>

namespace std {

string
ParseInput::GetLowerCase(string stringName) {
std::transform(stringName.begin(), stringName.end(), stringName.begin(), ::tolower);
return stringName;

}

void

ParseInput::parseInputXml(string fileName,char& TypeOfConnection, string& tcp_cc, string& udp_bw, double& SimuTime,bool& downloadMode, double& errRate, double& errRate2,double& k_up,double& pdv_up , double& avg_delay_up,double& k_dw,double& pdv_dw , double& avg_delay_dw,int& ErrorModel, int& ErrorModel2,string& user_bw_down, string& user_bw_up, int& htmlSize,string& tcp_mem_user, string& tcp_mem_user_wmem, string& tcp_mem_user_rmem, string& tcp_mem_server, string& tcp_mem_server_wmem, string& tcp_mem_server_rmem){

// set default value
TypeOfConnection='p';
tcp_cc="reno";
udp_bw="10";

SimuTime=50;
downloadMode=true;
errRate=0.001;

errRate2=0.001;

k_up=3;
pdv_up=2;
avg_delay_up=0;

k_dw=3;
pdv_dw=2;
avg_delay_dw=0;

ErrorModel=1;
ErrorModel2=1;
user_bw_down = "150Mbps";
user_bw_up = "150Mbps";
htmlSize = 2;
tcp_mem_user = "4096 8192 8388608";
tcp_mem_user_wmem = "4096 8192 8388608";
tcp_mem_user_rmem = "4096 8192 8388608";	
tcp_mem_server = "4096 8192 8388608";
tcp_mem_server_wmem = "4096 8192 8388608";
tcp_mem_server_rmem = "4096 8192 8388608";


TiXmlDocument readdoc(fileName.c_str());
bool loadOkay = readdoc.LoadFile();
if(!loadOkay)
{
cerr << readdoc.ErrorDesc() << endl;
}
TiXmlElement* readroot = readdoc.FirstChildElement();
if(readroot == NULL)
{
cerr << "Failed to load file: No root element."<< endl;
readdoc.Clear();
}

string typeOfConectionTmp,httpSizeTmp,modeOperTmp,simuTimeTmp;

string tcp_mem_user_min, tcp_mem_user_def, tcp_mem_user_max;
string tcp_mem_user_wmem_min, tcp_mem_user_wmem_def, tcp_mem_user_wmem_max;
string tcp_mem_user_rmem_min, tcp_mem_user_rmem_def, tcp_mem_user_rmem_max;

string tcp_mem_server_min, tcp_mem_server_def, tcp_mem_server_max;
string tcp_mem_server_wmem_min, tcp_mem_server_wmem_def, tcp_mem_server_wmem_max;
string tcp_mem_server_rmem_min, tcp_mem_server_rmem_def, tcp_mem_server_rmem_max;

for(TiXmlElement* elem = readroot->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
{
string elemName = elem->Value();
if (elemName=="TypeOfConnection")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
typeOfConectionTmp = text->Value();

if (GetLowerCase(typeOfConectionTmp)=="http")
{
TypeOfConnection ='w';
}

if (GetLowerCase(typeOfConectionTmp)=="iperf-udp")
{
TypeOfConnection ='u';
}

if (GetLowerCase(typeOfConectionTmp)=="iperf-tcp")
{

TypeOfConnection ='p';
}
}
if (elemName=="congestionControl")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
tcp_cc = text->Value();
}
if (elemName=="UDPBandwidth")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
udp_bw = text->Value();

}

if (elemName=="SimulationTime")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
simuTimeTmp = text->Value();
SimuTime=atof(simuTimeTmp.c_str());
}

if (elemName=="ModeOperation")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
modeOperTmp = text->Value();
if (GetLowerCase(modeOperTmp)=="download") {
downloadMode = true;
}
if (GetLowerCase(modeOperTmp)=="upload") {
downloadMode = false;
}
}

if (elemName=="ErrorRateUP")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
string errRateTmp2 = text->Value();
errRate2 = atof(errRateTmp2.c_str());
}


if (elemName=="ErrorRateDOWN")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
string errRateTmp = text->Value();
errRate = atof(errRateTmp.c_str());
}


if (elemName=="DelayParamUP")
{	
string kTmp= elem->Attribute("k");
k_up=atof(kTmp.c_str());	
string pdvTmp = elem->Attribute("pdv");
pdv_up=atof(pdvTmp.c_str());
string delayTmp = elem->Attribute("avg_delay");
avg_delay_up=atof(delayTmp.c_str());

}
if (elemName=="DelayParamDOWN")
{	
string kTmp= elem->Attribute("k");
k_dw=atof(kTmp.c_str());	
string pdvTmp = elem->Attribute("pdv");
pdv_dw=atof(pdvTmp.c_str());
string delayTmp = elem->Attribute("avg_delay");
avg_delay_dw=atof(delayTmp.c_str());
}

if (elemName=="UserBandwidthDown")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
user_bw_down = text->Value();
}
if (elemName=="UserBandwidthUp")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
user_bw_up = text->Value();
}
if (elemName=="ErrorModelUP")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
string ErrorModelTmp2=text->Value();
ErrorModel2 = atoi(ErrorModelTmp2.c_str());
}	
if (elemName=="ErrorModelDOWN")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
string ErrorModelTmp=text->Value();
ErrorModel = atoi(ErrorModelTmp.c_str());
}

if (elemName=="SizeOfHttpFile")
{
TiXmlNode* e = elem->FirstChild();
TiXmlText* text = e->ToText();
httpSizeTmp = text->Value();
htmlSize=atoi(httpSizeTmp.c_str());	
}

if (elemName=="UserTCPMem")
{
tcp_mem_user_min = elem->Attribute("min");
tcp_mem_user_def = elem->Attribute("default");
tcp_mem_user_max = elem->Attribute("max");
}
if (elemName=="UserWmem")
{
tcp_mem_user_wmem_min = elem->Attribute("min");
tcp_mem_user_wmem_def = elem->Attribute("default");
tcp_mem_user_wmem_max = elem->Attribute("max");
}
if (elemName=="UserRmem")
{
tcp_mem_user_rmem_min = elem->Attribute("min");
tcp_mem_user_rmem_def = elem->Attribute("default");
tcp_mem_user_rmem_max = elem->Attribute("max");
}

if (elemName=="ServerTCPMem")
{
tcp_mem_server_min = elem->Attribute("min");
tcp_mem_server_def = elem->Attribute("default");
tcp_mem_server_max = elem->Attribute("max");
}
if (elemName=="ServerWmem")
{
tcp_mem_server_wmem_min = elem->Attribute("min");
tcp_mem_server_wmem_def = elem->Attribute("default");
tcp_mem_server_wmem_max = elem->Attribute("max");
}
if (elemName=="ServerRmem")
{
tcp_mem_server_rmem_min = elem->Attribute("min");
tcp_mem_server_rmem_def = elem->Attribute("default");
tcp_mem_server_rmem_max = elem->Attribute("max");
}


}
tcp_mem_user = tcp_mem_user_min +"," +tcp_mem_user_def+ "," +tcp_mem_user_max;
tcp_mem_user_wmem = tcp_mem_user_wmem_min+ "," +tcp_mem_user_wmem_def+ "," +tcp_mem_user_wmem_max;
tcp_mem_user_rmem = tcp_mem_user_rmem_min+ "," +tcp_mem_user_rmem_def+ "," +tcp_mem_user_rmem_max;

tcp_mem_server = tcp_mem_server_min + ","+tcp_mem_server_def+","+tcp_mem_server_max;
tcp_mem_server_wmem = tcp_mem_server_wmem_min+","+tcp_mem_server_wmem_def+","+tcp_mem_server_wmem_max;
tcp_mem_server_rmem = tcp_mem_server_rmem_min+","+tcp_mem_server_rmem_def+","+tcp_mem_server_rmem_max;
      
      
}
}
