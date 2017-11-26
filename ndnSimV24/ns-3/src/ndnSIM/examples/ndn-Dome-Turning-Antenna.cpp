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

NS_LOG_COMPONENT_DEFINE("ndn.DomeTurningAntenna");

int main(int argc, char* argv[]) {

	std::string traceFileString = "ns-movements-RSU-To-Moving-2n.txt";

	// disable fragmentation
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("OfdmRate24Mbps"));

	CommandLine cmd;
	cmd.Parse(argc, argv);



	//Wifi Helper
	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate24Mbps"));

	//Dome spectrumChannel-Helper
	SpectrumChannelHelper spectrumChannelHelper = SpectrumChannelHelper::Default();
	spectrumChannelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	spectrumChannelHelper.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
	spectrumChannelHelper.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	//Dome spectrumWifi Helper
	SpectrumWifiPhyHelper spectrumWifiPhyHelper = SpectrumWifiPhyHelper::Default ();
	spectrumWifiPhyHelper.SetChannel(spectrumChannelHelper.Create());
	spectrumWifiPhyHelper.Set("TxPowerStart", DoubleValue(100));
	spectrumWifiPhyHelper.Set("TxPowerEnd", DoubleValue(100));

	spectrumWifiPhyHelper.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
	spectrumWifiPhyHelper.SetPcapDataLinkType (SpectrumWifiPhyHelper::DLT_IEEE802_11_RADIO);


	//Yans Wifi For interests

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
	wifiPhyHelper.SetChannel(wifiChannel.Create());
	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	//MacHelper
	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

	//Mobility Helper
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper(traceFileString);

	// Create Mobile nodes.
	NodeContainer mobileNodes;
	int nodeNr = 2;
	mobileNodes.Create(nodeNr);

	//NDev node 0 Parabolic 1
	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN01 = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN01 = DynamicCast<SpectrumWifiPhy> (wpN01);

	//Set Parabolic Antenna1 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntenna0 = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna0->SetBeamwidth(359);
	parabolicAntenna0->SetOrientation(90);
	swpN01->SetAntenna(parabolicAntenna0);

//	//NDev node 0 Parabolic 2
//	NetDeviceContainer netDeviceContainerParabolicN0Dev2 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN02 = netDeviceContainerParabolicN0Dev2.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN02 = DynamicCast<SpectrumWifiPhy> (wpN02);
//
//	//Set Parabolic Antenna2 to node 0
//	Ptr<ParabolicAntennaModel> parabolicAntennaN0A2 = CreateObject<ParabolicAntennaModel>();
//	parabolicAntennaN0A2->SetBeamwidth(10);
//	parabolicAntennaN0A2->SetOrientation(90);
//	swpN02->SetAntenna(parabolicAntennaN0A2);
//
//	//NDev node 0 Parabolic 3
//	NetDeviceContainer netDeviceContainerParabolicN0Dev3 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN03 = netDeviceContainerParabolicN0Dev3.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN03 = DynamicCast<SpectrumWifiPhy> (wpN03);
//
//	//Set Parabolic Antenna3 to node 0
//	Ptr<ParabolicAntennaModel> parabolicAntennaN0A3 = CreateObject<ParabolicAntennaModel>();
//	parabolicAntennaN0A3->SetBeamwidth(10);
//	parabolicAntennaN0A3->SetOrientation(180);
//	swpN03->SetAntenna(parabolicAntennaN0A3);
//
//	//NDev node 0 Parabolic 4
//	NetDeviceContainer netDeviceContainerParabolicN0Dev4 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN04 = netDeviceContainerParabolicN0Dev4.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN04 = DynamicCast<SpectrumWifiPhy> (wpN04);
//
//	//Set Parabolic Antenna4 to node 0
//	Ptr<ParabolicAntennaModel> parabolicAntennaN0A4 = CreateObject<ParabolicAntennaModel>();
//	parabolicAntennaN0A4->SetBeamwidth(10);
//	parabolicAntennaN0A4->SetOrientation(270);
//	swpN04->SetAntenna(parabolicAntennaN0A4);


	//Consumer node: 2

//	NetDeviceContainer netDeviceContainerParabolicN1Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN1A1 = netDeviceContainerParabolicN1Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN1A1 = DynamicCast<SpectrumWifiPhy>(wpN1A1);
//
//	//Set Parabolic Antenna to node 1
//	Ptr<ParabolicAntennaModel> parabolicAntenna1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntenna1->SetBeamwidth(10);
//	parabolicAntenna1->SetOrientation(0);
//	swpN1A1->SetAntenna(parabolicAntenna1);
//
//	NetDeviceContainer netDeviceContainerParabolicN1Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN1A2 = netDeviceContainerParabolicN1Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN1A2 = DynamicCast<SpectrumWifiPhy>(wpN1A2);
//
//	//Set Parabolic Antenna to node 1
//	Ptr<ParabolicAntennaModel> parabolicAntenna2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntenna2->SetBeamwidth(10);
//	parabolicAntenna2->SetOrientation(90);
//	swpN1A2->SetAntenna(parabolicAntenna2);
//
//	NetDeviceContainer netDeviceContainerParabolicN1Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN1A3 = netDeviceContainerParabolicN1Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN1A3 = DynamicCast<SpectrumWifiPhy>(wpN1A3);
//
//	//Set Parabolic Antenna to node 1
//	Ptr<ParabolicAntennaModel> parabolicAntenna3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntenna3->SetBeamwidth(10);
//	parabolicAntenna3->SetOrientation(180);
//	swpN1A3->SetAntenna(parabolicAntenna3);
//
	NetDeviceContainer netDeviceContainerParabolicN1Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN1A4 = netDeviceContainerParabolicN1Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN1A4 = DynamicCast<SpectrumWifiPhy>(wpN1A4);

	//Set Parabolic Antenna to node 1
	Ptr<ParabolicAntennaModel> parabolicAntenna4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntenna4->SetBeamwidth(359);
	parabolicAntenna4->SetOrientation(90);
	swpN1A4->SetAntenna(parabolicAntenna4);

	// Yans Devices
//	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//	NetDeviceContainer wifiNetDevices2 = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));


	//Install Mobility model
	ns2MobHelper.Install();

	//Install NDN stack
	NS_LOG_INFO("Installing NDN stack");
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.Install(mobileNodes);

	//Stategy
	ndn::StrategyChoiceHelper::Install(mobileNodes, "/", "/localhost/nfd/strategy/multicast");

	NS_LOG_INFO("Installing Applications");

	// Producer
	ndn::AppHelper producerHelper0("ns3::ndn::Producer");
	producerHelper0.SetPrefix("/beacon");
	producerHelper0.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper0.Install(mobileNodes.Get(0));
//	producerHelper0.SetTraceFile(traceFileString);

	//Consumer
 	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
	consumerHelper1.SetPrefix("/beacon");
	consumerHelper1.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper1.Install(mobileNodes.Get(1));
//	consumerHelper1.SetTraceFile(traceFileString);

	//Consumer
// 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
//	consumerHelper2.SetPrefix("/beacon");
//	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper2.Install(mobileNodes.Get(2));
//	consumerHelper2.SetTraceFile(traceFileString);

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
