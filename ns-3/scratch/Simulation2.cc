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
#include "ns3/constant-position-mobility-model.h"
#include "ns3/gnuplot.h"
#include <sstream>

namespace ns3{


//====================================================================================
int main (int argc, char *argv[])
{
	/***********************************************************************************/
	uint32_t nodeNum = 2;//

	LogComponentEnable ("ndn.Consumer", LOG_LEVEL_INFO);
	LogComponentEnable ("ndn.Producer", LOG_LEVEL_INFO);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	//1、Create nodes
	NodeContainer c;
	c.Create (nodeNum);



	//2、Set mobility and propagation of nodes
	Ptr<ConstantPositionMobilityModel> a = CreateObject<ConstantPositionMobilityModel>();
	Ptr<ConstantPositionMobilityModel> b = CreateObject<ConstantPositionMobilityModel>();

	Gnuplot plot;
	plot.AppendExtra("set xlabel 'Distance'");
	plot.AppendExtra("set ylabel 'rxPower(dBm)'");
	plot.AppendExtra("set key top right");
	double txPowerDbm = 20;
	Gnuplot2dDataset dataset;
	dataset.SetStyle(Gnuplot2dDataset::LINES);

	Ptr<ThreeLogDistancePropagationLossModel> lossModel = CreateObject<ThreeLogDistancePropagationLossModel>();
//	Ptr<NakagamiPropagationLossModel> lossModel = CreateObject<NakagamiPropagationLossModel>();
	lossModel->SetNext(CreateObject<NakagamiPropagationLossModel>());

	a->SetPosition(Vector(0.0,0.0,0.0));
	for(double distance = 0.0; distance < 1500.0; distance += 5){
		b->SetPosition(Vector(distance,0.0,0.0));
		double rxPowerDbm = lossModel->CalcRxPower(txPowerDbm,a,b);
		dataset.Add(distance,rxPowerDbm);
		Simulator::Stop(Seconds(1.0));
		Simulator::Run();
	}

	std::ostringstream os;
	os<<"txPower"<<txPowerDbm<<"dBm";
	dataset.SetTitle(os.str());
	plot.AddDataset(dataset);
	plot.AddDataset(Gnuplot2dFunction("Energy Detection Threshold","-96.0"));
	plot.SetTitle("Loss Model");

	GnuplotCollection gnuplots("propagation-modle.pdf");
	gnuplots.AddPlot(plot);

	std::ofstream plotFile("Loss.plt");
	gnuplots.GenerateOutput(plotFile);
	plotFile.close();
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
