#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/dce-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <iostream>
#include <map>
#include "parseXml.h"

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("kupakupa");

//function to turn interger to string for iperf input
string DoubleToString (double a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}


void GenerateHtmlFile (int fileSize)
{
	std::ofstream myfile;
	myfile.open ("files-5/index.html");

	myfile << "<http>\n <body>\n";
	std::vector<char> empty(1024, 0);

	for(int i = 0; i < 1024*fileSize; i++)
	{
		if (!myfile.write(&empty[0], empty.size()))
		{
			std::cerr << "problem writing to file" << std::endl;
		}
	}

	myfile << "Dummy html file\n";
	myfile << "</body>\n</http>\n";
	myfile.close();
}

//fungtion to get the last value in tcp_mem for tcp_mem_max
string SplitLastValue (const std::string& str)
{
  //std::cout << "Splitting: " << str << '\n';
  unsigned found = str.find_last_of(" ");
  ostringstream temp;
  temp << str.substr(found+1);
  return temp.str();
}
//to remove comma if user make input with comma
string RemoveComma (std::string& str) 
{
//std::cout << "test : " << str << '\n';
int i = 0;
std::string str2=str;
for (i=0; i<3; i++)
{
	
	std::size_t found=str.find(',');
	if (found!=std::string::npos) {
	str2 = str.replace(str.find(','),1," ");
	} else {
	//std::cout<<"no comma found.."<<std::endl;
	}
}
return str2;

}

static void RunIp (Ptr<Node> node, Time at, std::string str)
{
  DceApplicationHelper process;
  ApplicationContainer apps;
  process.SetBinary ("ip");
  process.SetStackSize (1 << 16);
  process.ResetArguments ();
  process.ParseArguments (str.c_str ());
  apps = process.Install (node);
  apps.Start (at);
}

double FindPk (double k)
{
//read pk.txt and store it in 50x1 matrix
    ifstream in;
    in.open("pk.txt");

  if(!in) {
    cout << "Cannot open input file.\n";
    return 1;
  }

  char str[255];
  double pkval[50];
  char pkchar[15];
  string buff;
  int pkcount=0;
  while(std::getline(in, buff)) {
    for (int a=0; a!=15; a++){
        pkchar[a]=buff[a+6];
    }
    pkval[pkcount]=atof(pkchar);
    pkcount=pkcount+1;
  }
  in.close();

  int y = static_cast<int>(k*10);
  double pk=pkval[(y-1)];
  //std::cout << "pk " << pk << std::endl;
  return pk;
}





void
PrintTcpFlags (std::string key, std::string value)
{
  NS_LOG_INFO (key << "=" << value);
}

int main (int argc, char *argv[])
{
	double errRate = 0.001;
	double errRate2 = 0.001;
	std::string tcp_cc = "reno";
	std::string tcp_mem_user = "4096 8192 8388608";
	std::string tcp_mem_user_wmem = "4096 8192 8388608";
	std::string tcp_mem_user_rmem = "4096 8192 8388608";
	
	std::string tcp_mem_server = "4096 8192 8388608";
	std::string tcp_mem_server_wmem = "4096 8192 8388608";
	std::string tcp_mem_server_rmem = "4096 8192 8388608";

	std::string udp_bw="1";
	
	std::string user_bw_down = "150Mbps";
	std::string user_bw_up = "150Mbps";
	
	double k_up = 0;
	double pdv_up = 0;
	double avg_delay_up=1;
	
	double k_dw = 0;
	double pdv_dw = 0;
	double avg_delay_dw=1;
	
	int monitor = 1;
	int mode = 0;
	
       int ErrorModel = 1;
       int ErrorModel2 = 1;
       double SimuTime = 50;
       int htmlSize = 2; // in mega bytes
       char TypeOfConnection = 'p'; // iperf tcp connection
       bool downloadMode = true;
       bool inputFromXml = false;
     
      	/*
	unsigned int chan_jitter = 1;
	double chan_alpha = 0.3;
   	double chan_theta = 2;
	double chan_k = 5;
        */
      CommandLine cmd;
      
      cmd.AddValue ("inputFromXml", "flag for reading input from xml file",inputFromXml);
      cmd.AddValue ("TypeOfConnection", "Link type: p for iperf-tcp, u for iperf-udp and w for wget-thttpd, default to iperf-tcp", TypeOfConnection);
      cmd.AddValue ("ModeOperation", "If true it's download mode for UE, else will do upload. http will always do download", downloadMode);
      cmd.AddValue ("tcp_cc", "TCP congestion control algorithm. Default is reno. Other options: bic, cubic, highspeed, htcp, hybla, illinois, lp, probe, scalable, vegas, veno, westwood, yeah", tcp_cc);
      cmd.AddValue ("tcp_mem_user", "put 3 values (min, default, max) separaed by comma for tcp_mem in user, range 4096-16000000", 		     tcp_mem_user);
      
      cmd.AddValue ("tcp_mem_user_wmem", "put 3 values (min, default, max) separaed by comma for tcp_mem in user, range 4096-16000000", 		     tcp_mem_user_wmem);
      
      cmd.AddValue ("tcp_mem_user_rmem", "put 3 values (min, default, max) separaed by comma for tcp_mem in user, range 4096-16000000", 		     tcp_mem_user_rmem);
      
      cmd.AddValue ("tcp_mem_server", "put 3 values (min, default, max) separaed by comma for tcp_mem in server, range 4096-54000000", tcp_mem_server);
      
     cmd.AddValue ("tcp_mem_server_wmem", "put 3 values (min, default, max) separaed by comma for tcp_mem in server, range 4096-54000000", tcp_mem_server_wmem);
            
     cmd.AddValue ("tcp_mem_server_rmem", "put 3 values (min, default, max) separaed by comma for tcp_mem in server, range 4096-54000000", tcp_mem_server_rmem);
                  
      cmd.AddValue ("user_bw_down", "bandwidth between user and BS, in Mbps. Default is 150", user_bw_down);
      cmd.AddValue ("user_bw_up", "bandwidth between server and BS, in Gbps. Default is 10", user_bw_up);

     
     cmd.AddValue ("errRate", "download Error rate.", errRate);
     cmd.AddValue ("errRate2", "upload Error rate.", errRate2);
     cmd.AddValue ("ErrorModel", "Choose error model you want to use. options: 1 -rate error model-default, 2 - burst error model", ErrorModel);
     cmd.AddValue ("ErrorModel2", "Choose error model you want to use. options: 1 -rate error model-default, 2 - burst error model", ErrorModel);
     cmd.AddValue ("udp_bw","banwidth set for UDP, default is 1M", udp_bw);
     cmd.AddValue ("htmlSize","banwidth set for UDP, default is 1M", htmlSize);
     cmd.AddValue ("SimuTime", "time to do the simulaton, in second", SimuTime);

    cmd.AddValue ("avg_delay_up", "average delay upstream.", avg_delay_up);
    cmd.AddValue ("delay_pdv_up", "theta for normal random distribution in server-BS conection upstream", pdv_up);
    cmd.AddValue ("chan_k_up", " Normal random distribution k in server-BS conection upstream", k_up);
    
    cmd.AddValue ("avg_delay_dw", "average delay downstream.", avg_delay_dw);
    cmd.AddValue ("delay_pdv_dw", "theta for normal random distribution in server-BS conection downstream", pdv_dw);
    cmd.AddValue ("chan_k_dw", " Normal random distribution k in server-BS conection downstream", k_dw);
	
     cmd.Parse (argc, argv);     
      

      if (inputFromXml)
      {
	string fileName = "inputDCE.xml";	
	ParseInput parser;

    parser.parseInputXml(fileName,TypeOfConnection,tcp_cc,udp_bw,SimuTime,downloadMode,errRate, errRate2,k_up, pdv_up, avg_delay_up, k_dw, pdv_dw, avg_delay_dw, ErrorModel, ErrorModel2, user_bw_down, user_bw_up, htmlSize,tcp_mem_user, tcp_mem_user_wmem, tcp_mem_user_rmem, tcp_mem_server, tcp_mem_server_wmem, tcp_mem_server_rmem);

	}
      	  TypeOfConnection = tolower (TypeOfConnection);
	  switch (TypeOfConnection)
	    {
	    case 'u': //iperf udp connection
	    	std::cout << "IPERF-UDP connection is selected" << std::endl;
	    	break;
	    case 'w': //thttpd - wget connection, always in download mode
	        std::cout << "HTTP connection is selected" << std::endl;
	      break;
	     case 'p': //thttpd - wget connection, always in download mode
	        std::cout << "IPERF-TCP connection is selected" << std::endl;
	      break;
	    default:
	      std::cout << "Unknown link type : " << TypeOfConnection << " ?" << std::endl;
	      //return 1;
    	     }
    	     
    	     
     
// calculating theta and delay
double delay_up;
double theta_up;
double pk_up = FindPk(k_up);

theta_up = pdv_up/pk_up;
delay_up = avg_delay_up-k_up*theta_up;

	if (delay_up < 0) {
		std::cout << "IMPOSIBLE DELAY ABORT SIMULATION" << std::endl;
		std::cout << "UPSTREAM" << std::endl;
		std::cout << "calculated theta " << theta_up << std::endl;
		std::cout << "calculater node processing time " << delay_up << std::endl;
		return 0;
	}


double delay_dw;
double theta_dw;
double pk_dw = FindPk(k_dw);

theta_dw = pdv_dw/pk_dw;
delay_dw = avg_delay_dw-k_dw*theta_dw;

	if (delay_dw < 0) {
		std::cout << "IMPOSIBLE DELAY ABORT SIMULATION" << std::endl;
		std::cout << "DOWNSTREAM" << std::endl;
		std::cout << "calculated theta " << theta_dw << std::endl;
		std::cout << "calculater node processing time " << delay_dw << std::endl;
		return 0;
	}

// topologies
    std::cout << "Building topologies.." << std::endl;
    NS_LOG_INFO ("Create nodes.");
    NodeContainer mobile,BS,core,router;
    mobile.Create(1);
    router.Create(2);
    BS.Create(2);
    core.Create (1);
    NodeContainer mobileRouter = NodeContainer (mobile.Get (0), router.Get (0));
    NodeContainer routerBSDown = NodeContainer (router.Get (0), BS.Get (0));
    NodeContainer routerBSUp = NodeContainer (router.Get (0), BS.Get (1));
    NodeContainer BSRouterDown = NodeContainer (BS.Get (0),router.Get (1));
    NodeContainer BSRouterUp = NodeContainer (BS.Get (1), router.Get (1));
    NodeContainer routerCore = NodeContainer (router.Get (1), core.Get (0));
    
    DceManagerHelper dceManager;

    std::cout << "Setting memory size.." << std::endl;
        //setting memory size for user and server
        
#ifdef KERNEL_STACK
    dceManager.SetNetworkStack ("ns3::LinuxSocketFdFactory", "Library", StringValue ("liblinux.so"));
    LinuxStackHelper stack;
    LinuxStackHelper routerStack;
    stack.Install (mobile);
    routerStack.Install (router);
    stack.Install (BS);
    stack.Install (core);
    dceManager.Install (mobile);
    dceManager.Install (router);
    dceManager.Install (BS);
    dceManager.Install (core);
    
    	//let's remove the comma
	tcp_mem_user = RemoveComma(tcp_mem_user);
	tcp_mem_user_wmem = RemoveComma(tcp_mem_user_wmem);
	tcp_mem_user_rmem = RemoveComma(tcp_mem_user_rmem);
	
	tcp_mem_server = RemoveComma(tcp_mem_server);
	tcp_mem_server_wmem = RemoveComma(tcp_mem_server_wmem);
	tcp_mem_server_rmem = RemoveComma(tcp_mem_server_rmem);
	
	//assume coma has been removed
	std::string tcp_mem_user_max_wmem = SplitLastValue(tcp_mem_server_wmem);
	std::string tcp_mem_user_max_rmem = SplitLastValue(tcp_mem_server_rmem);
	
	std::string tcp_mem_server_max_wmem = SplitLastValue(tcp_mem_server_wmem);
	std::string tcp_mem_server_max_rmem = SplitLastValue(tcp_mem_server_rmem);
	
	if (TypeOfConnection=='w')
	{
	    std::cout << "Generating html file with size =" << htmlSize <<"Mbytes" << std::endl;
	    mkdir ("files-5",0744);
	    GenerateHtmlFile(htmlSize);
	    SimuTime=100;
	    if (htmlSize*1000 > atoi(tcp_mem_user_max_wmem.c_str())){
	    
		double tmp2=atof(tcp_mem_user_max_wmem.c_str())/(htmlSize*1000);
		SimuTime = (htmlSize*10)/(tmp2)*1.5*(htmlSize/tmp2);
		
	    }

	}
		
    std::string IperfTime = DoubleToString(SimuTime);	
	
    stack.SysctlSet (mobile.Get(0), ".net.ipv4.tcp_mem", tcp_mem_user);
    stack.SysctlSet (mobile.Get(0), ".net.ipv4.tcp_wmem", tcp_mem_user_wmem);
    stack.SysctlSet (mobile.Get(0), ".net.ipv4.tcp_rmem", tcp_mem_user_rmem);
    stack.SysctlSet (mobile.Get(0), ".net.core.wmem_max", tcp_mem_user_max_wmem);
    stack.SysctlSet (mobile.Get(0), ".net.core.rmem_max", tcp_mem_user_max_rmem);
    stack.SysctlSet (mobile.Get(0), ".net.core.netdev_max_backlog", "250000");
        
    stack.SysctlSet (core.Get(0), ".net.ipv4.tcp_mem", tcp_mem_server);
    stack.SysctlSet (core.Get(0), ".net.ipv4.tcp_wmem", tcp_mem_server_wmem);
    stack.SysctlSet (core.Get(0), ".net.ipv4.tcp_rmem", tcp_mem_server_rmem);
    stack.SysctlSet (core.Get(0), ".net.core.wmem_max", tcp_mem_server_max_wmem);
    stack.SysctlSet (core.Get(0), ".net.core.rmem_max", tcp_mem_server_max_rmem);
    stack.SysctlSet (core.Get(0), ".net.core.netdev_max_backlog", "250000");
    
    stack.SysctlSet (mobile, ".net.ipv4.tcp_congestion_control", tcp_cc);
    stack.SysctlSet (BS, ".net.ipv4.tcp_congestion_control", tcp_cc);
    stack.SysctlSet (core, ".net.ipv4.tcp_congestion_control", tcp_cc);
    stack.SysctlSet (router, ".net.ipv4.tcp_congestion_control", tcp_cc);

#else
    NS_LOG_ERROR ("Linux kernel stack for DCE is not available. build with dce-linux module.");
    //silently exit
    return 0;
#endif
	
    std::cout << "Setting link.." << std::endl;


if (downloadMode) {
	std::cout << "Download mode is used "<< std::endl;
	mode = 0;
}
if (!downloadMode) {
	std::cout << "Upload mode is used "<< std::endl;
	mode = 1;
}

// channel for mobile router to BS
	NS_LOG_INFO ("Create channels.");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (user_bw_down));
	p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
	NetDeviceContainer chanRouterBSDown = p2p.Install (routerBSDown);
	
	p2p.SetDeviceAttribute ("DataRate", StringValue (user_bw_up));
	p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
	NetDeviceContainer chanRouterBSUp = p2p.Install (routerBSUp);



std::ostringstream delay_oss;
delay_oss.str ("");
delay_oss << delay_dw <<"ms";
//std::cout << "delay ="<<delay_oss.str () <<std::endl;

//channel for core router to BS
p2p.SetDeviceAttribute ("DataRate", StringValue ("200Gbps"));
p2p.SetChannelAttribute ("Delay", StringValue (delay_oss.str ().c_str ()));
p2p.SetChannelAttribute ("Jitter", UintegerValue (1));
p2p.SetChannelAttribute ("k", DoubleValue (k_dw));
p2p.SetChannelAttribute ("transparent", UintegerValue (0));
p2p.SetChannelAttribute ("theta", DoubleValue (theta_dw));
NetDeviceContainer chanBSRouterDown = p2p.Install (BSRouterDown);

delay_oss.str ("");
delay_oss << delay_up <<"ms";
//std::cout << "delay ="<<delay_oss.str () <<std::endl;

p2p.SetDeviceAttribute ("DataRate", StringValue ("200Gbps"));
p2p.SetChannelAttribute ("Delay", StringValue (delay_oss.str ().c_str ()));
p2p.SetChannelAttribute ("Jitter", UintegerValue (1));
p2p.SetChannelAttribute ("k", DoubleValue (k_up));
p2p.SetChannelAttribute ("transparent", UintegerValue (0));
p2p.SetChannelAttribute ("theta", DoubleValue (theta_up));
NetDeviceContainer chanBSRouterUp = p2p.Install (BSRouterUp);

// channel for mobile router to mobile and mobile router to core
	p2p.SetDeviceAttribute ("DataRate", StringValue ("200Gbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
	p2p.SetChannelAttribute ("transparent", UintegerValue (1));
	p2p.SetChannelAttribute ("coreRouter", UintegerValue (0));
	p2p.SetChannelAttribute ("monitor", UintegerValue (monitor));
	p2p.SetChannelAttribute ("mode",UintegerValue (mode) );
	NetDeviceContainer chanMobileRouter = p2p.Install (mobileRouter);
	
	p2p.SetDeviceAttribute ("DataRate", StringValue ("200Gbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
	p2p.SetChannelAttribute ("transparent", UintegerValue (1));
	p2p.SetChannelAttribute ("coreRouter", UintegerValue (1));
	p2p.SetChannelAttribute ("monitor", UintegerValue (monitor));
	p2p.SetChannelAttribute ("mode",UintegerValue (mode) );
	NetDeviceContainer chanRouterCore = p2p.Install (routerCore);
	

//error model options

	/*strangely, if em is not set at the begining, it doesn't want to compile.
	therefore, i just put it here as a default object and to make sure it can
	be build properly*/
	
	
	Ptr<RateErrorModel> em = CreateObjectWithAttributes<RateErrorModel> (
			    "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0,Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate),
			    "ErrorUnit", EnumValue (RateErrorModel::ERROR_UNIT_PACKET)
			    );
	std::cout << "Building error model..." <<std::endl;

	if (ErrorModel == 1)
	{
		std::cout << "Rate Error Model is selected"<<std::endl;
		Ptr<RateErrorModel> em = CreateObjectWithAttributes<RateErrorModel> (
			    "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0,Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate),
			    "ErrorUnit", EnumValue (RateErrorModel::ERROR_UNIT_PACKET)
			    );
		std::cout << "Building error model completed" <<std::endl;
	}
	else if (ErrorModel==2)
	{
		std::cout << "Burst Error Model is selected" <<std::endl;
		Ptr<BurstErrorModel> em = CreateObjectWithAttributes<BurstErrorModel> (
			    "BurstSize", StringValue ("ns3::UniformRandomVariable[Min=1,Max=4]"),
			    "BurstStart", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate)
			    );
		std::cout << "Building error model completed" <<std::endl;
	}
	else
	{
		//this will not change the error model
		std::cout << "Unknown download error model. Restore to default: rate error model" <<std::endl;
	}

	Ptr<RateErrorModel> em2 = CreateObjectWithAttributes<RateErrorModel> (
			    "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0,Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate2),
			    "ErrorUnit", EnumValue (RateErrorModel::ERROR_UNIT_PACKET)
			    );
	std::cout << "Building error model..." <<std::endl;

	if (ErrorModel2 == 1)
	{
		std::cout << "Rate Error Model is selected"<<std::endl;
		Ptr<RateErrorModel> em2 = CreateObjectWithAttributes<RateErrorModel> (
			    "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0,Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate2),
			    "ErrorUnit", EnumValue (RateErrorModel::ERROR_UNIT_PACKET)
			    );
		std::cout << "Building error model completed" <<std::endl;
	}
	else if (ErrorModel2 ==2)
	{
		std::cout << "Burst Error Model is selected" <<std::endl;
		Ptr<BurstErrorModel> em2 = CreateObjectWithAttributes<BurstErrorModel> (
			    "BurstSize", StringValue ("ns3::UniformRandomVariable[Min=1,Max=4]"),
			    "BurstStart", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
			    "ErrorRate", DoubleValue (errRate2)
			    );
		std::cout << "Building error model completed" <<std::endl;
	}
	else
	{
		//this will not change the error model
		std::cout << "Unknown upload error model. Restore to default: rate error model" <<std::endl;
	}


// IP Address
    NS_LOG_INFO ("Assign IP Addresses.");
    std::cout << "Setting IP addresses" << std::endl;
    Ipv4AddressHelper ipv4;
    //for router mobile and BS net devices
        ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    	Ipv4InterfaceContainer IPRouterBSDown = ipv4.Assign (chanRouterBSDown);

    	ipv4.SetBase ("10.2.1.0", "255.255.255.0");
    	Ipv4InterfaceContainer IPRouterBSUp = ipv4.Assign (chanRouterBSUp);
    	
        //for router core and BS devices
        ipv4.SetBase ("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer IPBSRouterDown = ipv4.Assign (chanBSRouterDown);
        
        ipv4.SetBase ("10.2.2.0", "255.255.255.0");
        Ipv4InterfaceContainer IPBSRouterUp= ipv4.Assign (chanBSRouterUp);

        // for router to mobile and core
        ipv4.SetBase ("10.9.1.0", "255.255.255.0");
        Ipv4InterfaceContainer IPMobileRouter = ipv4.Assign (chanMobileRouter);
        
        ipv4.SetBase ("10.9.2.0", "255.255.255.0");
        Ipv4InterfaceContainer IPRouterCore = ipv4.Assign (chanRouterCore);
        
// Create router nodes, initialize routing database and set up the routing tables in the nodes.
    std::cout << "Creating routing table" << std::endl;
    std::ostringstream cmd_oss;
    	
    //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    // setup ip routes
    // mobile
	cmd_oss.str ("");
	cmd_oss << "route add "<< "10.9.2.2"<<"/255.255.255.255" <<" via " <<"10.9.1.2";
	LinuxStackHelper::RunIp (mobile.Get (0), Seconds (0.1), cmd_oss.str ().c_str ());
    // core
       cmd_oss.str ("");
       cmd_oss << "route add "<< "10.9.1.1"<<"/255.255.255.255" <<" via " <<"10.9.2.1";
       LinuxStackHelper::RunIp (core.Get (0), Seconds (0.1), cmd_oss.str ().c_str ());
       
	
        // mobile router
	    cmd_oss.str ("");
	    cmd_oss << "route add "<< "10.9.2.2"<<"/255.255.255.255" <<" via " <<"10.2.1.2";
	    LinuxStackHelper::RunIp (router.Get (0), Seconds (0.1), cmd_oss.str ().c_str ());
	    
	// core router
	    cmd_oss.str ("");
	    cmd_oss << "route add "<< "10.9.1.1"<<"/255.255.255.255" <<" via " <<"10.1.2.1";
	    LinuxStackHelper::RunIp (router.Get (1), Seconds (0.1), cmd_oss.str ().c_str ());
	    
        // BS DOWNLINK
	    cmd_oss.str ("");
	    cmd_oss << "route add "<< "10.9.1.1"<<"/255.255.255.255" <<" via " <<"10.1.1.1";
	    LinuxStackHelper::RunIp (BS.Get (0), Seconds (0.1), cmd_oss.str ().c_str ());
	    
        // BS UPLINK
	    cmd_oss.str ("");
	    cmd_oss << "route add "<< "10.9.2.2"<<"/255.255.255.255" <<" via " <<"10.2.2.2";
	    LinuxStackHelper::RunIp (BS.Get (1), Seconds (0.1), cmd_oss.str ().c_str ());
	    


#ifdef KERNEL_STACK
    LinuxStackHelper::PopulateRoutingTables ();
#endif


// Application
    NS_LOG_INFO ("Create Applications.");
    std::cout << "Creating Applications.." << std::endl;
    DceApplicationHelper dce;

	dce.SetStackSize (1 << 20);
	int EndTime = 2*SimuTime;;
	
	if (EndTime<=100) {
	   EndTime=100;	
	}
	
  switch (TypeOfConnection)
    {
    case 'p':
      {
        if (downloadMode)
            {
            chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));
            // Launch iperf server on node 0 (mobile device)
            
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-s");
            dce.AddArgument ("-P");
            dce.AddArgument ("1");
            ApplicationContainer SerApps0 = dce.Install (mobile.Get (0));
            SerApps0.Start (Seconds (1));
            //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

            // Launch iperf client on node 2
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-c");
            dce.AddArgument ("10.9.1.1");
            dce.AddArgument ("-i");
            dce.AddArgument ("1");
            dce.AddArgument ("--time");
            dce.AddArgument (IperfTime);
            ApplicationContainer ClientApps0 = dce.Install (core.Get (0));
            ClientApps0.Start (Seconds (1));
            //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
        }
            else
            {
            chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));
                // Launch iperf server on node 2
                // server will receive tcp message
                dce.SetBinary ("iperf");
                dce.ResetArguments ();
                dce.ResetEnvironment ();
                dce.AddArgument ("-s");
                dce.AddArgument ("-P");
                dce.AddArgument ("1");
                ApplicationContainer SerApps0 = dce.Install (core.Get (0));
                SerApps0.Start (Seconds (1));
                //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

                // Launch iperf client on node 0
                dce.SetBinary ("iperf");
                dce.ResetArguments ();
                dce.ResetEnvironment ();
                dce.AddArgument ("-c");
                dce.AddArgument ("10.9.2.2");
                dce.AddArgument ("-i");                
                dce.AddArgument ("1");                        
                dce.AddArgument ("--time");                
                dce.AddArgument (IperfTime);
                ApplicationContainer ClientApps0 = dce.Install (mobile.Get (0));
                ClientApps0.Start (Seconds (1));
                //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
            }
        }
      break;

    case 'u':
      {
        if (downloadMode)
        {
            chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));
       	// Launch iperf udp server on node 0
       	dce.SetBinary ("iperf");
       	dce.ResetArguments ();
	dce.ResetEnvironment ();
       	dce.AddArgument ("-s");
       	dce.AddArgument ("-u");
       	dce.AddArgument ("-P");
       	dce.AddArgument ("1");
       	ApplicationContainer SerApps0 = dce.Install (mobile.Get (0));
       	SerApps0.Start (Seconds (1));
       	//SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

       	// Launch iperf client on node 2
       	dce.SetBinary ("iperf");
       	dce.ResetArguments ();
       	dce.ResetEnvironment ();
       	dce.AddArgument ("-c");
       	dce.AddArgument ("10.9.1.1");
       	dce.AddArgument ("-u");
       	dce.AddArgument ("-i");
       	dce.AddArgument ("1");
       	dce.AddArgument ("-b");
       	dce.AddArgument (udp_bw+"m");
       	dce.AddArgument ("--time");
       	dce.AddArgument (IperfTime);
       	ApplicationContainer ClientApps0 = dce.Install (core.Get (0));
       	ClientApps0.Start (Seconds (1));
       	//ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
        }
        else
            {
            chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));
		// Launch iperf udp server on node 0
             dce.SetBinary ("iperf");
             dce.ResetArguments ();
             dce.ResetEnvironment ();
             dce.AddArgument ("-s");
             dce.AddArgument ("-u");
             dce.AddArgument ("-i");
             dce.AddArgument ("1");
             ApplicationContainer SerApps0 = dce.Install (core.Get (0));
             SerApps0.Start (Seconds (1));
             //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

            // Launch iperf client on node 2           
            dce.SetBinary ("iperf");
       	    dce.ResetArguments ();
       	    dce.ResetEnvironment ();
       	    dce.AddArgument ("-c");
       	    dce.AddArgument ("10.9.2.2");
       	    dce.AddArgument ("-u");
       	    dce.AddArgument ("-i");
       	    dce.AddArgument ("1");
       	    dce.AddArgument ("-b");
       	    dce.AddArgument (udp_bw+"m");
       	    dce.AddArgument ("--time");
       	    dce.AddArgument (IperfTime);
       	    ApplicationContainer ClientApps0 = dce.Install (mobile.Get (0));
       	    ClientApps0.Start (Seconds (1));
       	    //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
            }
      }
      break;

    case 'w':
      {
        downloadMode=true;
        chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
        chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));

        dce.SetBinary ("thttpd");
        dce.ResetArguments ();
        dce.ResetEnvironment ();
        dce.SetUid (1);
        dce.SetEuid (1);
	ApplicationContainer serHttp = dce.Install (core.Get (0));
        serHttp.Start (Seconds (1));

        dce.SetBinary ("wget");
        dce.ResetArguments ();
        dce.ResetEnvironment ();
        dce.AddArgument ("-r");
        dce.AddArgument ("http://10.9.2.2/index.html");
        ApplicationContainer clientHttp = dce.Install (mobile.Get (0));
        clientHttp.Start (Seconds (1));
        }
      break;

    default:
        {
            // Launch iperf server on node 0
        chanRouterBSDown.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
        chanBSRouterUp.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em2));
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-s");
            dce.AddArgument ("-P");
            dce.AddArgument ("1");
            ApplicationContainer SerApps0 = dce.Install (mobile.Get (0));
            SerApps0.Start (Seconds (1));
            //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

            // Launch iperf client on node 2
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-c");
            dce.AddArgument ("10.1.1.1");
            dce.AddArgument ("-i");
            dce.AddArgument ("1");
            dce.AddArgument ("--time");
            dce.AddArgument (IperfTime);
            ApplicationContainer ClientApps0 = dce.Install (core.Get (0));
            ClientApps0.Start (Seconds (1));
            //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
        }
      break;
    }

    /*for (int n = 0; n < 3; n++)
    {
      RunIp (c.Get (n), Seconds (0.2), "link show");
      RunIp (c.Get (n), Seconds (0.3), "route show table all");
      RunIp (c.Get (n), Seconds (0.4), "addr list");
    }*/

  // print tcp sysctl value
    //LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1.0),".net.ipv4.tcp_available_congestion_control", &PrintTcpFlags);
    /*LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.ipv4.tcp_congestion_control", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.ipv4.tcp_congestion_control", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.ipv4.tcp_rmem", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.ipv4.tcp_wmem", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.core.rmem_max", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (0), Seconds (1),".net.core.wmem_max", &PrintTcpFlags);

        //LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.ipv4.tcp_available_congestion_control", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.ipv4.tcp_congestion_control", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.ipv4.tcp_rmem", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.ipv4.tcp_wmem", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.core.rmem_max", &PrintTcpFlags);
    LinuxStackHelper::SysctlGet (c.Get (2), Seconds (1),".net.core.wmem_max", &PrintTcpFlags);
	*/

    //AsciiTraceHelper ascii;
    //p2p.EnableAsciiAll (ascii.CreateFileStream ("Kupakupa.tr"));
    //p2p.EnablePcapAll ("kupakupa");
    NS_LOG_INFO ("Run Simulation.");
    
    //std::cout << "Simulation will take about "<< (SimuTime) <<"seconds." << std::endl;


    Simulator::Stop (Seconds (EndTime));
 
    
    std::cout << "Running simulation" <<std::endl;
    Simulator::Run ();
    std::cout << "Simulation is completed" << std::endl;
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");

  return 0;
}

