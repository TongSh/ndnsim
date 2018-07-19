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
#include "ns3/propagation-module.h"
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
	uint32_t nodeNum = 3;//
	/***********************************************************************************/
	double stopTime = 1500.0;//
	/***********************************************************************************/

	LogComponentEnable ("ndn.Consumer", LOG_LEVEL_INFO);
	LogComponentEnable ("ndn.Producer", LOG_LEVEL_INFO);
//	LogComponentEnable ("nfd.Forwarder", LOG_LEVEL_DEBUG);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	//1、Create nodes
	NodeContainer c;
	c.Create (nodeNum);



	//2、Set mobility and propagation of nodes
	//  ns3::Ns2MobilityHelper ns2helper("Scene/400node2.tcl");
	//  ns2helper.Install();

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (0.0, 0.0, 0.0));
	positionAlloc->Add (Vector (50.0, 50.0, 0.0));
	positionAlloc->Add (Vector (100.0, 0.0, 0.0));
	mobility.SetPositionAllocator (positionAlloc);
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.Install (c);

	Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel>();
	lossModel->SetDefaultLoss(200);
	lossModel->SetLoss(c.Get(0)->GetObject<MobilityModel>(),c.Get(1)->GetObject<MobilityModel>(),10);
	lossModel->SetLoss(c.Get(2)->GetObject<MobilityModel>(),c.Get(1)->GetObject<MobilityModel>(),10);

	// 3、Set PHY and  MAC Layer
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
	ndnHelper.InstallAll();

	// Choosing forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/multicast");

	//5、 Installing applications
	// Consumer
	//node 0
	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
	consumerHelper1.SetAttribute("Frequency", StringValue("0.1"));  //1 interests a second
	ApplicationContainer consumerApp1 = consumerHelper1.Install(c.Get(0));
	consumerApp1.Start(Seconds(0));
	consumerApp1.Stop(Seconds(1000));

	//Provider
	//node 2
 	ndn::AppHelper producerHelper("ns3::ndn::Producer");
 	producerHelper.SetPrefix("/nankai");
 	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
 	producerHelper.Install(c.Get(2));

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
