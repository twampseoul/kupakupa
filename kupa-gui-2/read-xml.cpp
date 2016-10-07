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

	TiXmlDocument readdoc("inputDCE.xml");
	bool loadOkay = readdoc.LoadFile();
	if(!loadOkay)
	{
		cerr << readdoc.ErrorDesc() << endl;
	}
	TiXmlElement* readroot = readdoc.FirstChildElement();
	if(readroot == NULL)
	{
		cerr << "Failed to load file: No root element."
			 << endl;
		readdoc.Clear();
	}
	int ErrorModel;
	string typeOfConection,tcp_cc,udp_bw,delay,httpSize;
	string tcp_mem_user_min,tcp_mem_user_def,tcp_mem_user_max, tcp_mem_server_min,tcp_mem_server_def,tcp_mem_server_max;
	for(TiXmlElement* elem = readroot->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
    string elemName = elem->Value();
		if (elemName=="TypeOfConnection")
		{
			TiXmlNode* e = elem->FirstChild();
			TiXmlText* text = e->ToText();
			typeOfConection = text->Value();
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
		if (elemName=="Delay")
		{
			TiXmlNode* e = elem->FirstChild();
			TiXmlText* text = e->ToText();
			delay = text->Value();
		}
		if (elemName=="ErrorModel")
		{
			TiXmlNode* e = elem->FirstChild();
			TiXmlText* text = e->ToText();
			string ErrorModel=text->Value();
		}
		if (elemName=="SizeOfHttpFile")
		{
			TiXmlNode* e = elem->FirstChild();
			TiXmlText* text = e->ToText();
			httpSize = text->Value();
		}
		if (elemName=="UserMemory")
		{
			tcp_mem_user_min = elem->Attribute("min");
			tcp_mem_user_def = elem->Attribute("default");
			tcp_mem_user_max = elem->Attribute("max");
		}
		if (elemName=="ServerMemory")
		{
			tcp_mem_server_min = elem->Attribute("min");
			tcp_mem_server_def = elem->Attribute("default");
			tcp_mem_server_max = elem->Attribute("max");
		}

		
	}
	string tcp_mem_user = tcp_mem_user_min + ","+tcp_mem_user_def+","+tcp_mem_user_max;
	string tcp_mem_server = tcp_mem_server_min + ","+tcp_mem_server_def+","+tcp_mem_server_max;
}
