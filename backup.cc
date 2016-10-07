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
	myfile.open ("files-2/index.html");

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

void
PrintTcpFlags (std::string key, std::string value)
{
  NS_LOG_INFO (key << "=" << value);
}

int main (int argc, char *argv[])
{
	double errRate = 0.001;
	std::string tcp_cc = "reno";
	std::string tcp_mem_user = "4096 8192 8388608";
	std::string tcp_mem_user_wmem = "4096 8192 8388608";
	std::string tcp_mem_user_rmem = "4096 8192 8388608";
	
	std::string tcp_mem_server = "4096 8192 8388608";
	std::string tcp_mem_server_wmem = "4096 8192 8388608";
	std::string tcp_mem_server_rmem = "4096 8192 8388608";

	std::string udp_bw="1";
	std::string delay = "2ms";
	std::string user_bw = "150Mbps";
	std::string server_bw = "10Gbps";

	int jitter =1;
	double alpha = 0;
	double mean = 0;
	double variance = 0;
	int monitor = 1;
	int mode = 0;
	
       int ErrorModel = 1;
       double SimuTime = 50;
       int htmlSize = 2; // in mega bytes
       char TypeOfConnection = 'p'; // iperf tcp connection
       bool downloadMode = true;
       bool inputFromXml = false;
     
      	/*
	unsigned int chan_jitter = 1;
	double chan_alpha = 0.3;
   	double chan_variance = 2;
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
                  
      cmd.AddValue ("user_bw", "bandwidth between user and BS, in Mbps. Default is 150", user_bw);
      cmd.AddValue ("server_bw", "bandwidth between server and BS, in Gbps. Default is 10", server_bw);

     cmd.AddValue ("delay", "Delay.", delay);
     cmd.AddValue ("errRate", "Error rate.", errRate);
     cmd.AddValue ("ErrorModel", "Choose error model you want to use. options: 1 -rate error model-default, 2 - burst error model", ErrorModel);
     cmd.AddValue ("udp_bw","banwidth set for UDP, default is 1M", udp_bw);
     cmd.AddValue ("htmlSize","banwidth set for UDP, default is 1M", htmlSize);
     cmd.AddValue ("SimuTime", "time to do the simulaton, in second", SimuTime);

    cmd.AddValue ("chan_jitter", "jitter in server-BS conection", jitter);
    cmd.AddValue ("chan_alpha", "alpha for random distribution in server-BS conection", alpha);
    cmd.AddValue ("chan_variance", "variance for normal random distribution in server-BS conection", variance);
    cmd.AddValue ("chan_mean", " Normal random distribution mean in server-BS conection", mean);
	
     cmd.Parse (argc, argv);     
      

      if (inputFromXml)
      {
	string fileName = "inputDCE.xml";	
	ParseInput parser;
    parser.parseInputXml(fileName,TypeOfConnection,tcp_cc,udp_bw,delay,SimuTime,downloadMode,errRate,jitter,alpha,mean,variance, ErrorModel, user_bw, server_bw, htmlSize,tcp_mem_user, tcp_mem_user_wmem,tcp_mem_user_rmem, tcp_mem_server, tcp_mem_server_wmem, tcp_mem_server_rmem);
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
    	     
    	     
     
     
    
    


// topologies
    std::cout << "Building topologies.." << std::endl;
    NS_LOG_INFO ("Create nodes.");
    NodeContainer c;
    c.Create (3);
    NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
    NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));

    DceManagerHelper dceManager;

    std::cout << "Setting memory size.." << std::endl;
        //setting memory size for user and server
        
#ifdef KERNEL_STACK
    dceManager.SetNetworkStack ("ns3::LinuxSocketFdFactory", "Library", StringValue ("liblinux.so"));
    LinuxStackHelper stack;
    stack.Install (c);
    dceManager.Install (c);
    
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
	    mkdir ("files-2",0744);
	    GenerateHtmlFile(htmlSize);
	    SimuTime=100;
	    if (htmlSize*1000 > atoi(tcp_mem_user_max_wmem.c_str())){
	    
		double tmp2=atof(tcp_mem_user_max_wmem.c_str())/(htmlSize*1000);
		SimuTime = (htmlSize*10)/(tmp2)*1.5*(htmlSize/tmp2);
		

	    }

	}
		
    std::string IperfTime = DoubleToString(SimuTime);	
	
    stack.SysctlSet (c.Get(0), ".net.ipv4.tcp_mem", tcp_mem_user);
    stack.SysctlSet (c.Get(0), ".net.ipv4.tcp_wmem", tcp_mem_user_wmem);
    stack.SysctlSet (c.Get(0), ".net.ipv4.tcp_rmem", tcp_mem_user_rmem);
    stack.SysctlSet (c.Get(0), ".net.core.wmem_max", tcp_mem_user_max_wmem);
    stack.SysctlSet (c.Get(0), ".net.core.rmem_max", tcp_mem_user_max_rmem);
    stack.SysctlSet (c.Get(0), ".net.core.netdev_max_backlog", "250000");
        
    stack.SysctlSet (c.Get(2), ".net.ipv4.tcp_mem", tcp_mem_server);
    stack.SysctlSet (c.Get(2), ".net.ipv4.tcp_wmem", tcp_mem_server_wmem);
    stack.SysctlSet (c.Get(2), ".net.ipv4.tcp_rmem", tcp_mem_server_rmem);
    stack.SysctlSet (c.Get(2), ".net.core.wmem_max", tcp_mem_server_max_wmem);
    stack.SysctlSet (c.Get(2), ".net.core.rmem_max", tcp_mem_server_max_rmem);
    stack.SysctlSet (c.Get(2), ".net.core.netdev_max_backlog", "250000");
    stack.SysctlSet (c, ".net.ipv4.tcp_congestion_control", tcp_cc);
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

// channel for user to BS
	NS_LOG_INFO ("Create channels.");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (user_bw));
	p2p.SetChannelAttribute ("Delay", StringValue ("0"));
	p2p.SetChannelAttribute ("monitor", UintegerValue (monitor));
	p2p.SetChannelAttribute ("mode",UintegerValue (mode));
	NetDeviceContainer d0d1 = p2p.Install (n0n1);
//channel for server to BS
p2p.SetDeviceAttribute ("DataRate", StringValue (server_bw));
p2p.SetChannelAttribute ("Delay", StringValue (delay));
p2p.SetChannelAttribute ("Jitter", UintegerValue (1));
p2p.SetChannelAttribute ("alpha", DoubleValue (alpha));
p2p.SetChannelAttribute ("mean", DoubleValue (mean));
p2p.SetChannelAttribute ("variance", DoubleValue (variance));
p2p.SetChannelAttribute ("monitor", UintegerValue (monitor));
p2p.SetChannelAttribute ("mode",UintegerValue (mode) );
NetDeviceContainer d1d2 = p2p.Install (n1n2);
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
		std::cout << "Unknown error model. Restore to default: rate error model" <<std::endl;
	}



// IP Address
    NS_LOG_INFO ("Assign IP Addresses.");
    std::cout << "Setting IP addresses" << std::endl;
    Ipv4AddressHelper ipv4;
    //for client and BS net devices
        ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
        //for server and BS devices
        ipv4.SetBase ("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

// Create router nodes, initialize routing database and set up the routing tables in the nodes.
    std::cout << "Creating routing table" << std::endl;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

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
            d1d2.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            // Launch iperf server on node 0 (mobile device)
            
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-s");
            dce.AddArgument ("-P");
            dce.AddArgument ("1");
            ApplicationContainer SerApps0 = dce.Install (c.Get (0));
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
            ApplicationContainer ClientApps0 = dce.Install (c.Get (2));
            ClientApps0.Start (Seconds (1));
            //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
        }
            else
            {
                d1d2.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
                // Launch iperf server on node 2
                // server will receive tcp message
                dce.SetBinary ("iperf");
                dce.ResetArguments ();
                dce.ResetEnvironment ();
                dce.AddArgument ("-s");
                dce.AddArgument ("-P");
                dce.AddArgument ("1");
                ApplicationContainer SerApps0 = dce.Install (c.Get (2));
                SerApps0.Start (Seconds (1));
                //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

                // Launch iperf client on node 0
                dce.SetBinary ("iperf");
                dce.ResetArguments ();
                dce.ResetEnvironment ();
                dce.AddArgument ("-c");
                dce.AddArgument ("10.1.2.2");
                dce.AddArgument ("-i");                
                dce.AddArgument ("1");                        
                dce.AddArgument ("--time");                
                dce.AddArgument (IperfTime);
                ApplicationContainer ClientApps0 = dce.Install (c.Get (0));
                ClientApps0.Start (Seconds (1));
                //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
            }
        }
      break;

    case 'u':
      {
        if (downloadMode)
        {
        d1d2.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
       	// Launch iperf udp server on node 0
       	dce.SetBinary ("iperf");
       	dce.ResetArguments ();
	dce.ResetEnvironment ();
       	dce.AddArgument ("-s");
       	dce.AddArgument ("-u");
       	dce.AddArgument ("-P");
       	dce.AddArgument ("1");
       	ApplicationContainer SerApps0 = dce.Install (c.Get (0));
       	SerApps0.Start (Seconds (1));
       	//SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

       	// Launch iperf client on node 2
       	dce.SetBinary ("iperf");
       	dce.ResetArguments ();
       	dce.ResetEnvironment ();
       	dce.AddArgument ("-c");
       	dce.AddArgument ("10.1.1.1");
       	dce.AddArgument ("-u");
       	dce.AddArgument ("-i");
       	dce.AddArgument ("1");
       	dce.AddArgument ("-b");
       	dce.AddArgument (udp_bw+"m");
       	dce.AddArgument ("--time");
       	dce.AddArgument (IperfTime);
       	ApplicationContainer ClientApps0 = dce.Install (c.Get (2));
       	ClientApps0.Start (Seconds (1));
       	//ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
        }
        else
            {
             d1d2.Get(1)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
		// Launch iperf udp server on node 0
             dce.SetBinary ("iperf");
             dce.ResetArguments ();
             dce.ResetEnvironment ();
             dce.AddArgument ("-s");
             dce.AddArgument ("-u");
             dce.AddArgument ("-i");
             dce.AddArgument ("1");
             ApplicationContainer SerApps0 = dce.Install (c.Get (2));
             SerApps0.Start (Seconds (1));
             //SerApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));

            // Launch iperf client on node 2           
            dce.SetBinary ("iperf");
       	    dce.ResetArguments ();
       	    dce.ResetEnvironment ();
       	    dce.AddArgument ("-c");
       	    dce.AddArgument ("10.1.2.2");
       	    dce.AddArgument ("-u");
       	    dce.AddArgument ("-i");
       	    dce.AddArgument ("1");
       	    dce.AddArgument ("-b");
       	    dce.AddArgument (udp_bw+"m");
       	    dce.AddArgument ("--time");
       	    dce.AddArgument (IperfTime);
       	    ApplicationContainer ClientApps0 = dce.Install (c.Get (0));
       	    ClientApps0.Start (Seconds (1));
       	    //ClientApps0.Stop (Seconds (SimuTime+(SimuTime*25/100)));
            }
      }
      break;

    case 'w':
      {
        downloadMode=true;
        d1d2.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));

        dce.SetBinary ("thttpd");
        dce.ResetArguments ();
        dce.ResetEnvironment ();
        dce.SetUid (1);
        dce.SetEuid (1);
	ApplicationContainer serHttp = dce.Install (c.Get (2));
        serHttp.Start (Seconds (1));

        dce.SetBinary ("wget");
        dce.ResetArguments ();
        dce.ResetEnvironment ();
        dce.AddArgument ("-r");
        dce.AddArgument ("http://10.1.2.2/index.html");
        ApplicationContainer clientHttp = dce.Install (c.Get (0));
        clientHttp.Start (Seconds (1));
        }
      break;

    default:
        {
            // Launch iperf server on node 0
            d1d2.Get(0)-> SetAttribute ("ReceiveErrorModel", PointerValue (em));
            dce.SetBinary ("iperf");
            dce.ResetArguments ();
            dce.ResetEnvironment ();
            dce.AddArgument ("-s");
            dce.AddArgument ("-P");
            dce.AddArgument ("1");
            ApplicationContainer SerApps0 = dce.Install (c.Get (0));
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
            ApplicationContainer ClientApps0 = dce.Install (c.Get (2));
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

    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll (ascii.CreateFileStream ("Kupakupa.tr"));
    p2p.EnablePcapAll ("kupakupa");
    NS_LOG_INFO ("Run Simulation.");
    
    //std::cout << "Simulation will take about "<< (SimuTime) <<"seconds." << std::endl;


    Simulator::Stop (Seconds (EndTime));
 
    
    
    Simulator::Run ();
    std::cout << "Simulation is completed" << std::endl;
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");

  return 0;
}

