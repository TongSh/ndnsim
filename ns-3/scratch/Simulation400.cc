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
#include "ns3/propagation-module.h"
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
	double stopTime = 2000.0;//
	/***********************************************************************************/

	LogComponentEnable ("ndn.Consumer", LOG_LEVEL_INFO);
	LogComponentEnable ("ndn.Producer", LOG_LEVEL_INFO);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	//1、Create nodes
	NodeContainer c;
	c.Create (nodeNum);


	//2、Set mobility and propagation of nodes
	ns3::Ns2MobilityHelper ns2helper("Scene/400node2.tcl");
	ns2helper.Install();

	// 3、Set PHY and  MAC Layer
	Ptr<ThreeLogDistancePropagationLossModel> lossModel = CreateObject<ThreeLogDistancePropagationLossModel>();
	lossModel->SetNext(CreateObject<NakagamiPropagationLossModel>());

	Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
	wifiChannel->SetPropagationLossModel (lossModel);
	wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	wifiPhy.SetChannel (wifiChannel);

	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
	NetDeviceContainer devices1 = wifi80211p.Install (wifiPhy, wifi80211pMac, c);


	// 4、Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	//ndnHelper.setCsSize (2000);
	ndnHelper.InstallAll();

	// Choosing forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/multicast");

	//5、 Installing applications
	// Consumer
	//------------------------------------------------------------------------------------------

	//node 64
	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
	consumerHelper1.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
	ApplicationContainer consumerApp1 = consumerHelper1.Install(c.Get(64));
	consumerApp1.Start(Seconds(68));
	consumerApp1.Stop(Seconds(1283));

  	//node 124
	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
	ApplicationContainer consumerApp2 = consumerHelper2.Install(c.Get(124));
	consumerApp2.Start(Seconds(126));
	consumerApp2.Stop(Seconds(1177));

	//node 188
	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
	consumerHelper3.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
	ApplicationContainer consumerApp3 = consumerHelper3.Install(c.Get(188));
	consumerApp3.Start(Seconds(185));
	consumerApp3.Stop(Seconds(1504));

	//node 239
	ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
	consumerHelper4.SetAttribute("Frequency", StringValue("1"));  //1 interests a second
	ApplicationContainer consumerApp4 = consumerHelper4.Install(c.Get(239));
	consumerApp4.Start(Seconds(229));
	consumerApp4.Stop(Seconds(1633));

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
