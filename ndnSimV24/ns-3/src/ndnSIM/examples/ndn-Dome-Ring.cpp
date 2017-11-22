

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/ndnSIM-module.h"

#include "ns3/spectrum-helper.h"
#include "ns3/antenna-model.h"
#include "ns3/parabolic-antenna-model.h"
#include "ns3/spectrum-wifi-phy.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/angles.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.RingDome");


int main(int argc, char* argv[]) {
	// disable fragmentation
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("OfdmRate24Mbps"));

	CommandLine cmd;
	cmd.Parse(argc, argv);

	//////////////////////
	//////////////////////
	//////////////////////
	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate24Mbps"));

	//Dome spectrum
	SpectrumChannelHelper spectrumChannelHelper = SpectrumChannelHelper::Default();
	spectrumChannelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	spectrumChannelHelper.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
	spectrumChannelHelper.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
//	Config::SetDefault ("ns3::WifiPhy::CcaMode1Threshold", DoubleValue (-62.0));

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	//Dome spectrum
	SpectrumWifiPhyHelper spectrumWifiPhyHelper = SpectrumWifiPhyHelper::Default ();
	spectrumWifiPhyHelper.SetChannel(spectrumChannelHelper.Create());
	spectrumWifiPhyHelper.Set("TxPowerStart", DoubleValue(100));
	spectrumWifiPhyHelper.Set("TxPowerEnd", DoubleValue(100));

	spectrumWifiPhyHelper.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
	spectrumWifiPhyHelper.SetPcapDataLinkType (SpectrumWifiPhyHelper::DLT_IEEE802_11_RADIO);

//	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
//	wifiPhyHelper.SetChannel(wifiChannel.Create());
//	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
//	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");


//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-3n.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-Ring-7n.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Diamond-5n-moderateDist.txt");
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Diamond-5n-with-moving.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Diamond-5n-far-Dist.txt");





	// Create Mobile nodes.
	NodeContainer mobileNodes;
	int nodeNr = 5;
	mobileNodes.Create(nodeNr);

	NetDeviceContainer netDeviceContParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//	NetDeviceContainer netDeviceContParabolicN0Dev2 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//	NetDeviceContainer netDeviceContParabolicN0Dev3 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//	NetDeviceContainer netDeviceContParabolicN0Dev4 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));


	for (int i = 0; i < 1; i++)
	  {
//		Ptr<WifiPhy> wp = netDeviceContParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//		Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy> (wp);
//
//		Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
//		parabolicAntenna->SetBeamwidth(10);
//		parabolicAntenna->SetOrientation(0);
//		swp->SetAntenna(parabolicAntenna);
//
//		Ptr<WifiPhy> wp2 = netDeviceContParabolicN0Dev2.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//		Ptr<SpectrumWifiPhy> swp2 = DynamicCast<SpectrumWifiPhy> (wp2);
//
//		Ptr<ParabolicAntennaModel> parabolicAntenna2 = CreateObject<ParabolicAntennaModel>();
//		parabolicAntenna2->SetBeamwidth(10);
//		parabolicAntenna2->SetOrientation(90);
//		swp2->SetAntenna(parabolicAntenna2);
//
//		Ptr<WifiPhy> wp3 = netDeviceContParabolicN0Dev3.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//		Ptr<SpectrumWifiPhy> swp3 = DynamicCast<SpectrumWifiPhy>(wp3);
//
//		Ptr<ParabolicAntennaModel> parabolicAntenna3 = CreateObject<ParabolicAntennaModel>();
//		parabolicAntenna3->SetBeamwidth(10);
//		parabolicAntenna3->SetOrientation(180);
//		swp3->SetAntenna(parabolicAntenna3);
//
//		Ptr<WifiPhy> wp4 = netDeviceContParabolicN0Dev4.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//		Ptr<SpectrumWifiPhy> swp4 = DynamicCast<SpectrumWifiPhy>(wp4);
//
//		Ptr<ParabolicAntennaModel> parabolicAntenna4 = CreateObject<ParabolicAntennaModel>();
//		parabolicAntenna4->SetBeamwidth(10);
//		parabolicAntenna4->SetOrientation(270);
//		swp4->SetAntenna(parabolicAntenna4);

		//4 antennas in 1 net dev
		Ptr<WifiPhy> wp = netDeviceContParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
		Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy>(wp);

		Ptr<ParabolicAntennaModel> parabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
		parabolicAntenna1->SetBeamwidth(10);
		parabolicAntenna1->SetOrientation(0);
		swp->SetAntenna(parabolicAntenna1);

		Ptr<ParabolicAntennaModel> parabolicAntenna2 = CreateObject<ParabolicAntennaModel>();
		parabolicAntenna2->SetBeamwidth(10);
		parabolicAntenna2->SetOrientation(90);
		swp->SetAntenna(parabolicAntenna2);

		Ptr<ParabolicAntennaModel> parabolicAntenna3 = CreateObject<ParabolicAntennaModel>();
		parabolicAntenna3->SetBeamwidth(10);
		parabolicAntenna3->SetOrientation(180);
		swp->SetAntenna(parabolicAntenna3);

		Ptr<ParabolicAntennaModel> parabolicAntenna4 = CreateObject<ParabolicAntennaModel>();
		parabolicAntenna4->SetBeamwidth(10);
		parabolicAntenna4->SetOrientation(270);
		swp->SetAntenna(parabolicAntenna4);
	  }


	NetDeviceContainer netDeviceContParabolicN1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

		for (int i = 0; i < 1; i++)
			{
				Ptr<WifiPhy> wp = netDeviceContParabolicN1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
				Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy> (wp);

				Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
				parabolicAntenna->SetBeamwidth(20);
				parabolicAntenna->SetOrientation(0);
				swp->SetAntenna(parabolicAntenna);

		}


	NetDeviceContainer netDeviceContParabolicN2 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

		for (int i = 0; i < 1; i++)
		  {
			Ptr<WifiPhy> wp = netDeviceContParabolicN2.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
			Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy> (wp);

			Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
			parabolicAntenna->SetBeamwidth(20);
			parabolicAntenna->SetOrientation(90); // if prod to 270 =>  moderate Dist(50): funcitons with 0 90 180 270 ; far dist(120): yes:90 no:0 270 180
			swp->SetAntenna(parabolicAntenna);

		  }


	NetDeviceContainer netDeviceContParabolicN3 = wifi.Install(spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));

	for (int i = 0; i < 1; i++) {
		Ptr<WifiPhy> wp = netDeviceContParabolicN3.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
		Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy>(wp);

		Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
		parabolicAntenna->SetBeamwidth(20);
		parabolicAntenna->SetOrientation(180);
		swp->SetAntenna(parabolicAntenna);

	}

	NetDeviceContainer netDeviceContParabolicN4 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));

		for (int i = 0; i < 1; i++)
		  {
			Ptr<WifiPhy> wp = netDeviceContParabolicN4.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
			Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy> (wp);

			Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
			parabolicAntenna->SetBeamwidth(20);
			parabolicAntenna->SetOrientation(270);
			swp->SetAntenna(parabolicAntenna);

		 }


	// configure movements for each node, while reading trace file

	////////////////
	// 1. Install Wifi
//	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//	NetDeviceContainer wifiNetDevices2 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//	NetDeviceContainer wifiNetDevices3 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));
//	NetDeviceContainer wifiNetDevices4 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));
//	NetDeviceContainer wifiNetDevices5 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));
//	NetDeviceContainer wifiNetDevices6 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(5));

	// 2. Install Mobility model
//	Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (80));
	ns2MobHelper.Install();


	// 3. Install NDN stack
	NS_LOG_INFO("Installing NDN stack");
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.Install(mobileNodes);

	ndn::StrategyChoiceHelper::Install(mobileNodes, "/", "/localhost/nfd/strategy/multicast");



	// 4. Set up applications
	NS_LOG_INFO("Installing Applications");

//	ndn::AppHelper producerHelper("ns3::ndn::Producer");
//	producerHelper.SetPrefix("/beacon");
//	producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
//	producerHelper.Install(mobileNodes.Get(6));

	ndn::AppHelper producerHelper("ns3::ndn::Producer");
	producerHelper.SetPrefix("/beacon");
	producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper.Install(mobileNodes.Get(0));

//	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetPrefix("/beacon");
//	consumerHelper.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper.Install(mobileNodes.Get(0));

 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/beacon");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper2.Install(mobileNodes.Get(1));

	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
	consumerHelper3.SetPrefix("/beacon");
	consumerHelper3.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper3.Install(mobileNodes.Get(2));

	ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
	consumerHelper4.SetPrefix("/beacon");
	consumerHelper4.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper4.Install(mobileNodes.Get(3));

	ndn::AppHelper consumerHelper5("ns3::ndn::ConsumerCbr");
	consumerHelper5.SetPrefix("/beacon");
	consumerHelper5.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper5.Install(mobileNodes.Get(4));

//	ndn::AppHelper consumerHelper6("ns3::ndn::ConsumerCbr");
////	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
//	consumerHelper6.SetPrefix("/beacon");
//	consumerHelper6.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper6.Install(mobileNodes.Get(5));


	////////////////

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
