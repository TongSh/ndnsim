/* ****************************************************
 * This is a simulation draft for our Vehicular NDN
 * Yuwei Xu
 * 2016-12-11
**************************************************** */
#include "ns3/vector.h"
#include "ns3/string.h"
#include "ns3/socket.h"
#include "ns3/double.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/command-line.h"
#include "ns3/mobility-model.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ns2-mobility-helper.h"
#include <iostream>

namespace ns3{


//====================================================================================
int main (int argc, char *argv[])
{
	/***********************************************************************************/
	  uint32_t nodeNum = 412;//
	/***********************************************************************************/
	  double stopTime = 4000.0;//
	/***********************************************************************************/

  LogComponentEnable ("ndn.Consumer", LOG_LEVEL_INFO);
  LogComponentEnable ("ndn.Producer", LOG_LEVEL_INFO);

  CommandLine cmd;
  cmd.Parse (argc, argv);

  //1、Create nodes
  NodeContainer c;
  c.Create (nodeNum);

  // 2、Set PHY and  MAC Layer
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);

  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
  NetDeviceContainer devices1 = wifi80211p.Install (wifiPhy, wifi80211pMac, c);

  //3、Set mobility of nodes from a tcl file
  ns3::Ns2MobilityHelper ns2helper("Scene/400node2.tcl");
  ns2helper.Install();

  // 4、Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  //ndnHelper.setCsSize (2000);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/nankai", "/localhost/nfd/strategy/multicast");

  //5、 Installing applications
  // Consumer
  //------------------------------------------------------------------------------------------
  //node 18
  ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
  consumerHelper1.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp1 = consumerHelper1.Install(c.Get(18));
  consumerApp1.Start(Seconds(25));
  consumerApp1.Stop(Seconds(1048));

  //node 25
  ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
  consumerHelper2.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp2 = consumerHelper2.Install(c.Get(25));
  consumerApp2.Start(Seconds(29));
  consumerApp2.Stop(Seconds(1226));

  //node 39
  ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
  consumerHelper3.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp3 = consumerHelper3.Install(c.Get(39));
  consumerApp3.Start(Seconds(48));
  consumerApp3.Stop(Seconds(1377));

  //node 63
  ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
  consumerHelper4.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp4 = consumerHelper4.Install(c.Get(63));
  consumerApp4.Start(Seconds(68));
  consumerApp4.Stop(Seconds(1535));

  //node 84
  ndn::AppHelper consumerHelper5("ns3::ndn::ConsumerCbr");
  consumerHelper5.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp5 = consumerHelper5.Install(c.Get(84));
  consumerApp5.Start(Seconds(111));
  consumerApp5.Stop(Seconds(1174));

  //node 118
  ndn::AppHelper consumerHelper6("ns3::ndn::ConsumerCbr");
  consumerHelper6.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp6 = consumerHelper6.Install(c.Get(118));
  consumerApp6.Start(Seconds(87));
  consumerApp6.Stop(Seconds(1501));

  //node 143
  ndn::AppHelper consumerHelper7("ns3::ndn::ConsumerCbr");
  consumerHelper7.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp7 = consumerHelper7.Install(c.Get(143));
  consumerApp7.Start(Seconds(147));
  consumerApp7.Stop(Seconds(1288));

  //node 167
  ndn::AppHelper consumerHelper8("ns3::ndn::ConsumerCbr");
  consumerHelper8.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp8 = consumerHelper8.Install(c.Get(167));
  consumerApp8.Start(Seconds(168));
  consumerApp8.Stop(Seconds(1641));

  //node 208
  ndn::AppHelper consumerHelper9("ns3::ndn::ConsumerCbr");
  consumerHelper9.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp9 = consumerHelper9.Install(c.Get(208));
  consumerApp9.Start(Seconds(203));
  consumerApp9.Stop(Seconds(1407));

  //node 230
  ndn::AppHelper consumerHelper10("ns3::ndn::ConsumerCbr");
  consumerHelper10.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp10 = consumerHelper10.Install(c.Get(230));
  consumerApp10.Start(Seconds(221));
  consumerApp10.Stop(Seconds(1281));

  //node 279
  ndn::AppHelper consumerHelper11("ns3::ndn::ConsumerCbr");
  consumerHelper11.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp11 = consumerHelper11.Install(c.Get(279));
  consumerApp11.Start(Seconds(274));
  consumerApp11.Stop(Seconds(1288));

  //node 289
  ndn::AppHelper consumerHelper12("ns3::ndn::ConsumerCbr");
  consumerHelper12.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp12 = consumerHelper12.Install(c.Get(289));
  consumerApp12.Start(Seconds(282));
  consumerApp12.Stop(Seconds(1408));

  //node 346
  ndn::AppHelper consumerHelper13("ns3::ndn::ConsumerCbr");
  consumerHelper13.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp13 = consumerHelper13.Install(c.Get(346));
  consumerApp13.Start(Seconds(343));
  consumerApp13.Stop(Seconds(1544));

  //node 369
  ndn::AppHelper consumerHelper14("ns3::ndn::ConsumerCbr");
  consumerHelper14.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp14 = consumerHelper14.Install(c.Get(369));
  consumerApp14.Start(Seconds(378));
  consumerApp14.Stop(Seconds(2011));

  //node 384
  ndn::AppHelper consumerHelper15("ns3::ndn::ConsumerCbr");
  consumerHelper15.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
  ApplicationContainer consumerApp15 = consumerHelper15.Install(c.Get(384));
  consumerApp15.Start(Seconds(421));
  consumerApp15.Stop(Seconds(2013));

  //Provider
  //------------------------------------------------------------------------------------------
  //node 400
 	ndn::AppHelper producerHelper("ns3::ndn::Producer");
 	producerHelper.SetPrefix("/nankai/Info_0");
 	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper.Install(c.Get(400));

 	//node 401
 	ndn::AppHelper producerHelper2("ns3::ndn::Producer");
 	producerHelper2.SetPrefix("/nankai/Info_1");
 	producerHelper2.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper2.Install(c.Get(401));

 	//node 402
 	ndn::AppHelper producerHelper3("ns3::ndn::Producer");
 	producerHelper3.SetPrefix("/nankai/Info_2");
 	producerHelper3.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper3.Install(c.Get(402));

 	//node 403
 	ndn::AppHelper producerHelper4("ns3::ndn::Producer");
 	producerHelper4.SetPrefix("/nankai/Info_3");
 	producerHelper4.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper4.Install(c.Get(403));

 	//node 404
 	ndn::AppHelper producerHelper5("ns3::ndn::Producer");
 	producerHelper5.SetPrefix("/nankai/Info_4");
 	producerHelper5.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper5.Install(c.Get(404));

 	//node 405
 	ndn::AppHelper producerHelper6("ns3::ndn::Producer");
 	producerHelper6.SetPrefix("/nankai/Info_5");
 	producerHelper6.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper6.Install(c.Get(405));

 	//node 406
 	ndn::AppHelper producerHelper7("ns3::ndn::Producer");
 	producerHelper7.SetPrefix("/nankai/Info_3");
 	producerHelper7.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper7.Install(c.Get(406));

 	//node 407
 	ndn::AppHelper producerHelper8("ns3::ndn::Producer");
 	producerHelper8.SetPrefix("/nankai/Info_4");
 	producerHelper8.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper8.Install(c.Get(407));

 	//node 408
 	ndn::AppHelper producerHelper9("ns3::ndn::Producer");
 	producerHelper9.SetPrefix("/nankai/Info_5");
 	producerHelper9.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper9.Install(c.Get(408));

 	//node 409
 	ndn::AppHelper producerHelper10("ns3::ndn::Producer");
 	producerHelper10.SetPrefix("/nankai/Info_0");
 	producerHelper10.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper10.Install(c.Get(409));

 	//node 410
 	ndn::AppHelper producerHelper11("ns3::ndn::Producer");
 	producerHelper11.SetPrefix("/nankai/Info_1");
 	producerHelper11.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper11.Install(c.Get(410));

 	//node 411
 	ndn::AppHelper producerHelper12("ns3::ndn::Producer");
 	producerHelper12.SetPrefix("/nankai/Info_2");
 	producerHelper12.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper12.Install(c.Get(411));


  //Tracer
  ndn::AppDelayTracer::InstallAll("Result/app-delays-trace.txt");

  Simulator::Stop(Seconds(stopTime));

  Simulator::Run();
  Simulator::Destroy();

  cout<<"Simulation End Successfully!"<<endl;
  return 0;
}
}

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
