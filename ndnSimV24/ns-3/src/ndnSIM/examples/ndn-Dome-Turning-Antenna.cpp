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

//	std::string traceFileString = "ns-movements-RSU-To-Moving-2n.txt";
//	std::string traceFileString = "ns-movements-stationary-20nodes.txt";
	std::string traceFileString = "ns-movements-TestTraceFile1.txt";



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


//	//Yans Wifi For interests
//
//	YansWifiChannelHelper wifiChannel;
//	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
//	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
//	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
//
//	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
//	wifiPhyHelper.SetChannel(wifiChannel.Create());
//	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
//	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	//MacHelper
	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

	//Mobility Helper
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper(traceFileString);

	// Create Mobile nodes.
	NodeContainer mobileNodes;
//	mobileNodes.setTraceFileString(traceFileString);
	int nodeNr = 9;
	mobileNodes.Create(nodeNr);
//	int nDevicesPerNode = 4;
//
//	for(int i = 0 ; i<nodeNr ; i++){
//		for(int devices = 0 ; devices< nDevicesPerNode ; devices++){
//			std::string netDeviceName = "netDevice"+devices+"Node"+nodeNr;
//			NetDeviceContainer netDeviceName = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(i));
//
//		}
//	}


//	//Producer node 0 -------------------- 1netDev -1 antenna at first 360
//	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install(spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN0A1 = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN0A1 = DynamicCast<SpectrumWifiPhy>(wpN0A1);
//
//	//Set Parabolic Antenna1 to node 0
//	Ptr<ParabolicAntennaModel> prodParabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
//
//	prodParabolicAntenna1->SetBeamwidth(90);
//	prodParabolicAntenna1->SetOrientation(90);
//	swpN0A1->SetAntenna(prodParabolicAntenna1);


//	//Producer node 0 -------------------- 1netDev -4 antennas
//	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN0AX = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN0AX = DynamicCast<SpectrumWifiPhy>(wpN0AX);
//
//	//Set Parabolic Antenna1 to node 0
//	Ptr<ParabolicAntennaModel> prodParabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> prodParabolicAntenna2 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> prodParabolicAntenna3 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> prodParabolicAntenna4 = CreateObject<ParabolicAntennaModel>();
//
//	prodParabolicAntenna1->SetBeamwidth(90);
//	prodParabolicAntenna1->SetOrientation(0);
//	swpN0AX->SetAntenna(prodParabolicAntenna1);
//
//	prodParabolicAntenna2->SetBeamwidth(90);
//	prodParabolicAntenna2->SetOrientation(90);
//	swpN0AX->SetAntenna(prodParabolicAntenna2);
//
//	prodParabolicAntenna3->SetBeamwidth(90);
//	prodParabolicAntenna3->SetOrientation(180);
//	swpN0AX->SetAntenna(prodParabolicAntenna3);
//
//	prodParabolicAntenna4->SetBeamwidth(90);
//	prodParabolicAntenna4->SetOrientation(270);
//	swpN0AX->SetAntenna(prodParabolicAntenna4);


	//Producer------------------- 4 Net dev 4 Antennas
	//NDev node 0 Parabolic 1
	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A1 = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A1 = DynamicCast<SpectrumWifiPhy> (wpN0A1);

	//Set Parabolic Antenna1 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntenna0 = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna0->SetBeamwidth(90);
	parabolicAntenna0->SetOrientation(0);
	swpN0A1->SetAntenna(parabolicAntenna0);

	//NDev node 0 Parabolic 2
	NetDeviceContainer netDeviceContainerParabolicN0Dev2 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A2 = netDeviceContainerParabolicN0Dev2.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A2 = DynamicCast<SpectrumWifiPhy> (wpN0A2);

	//Set Parabolic Antenna2 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A2 = CreateObject<ParabolicAntennaModel>();
	parabolicAntennaN0A2->SetBeamwidth(90);
	parabolicAntennaN0A2->SetOrientation(90);
	swpN0A2->SetAntenna(parabolicAntennaN0A2);

	//NDev node 0 Parabolic 3
	NetDeviceContainer netDeviceContainerParabolicN0Dev3 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A3 = netDeviceContainerParabolicN0Dev3.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A3 = DynamicCast<SpectrumWifiPhy> (wpN0A3);

	//Set Parabolic Antenna3 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A3 = CreateObject<ParabolicAntennaModel>();
	parabolicAntennaN0A3->SetBeamwidth(90);
	parabolicAntennaN0A3->SetOrientation(180);
	swpN0A3->SetAntenna(parabolicAntennaN0A3);

	//NDev node 0 Parabolic 4
	NetDeviceContainer netDeviceContainerParabolicN0Dev4 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A4 = netDeviceContainerParabolicN0Dev4.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A4 = DynamicCast<SpectrumWifiPhy> (wpN0A4);

	//Set Parabolic Antenna4 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A4 = CreateObject<ParabolicAntennaModel>();
	parabolicAntennaN0A4->SetBeamwidth(90);
	parabolicAntennaN0A4->SetOrientation(270);
	swpN0A4->SetAntenna(parabolicAntennaN0A4);


//	//Consumer node: 2 ----------------- 1 net device 1 antenna at first 360
//
//	NetDeviceContainer netDeviceContainerParabolicN1Dev1 = wifi.Install(spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN1A1 = netDeviceContainerParabolicN1Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN1A1 = DynamicCast<SpectrumWifiPhy>(wpN1A1);
//
//	//Set Parabolic Antennas to node 1
//	Ptr<ParabolicAntennaModel> consParabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
//
//	consParabolicAntenna1->SetBeamwidth(90);
//	consParabolicAntenna1->SetOrientation(270);
//	swpN1A1->SetAntenna(consParabolicAntenna1);


//	//Consumer node: 2 ----------------- 1 net device 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN1Dev1 = wifi.Install(spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN1AX = netDeviceContainerParabolicN1Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN1AX = DynamicCast<SpectrumWifiPhy>(wpN1AX);
//
//	//Set Parabolic Antennas to node 1
//	Ptr<ParabolicAntennaModel> consParabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> consParabolicAntenna2 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> consParabolicAntenna3 = CreateObject<ParabolicAntennaModel>();
//	Ptr<ParabolicAntennaModel> consParabolicAntenna4 = CreateObject<ParabolicAntennaModel>();
//
//	consParabolicAntenna1->SetBeamwidth(90);
//	consParabolicAntenna1->SetOrientation(0);
//	swpN1AX->SetAntenna(consParabolicAntenna1);
//
//	consParabolicAntenna2->SetBeamwidth(90);
//	consParabolicAntenna2->SetOrientation(90);
//	swpN1AX->SetAntenna(consParabolicAntenna2);
//
//	consParabolicAntenna3->SetBeamwidth(90);
//	consParabolicAntenna3->SetOrientation(180);
//	swpN1AX->SetAntenna(consParabolicAntenna3);
//
//	consParabolicAntenna4->SetBeamwidth(90);
//	consParabolicAntenna4->SetOrientation(270);
//	swpN1AX->SetAntenna(consParabolicAntenna4);


	//Consumer node: 1 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN1Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN1A1 = netDeviceContainerParabolicN1Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN1A1 = DynamicCast<SpectrumWifiPhy>(wpN1A1);

	//Set Parabolic Antenna1 to node 1
	Ptr<ParabolicAntennaModel> parabolicAntennaN1A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN1A1->SetBeamwidth(90);
	parabolicAntennaN1A1->SetOrientation(0);
	swpN1A1->SetAntenna(parabolicAntennaN1A1);

	NetDeviceContainer netDeviceContainerParabolicN1Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN1A2 = netDeviceContainerParabolicN1Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN1A2 = DynamicCast<SpectrumWifiPhy>(wpN1A2);

	//Set Parabolic Antenna2 to node 1
	Ptr<ParabolicAntennaModel> parabolicAntennaN1A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN1A2->SetBeamwidth(90);
	parabolicAntennaN1A2->SetOrientation(90);
	swpN1A2->SetAntenna(parabolicAntennaN1A2);

	NetDeviceContainer netDeviceContainerParabolicN1Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN1A3 = netDeviceContainerParabolicN1Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN1A3 = DynamicCast<SpectrumWifiPhy>(wpN1A3);

	//Set Parabolic Antenna3 to node 1
	Ptr<ParabolicAntennaModel> parabolicAntennaN1A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN1A3->SetBeamwidth(90);
	parabolicAntennaN1A3->SetOrientation(180);
	swpN1A3->SetAntenna(parabolicAntennaN1A3);

	NetDeviceContainer netDeviceContainerParabolicN1Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN1A4 = netDeviceContainerParabolicN1Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN1A4 = DynamicCast<SpectrumWifiPhy>(wpN1A4);

	//Set Parabolic Antenna4 to node 1
	Ptr<ParabolicAntennaModel> parabolicAntennaN1A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN1A4->SetBeamwidth(90);
	parabolicAntennaN1A4->SetOrientation(270);
	swpN1A4->SetAntenna(parabolicAntennaN1A4);


	//Consumer node: 2 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN2Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN2A1 = netDeviceContainerParabolicN2Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN2A1 = DynamicCast<SpectrumWifiPhy>(wpN2A1);

	//Set Parabolic Antenna1 to node 2
	Ptr<ParabolicAntennaModel> parabolicAntennaN2A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN2A1->SetBeamwidth(90);
	parabolicAntennaN2A1->SetOrientation(0);
	swpN2A1->SetAntenna(parabolicAntennaN2A1);

	NetDeviceContainer netDeviceContainerParabolicN2Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN2A2 = netDeviceContainerParabolicN2Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN2A2 = DynamicCast<SpectrumWifiPhy>(wpN2A2);

	//Set Parabolic Antenna2 to node 2
	Ptr<ParabolicAntennaModel> parabolicAntennaN2A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN2A2->SetBeamwidth(90);
	parabolicAntennaN2A2->SetOrientation(90);
	swpN2A2->SetAntenna(parabolicAntennaN2A2);

	NetDeviceContainer netDeviceContainerParabolicN2Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN2A3 = netDeviceContainerParabolicN2Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN2A3 = DynamicCast<SpectrumWifiPhy>(wpN2A3);

	//Set Parabolic Antenna3 to node 2
	Ptr<ParabolicAntennaModel> parabolicAntennaN2A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN2A3->SetBeamwidth(90);
	parabolicAntennaN2A3->SetOrientation(180);
	swpN2A3->SetAntenna(parabolicAntennaN2A3);

	NetDeviceContainer netDeviceContainerParabolicN2Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN2A4 = netDeviceContainerParabolicN2Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN2A4 = DynamicCast<SpectrumWifiPhy>(wpN2A4);

	//Set Parabolic Antenna4 to node 2
	Ptr<ParabolicAntennaModel> parabolicAntennaN2A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN2A4->SetBeamwidth(90);
	parabolicAntennaN2A4->SetOrientation(270);
	swpN2A4->SetAntenna(parabolicAntennaN2A4);

	//Consumer node: 3 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN3Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN3A1 = netDeviceContainerParabolicN3Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN3A1 = DynamicCast<SpectrumWifiPhy>(wpN3A1);

	//Set Parabolic Antenna1 to node 3
	Ptr<ParabolicAntennaModel> parabolicAntennaN3A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN3A1->SetBeamwidth(90);
	parabolicAntennaN3A1->SetOrientation(0);
	swpN3A1->SetAntenna(parabolicAntennaN3A1);

	NetDeviceContainer netDeviceContainerParabolicN3Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN3A2 = netDeviceContainerParabolicN3Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN3A2 = DynamicCast<SpectrumWifiPhy>(wpN3A2);

	//Set Parabolic Antenna2 to node 3
	Ptr<ParabolicAntennaModel> parabolicAntennaN3A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN3A2->SetBeamwidth(90);
	parabolicAntennaN3A2->SetOrientation(90);
	swpN3A2->SetAntenna(parabolicAntennaN3A2);

	NetDeviceContainer netDeviceContainerParabolicN3Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN3A3 = netDeviceContainerParabolicN3Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN3A3 = DynamicCast<SpectrumWifiPhy>(wpN3A3);

	//Set Parabolic Antenna3 to node 3
	Ptr<ParabolicAntennaModel> parabolicAntennaN3A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN3A3->SetBeamwidth(90);
	parabolicAntennaN3A3->SetOrientation(180);
	swpN3A3->SetAntenna(parabolicAntennaN3A3);

	NetDeviceContainer netDeviceContainerParabolicN3Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(3));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN3A4 = netDeviceContainerParabolicN3Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN3A4 = DynamicCast<SpectrumWifiPhy>(wpN3A4);

	//Set Parabolic Antenna4 to node 3
	Ptr<ParabolicAntennaModel> parabolicAntennaN3A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN3A4->SetBeamwidth(90);
	parabolicAntennaN3A4->SetOrientation(270);
	swpN3A4->SetAntenna(parabolicAntennaN3A4);

	//Consumer node: 4 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN4Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN4A1 = netDeviceContainerParabolicN4Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN4A1 = DynamicCast<SpectrumWifiPhy>(wpN4A1);

	//Set Parabolic Antenna1 to node 4
	Ptr<ParabolicAntennaModel> parabolicAntennaN4A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN4A1->SetBeamwidth(90);
	parabolicAntennaN4A1->SetOrientation(0);
	swpN4A1->SetAntenna(parabolicAntennaN4A1);

	NetDeviceContainer netDeviceContainerParabolicN4Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN4A2 = netDeviceContainerParabolicN4Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN4A2 = DynamicCast<SpectrumWifiPhy>(wpN4A2);

	//Set Parabolic Antenna2 to node 4
	Ptr<ParabolicAntennaModel> parabolicAntennaN4A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN4A2->SetBeamwidth(90);
	parabolicAntennaN4A2->SetOrientation(90);
	swpN4A2->SetAntenna(parabolicAntennaN4A2);

	NetDeviceContainer netDeviceContainerParabolicN4Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN4A3 = netDeviceContainerParabolicN4Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN4A3 = DynamicCast<SpectrumWifiPhy>(wpN4A3);

	//Set Parabolic Antenna3 to node 4
	Ptr<ParabolicAntennaModel> parabolicAntennaN4A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN4A3->SetBeamwidth(90);
	parabolicAntennaN4A3->SetOrientation(180);
	swpN4A3->SetAntenna(parabolicAntennaN4A3);

	NetDeviceContainer netDeviceContainerParabolicN4Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(4));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN4A4 = netDeviceContainerParabolicN4Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN4A4 = DynamicCast<SpectrumWifiPhy>(wpN4A4);

	//Set Parabolic Antenna4 to node 4
	Ptr<ParabolicAntennaModel> parabolicAntennaN4A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN4A4->SetBeamwidth(90);
	parabolicAntennaN4A4->SetOrientation(270);
	swpN4A4->SetAntenna(parabolicAntennaN4A4);

	//Consumer node: 5 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN5Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(5));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN5A1 = netDeviceContainerParabolicN5Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN5A1 = DynamicCast<SpectrumWifiPhy>(wpN5A1);

	//Set Parabolic Antenna1 to node 5
	Ptr<ParabolicAntennaModel> parabolicAntennaN5A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN5A1->SetBeamwidth(90);
	parabolicAntennaN5A1->SetOrientation(0);
	swpN5A1->SetAntenna(parabolicAntennaN5A1);

	NetDeviceContainer netDeviceContainerParabolicN5Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(5));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN5A2 = netDeviceContainerParabolicN5Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN5A2 = DynamicCast<SpectrumWifiPhy>(wpN5A2);

	//Set Parabolic Antenna2 to node 5
	Ptr<ParabolicAntennaModel> parabolicAntennaN5A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN5A2->SetBeamwidth(90);
	parabolicAntennaN5A2->SetOrientation(90);
	swpN5A2->SetAntenna(parabolicAntennaN5A2);

	NetDeviceContainer netDeviceContainerParabolicN5Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(5));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN5A3 = netDeviceContainerParabolicN5Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN5A3 = DynamicCast<SpectrumWifiPhy>(wpN5A3);

	//Set Parabolic Antenna3 to node 5
	Ptr<ParabolicAntennaModel> parabolicAntennaN5A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN5A3->SetBeamwidth(90);
	parabolicAntennaN5A3->SetOrientation(180);
	swpN5A3->SetAntenna(parabolicAntennaN5A3);

	NetDeviceContainer netDeviceContainerParabolicN5Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(5));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN5A4 = netDeviceContainerParabolicN5Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN5A4 = DynamicCast<SpectrumWifiPhy>(wpN5A4);

	//Set Parabolic Antenna4 to node 5
	Ptr<ParabolicAntennaModel> parabolicAntennaN5A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN5A4->SetBeamwidth(90);
	parabolicAntennaN5A4->SetOrientation(270);
	swpN5A4->SetAntenna(parabolicAntennaN5A4);

	//Consumer node: 6 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN6Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(6));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN6A1 = netDeviceContainerParabolicN6Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN6A1 = DynamicCast<SpectrumWifiPhy>(wpN6A1);

	//Set Parabolic Antenna1 to node 6
	Ptr<ParabolicAntennaModel> parabolicAntennaN6A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN6A1->SetBeamwidth(90);
	parabolicAntennaN6A1->SetOrientation(0);
	swpN6A1->SetAntenna(parabolicAntennaN6A1);

	NetDeviceContainer netDeviceContainerParabolicN6Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(6));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN6A2 = netDeviceContainerParabolicN6Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN6A2 = DynamicCast<SpectrumWifiPhy>(wpN6A2);

	//Set Parabolic Antenna2 to node 6
	Ptr<ParabolicAntennaModel> parabolicAntennaN6A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN6A2->SetBeamwidth(90);
	parabolicAntennaN6A2->SetOrientation(90);
	swpN6A2->SetAntenna(parabolicAntennaN6A2);

	NetDeviceContainer netDeviceContainerParabolicN6Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(6));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN6A3 = netDeviceContainerParabolicN6Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN6A3 = DynamicCast<SpectrumWifiPhy>(wpN6A3);

	//Set Parabolic Antenna3 to node 6
	Ptr<ParabolicAntennaModel> parabolicAntennaN6A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN6A3->SetBeamwidth(90);
	parabolicAntennaN6A3->SetOrientation(180);
	swpN6A3->SetAntenna(parabolicAntennaN6A3);

	NetDeviceContainer netDeviceContainerParabolicN6Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(6));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN6A4 = netDeviceContainerParabolicN6Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN6A4 = DynamicCast<SpectrumWifiPhy>(wpN6A4);

	//Set Parabolic Antenna4 to node 6
	Ptr<ParabolicAntennaModel> parabolicAntennaN6A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN6A4->SetBeamwidth(90);
	parabolicAntennaN6A4->SetOrientation(270);
	swpN6A4->SetAntenna(parabolicAntennaN6A4);


	//Consumer node: 7 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN7Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(7));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN7A1 = netDeviceContainerParabolicN7Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN7A1 = DynamicCast<SpectrumWifiPhy>(wpN7A1);

	//Set Parabolic Antenna1 to node 7
	Ptr<ParabolicAntennaModel> parabolicAntennaN7N1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN7N1->SetBeamwidth(90);
	parabolicAntennaN7N1->SetOrientation(0);
	swpN7A1->SetAntenna(parabolicAntennaN7N1);

	NetDeviceContainer netDeviceContainerParabolicN7Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(7));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN7A2 = netDeviceContainerParabolicN7Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN7A2 = DynamicCast<SpectrumWifiPhy>(wpN7A2);

	//Set Parabolic Antenna2 to node 7
	Ptr<ParabolicAntennaModel> parabolicAntennaN7N2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN7N2->SetBeamwidth(90);
	parabolicAntennaN7N2->SetOrientation(90);
	swpN7A2->SetAntenna(parabolicAntennaN7N2);

	NetDeviceContainer netDeviceContainerParabolicN7Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(7));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN7A3 = netDeviceContainerParabolicN7Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN7A3 = DynamicCast<SpectrumWifiPhy>(wpN7A3);

	//Set Parabolic Antenna3 to node 7
	Ptr<ParabolicAntennaModel> parabolicAntennaN7N3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN7N3->SetBeamwidth(90);
	parabolicAntennaN7N3->SetOrientation(180);
	swpN7A3->SetAntenna(parabolicAntennaN7N3);

	NetDeviceContainer netDeviceContainerParabolicN7Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(7));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN7A4 = netDeviceContainerParabolicN7Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN7A4 = DynamicCast<SpectrumWifiPhy>(wpN7A4);

	//Set Parabolic Antenna4 to node 7
	Ptr<ParabolicAntennaModel> parabolicAntennaN7N4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN7N4->SetBeamwidth(90);
	parabolicAntennaN7N4->SetOrientation(270);
	swpN7A4->SetAntenna(parabolicAntennaN7N4);

	//Consumer node: 8 --------------------- 4 net devices 4 antennas

	NetDeviceContainer netDeviceContainerParabolicN8Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(8));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN8A1 = netDeviceContainerParabolicN8Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN8A1 = DynamicCast<SpectrumWifiPhy>(wpN8A1);

	//Set Parabolic Antenna1 to node 8
	Ptr<ParabolicAntennaModel> parabolicAntennaN8A1 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN8A1->SetBeamwidth(90);
	parabolicAntennaN8A1->SetOrientation(0);
	swpN8A1->SetAntenna(parabolicAntennaN8A1);

	NetDeviceContainer netDeviceContainerParabolicN8Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(8));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN8A2 = netDeviceContainerParabolicN8Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN8A2 = DynamicCast<SpectrumWifiPhy>(wpN8A2);

	//Set Parabolic Antenna2 to node 8
	Ptr<ParabolicAntennaModel> parabolicAntennaN8A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN8A2->SetBeamwidth(90);
	parabolicAntennaN8A2->SetOrientation(90);
	swpN8A2->SetAntenna(parabolicAntennaN8A2);

	NetDeviceContainer netDeviceContainerParabolicN8Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(8));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN8A3 = netDeviceContainerParabolicN8Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN8A3 = DynamicCast<SpectrumWifiPhy>(wpN8A3);

	//Set Parabolic Antenna3 to node 8
	Ptr<ParabolicAntennaModel> parabolicAntennaN8A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN8A3->SetBeamwidth(90);
	parabolicAntennaN8A3->SetOrientation(180);
	swpN8A3->SetAntenna(parabolicAntennaN8A3);

	NetDeviceContainer netDeviceContainerParabolicN8Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(8));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN8A4 = netDeviceContainerParabolicN8Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN8A4 = DynamicCast<SpectrumWifiPhy>(wpN8A4);

	//Set Parabolic Antenna4 to node 8
	Ptr<ParabolicAntennaModel> parabolicAntennaN8A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN8A4->SetBeamwidth(90);
	parabolicAntennaN8A4->SetOrientation(270);
	swpN8A4->SetAntenna(parabolicAntennaN8A4);

//	//Consumer node: 9 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN9Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(9));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN9A1 = netDeviceContainerParabolicN9Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN9A1 = DynamicCast<SpectrumWifiPhy>(wpN9A1);
//
//	//Set Parabolic Antenna1 to node 9
//	Ptr<ParabolicAntennaModel> parabolicAntennaN9N1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN9N1->SetBeamwidth(90);
//	parabolicAntennaN9N1->SetOrientation(0);
//	swpN9A1->SetAntenna(parabolicAntennaN9N1);
//
//	NetDeviceContainer netDeviceContainerParabolicN9Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(9));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN9A2 = netDeviceContainerParabolicN9Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN9A2 = DynamicCast<SpectrumWifiPhy>(wpN9A2);
//
//	//Set Parabolic Antenna2 to node 9
//	Ptr<ParabolicAntennaModel> parabolicAntennaN9N2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN9N2->SetBeamwidth(90);
//	parabolicAntennaN9N2->SetOrientation(90);
//	swpN9A2->SetAntenna(parabolicAntennaN9N2);
//
//	NetDeviceContainer netDeviceContainerParabolicN9Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(9));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN9A3 = netDeviceContainerParabolicN9Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN9A3 = DynamicCast<SpectrumWifiPhy>(wpN9A3);
//
//	//Set Parabolic Antenna3 to node 9
//	Ptr<ParabolicAntennaModel> parabolicAntennaN9N3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN9N3->SetBeamwidth(90);
//	parabolicAntennaN9N3->SetOrientation(180);
//	swpN9A3->SetAntenna(parabolicAntennaN9N3);
//
//	NetDeviceContainer netDeviceContainerParabolicN9Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(9));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN9A4 = netDeviceContainerParabolicN9Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN9A4 = DynamicCast<SpectrumWifiPhy>(wpN9A4);
//
//	//Set Parabolic Antenna4 to node 9
//	Ptr<ParabolicAntennaModel> parabolicAntennaN9N4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN9N4->SetBeamwidth(90);
//	parabolicAntennaN9N4->SetOrientation(270);
//	swpN9A4->SetAntenna(parabolicAntennaN9N4);

//	//Consumer node: 10 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN10Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(10));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN10A1 = netDeviceContainerParabolicN10Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN10A1 = DynamicCast<SpectrumWifiPhy>(wpN10A1);
//
//	//Set Parabolic Antenna1 to node 10
//	Ptr<ParabolicAntennaModel> parabolicAntennaN10A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN10A1->SetBeamwidth(90);
//	parabolicAntennaN10A1->SetOrientation(0);
//	swpN10A1->SetAntenna(parabolicAntennaN10A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN10Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(10));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN10A2 = netDeviceContainerParabolicN10Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN10A2 = DynamicCast<SpectrumWifiPhy>(wpN10A2);
//
//	//Set Parabolic Antenna2 to node 10
//	Ptr<ParabolicAntennaModel> parabolicAntennaN10A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN10A2->SetBeamwidth(90);
//	parabolicAntennaN10A2->SetOrientation(90);
//	swpN10A2->SetAntenna(parabolicAntennaN10A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN10Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(10));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN10A3 = netDeviceContainerParabolicN10Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN10A3 = DynamicCast<SpectrumWifiPhy>(wpN10A3);
//
//	//Set Parabolic Antenna3 to node 10
//	Ptr<ParabolicAntennaModel> parabolicAntennaN10A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN10A3->SetBeamwidth(90);
//	parabolicAntennaN10A3->SetOrientation(180);
//	swpN10A3->SetAntenna(parabolicAntennaN10A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN10Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(10));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN10A4 = netDeviceContainerParabolicN10Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN10A4 = DynamicCast<SpectrumWifiPhy>(wpN10A4);
//
//	//Set Parabolic Antenna4 to node 10
//	Ptr<ParabolicAntennaModel> parabolicAntennaN10A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN10A4->SetBeamwidth(90);
//	parabolicAntennaN10A4->SetOrientation(270);
//	swpN10A4->SetAntenna(parabolicAntennaN10A4);
//
//	//Consumer node: 11 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN11Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(11));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN11A1 = netDeviceContainerParabolicN11Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN11A1 = DynamicCast<SpectrumWifiPhy>(wpN11A1);
//
//	//Set Parabolic Antenna1 to node 11
//	Ptr<ParabolicAntennaModel> parabolicAntennaN11A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN11A1->SetBeamwidth(90);
//	parabolicAntennaN11A1->SetOrientation(0);
//	swpN11A1->SetAntenna(parabolicAntennaN11A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN11Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(11));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN11A2 = netDeviceContainerParabolicN11Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN11A2 = DynamicCast<SpectrumWifiPhy>(wpN11A2);
//
//	//Set Parabolic Antenna2 to node 11
//	Ptr<ParabolicAntennaModel> parabolicAntennaN11A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN11A2->SetBeamwidth(90);
//	parabolicAntennaN11A2->SetOrientation(90);
//	swpN11A2->SetAntenna(parabolicAntennaN11A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN11Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(11));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN11A3 = netDeviceContainerParabolicN11Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN11A3 = DynamicCast<SpectrumWifiPhy>(wpN11A3);
//
//	//Set Parabolic Antenna3 to node 11
//	Ptr<ParabolicAntennaModel> parabolicAntennaN11A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN11A3->SetBeamwidth(90);
//	parabolicAntennaN11A3->SetOrientation(180);
//	swpN11A3->SetAntenna(parabolicAntennaN11A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN11Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(11));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN11A4 = netDeviceContainerParabolicN11Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN11A4 = DynamicCast<SpectrumWifiPhy>(wpN11A4);
//
//	//Set Parabolic Antenna4 to node 11
//	Ptr<ParabolicAntennaModel> parabolicAntennaN11A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN11A4->SetBeamwidth(90);
//	parabolicAntennaN11A4->SetOrientation(270);
//	swpN11A4->SetAntenna(parabolicAntennaN11A4);
//
//	//Consumer node: 12 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN12Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(12));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN12A1 = netDeviceContainerParabolicN12Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN12A1 = DynamicCast<SpectrumWifiPhy>(wpN12A1);
//
//	//Set Parabolic Antenna1 to node 12
//	Ptr<ParabolicAntennaModel> parabolicAntennaN12A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN12A1->SetBeamwidth(90);
//	parabolicAntennaN12A1->SetOrientation(0);
//	swpN12A1->SetAntenna(parabolicAntennaN12A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN12Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(12));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN12A2 = netDeviceContainerParabolicN12Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN12A2 = DynamicCast<SpectrumWifiPhy>(wpN12A2);
//
//	//Set Parabolic Antenna2 to node 12
//	Ptr<ParabolicAntennaModel> parabolicAntennaN12A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN12A2->SetBeamwidth(90);
//	parabolicAntennaN12A2->SetOrientation(90);
//	swpN12A2->SetAntenna(parabolicAntennaN12A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN12Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(12));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN12A3 = netDeviceContainerParabolicN12Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN12A3 = DynamicCast<SpectrumWifiPhy>(wpN12A3);
//
//	//Set Parabolic Antenna3 to node 12
//	Ptr<ParabolicAntennaModel> parabolicAntennaN12A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN12A3->SetBeamwidth(90);
//	parabolicAntennaN12A3->SetOrientation(180);
//	swpN12A3->SetAntenna(parabolicAntennaN12A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN12Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(12));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN12A4 = netDeviceContainerParabolicN12Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN12A4 = DynamicCast<SpectrumWifiPhy>(wpN12A4);
//
//	//Set Parabolic Antenna4 to node 12
//	Ptr<ParabolicAntennaModel> parabolicAntennaN12A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN12A4->SetBeamwidth(90);
//	parabolicAntennaN12A4->SetOrientation(270);
//	swpN12A4->SetAntenna(parabolicAntennaN12A4);
//
//	//Consumer node: 13 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN13Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(13));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN13A1 = netDeviceContainerParabolicN13Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN13A1 = DynamicCast<SpectrumWifiPhy>(wpN13A1);
//
//	//Set Parabolic Antenna1 to node 13
//	Ptr<ParabolicAntennaModel> parabolicAntennaN13A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN13A1->SetBeamwidth(90);
//	parabolicAntennaN13A1->SetOrientation(0);
//	swpN13A1->SetAntenna(parabolicAntennaN13A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN13Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(13));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN13A2 = netDeviceContainerParabolicN13Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN13A2 = DynamicCast<SpectrumWifiPhy>(wpN13A2);
//
//	//Set Parabolic Antenna2 to node 13
//	Ptr<ParabolicAntennaModel> parabolicAntennaN13A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN13A2->SetBeamwidth(90);
//	parabolicAntennaN13A2->SetOrientation(90);
//	swpN13A2->SetAntenna(parabolicAntennaN13A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN13Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(13));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN13A3 = netDeviceContainerParabolicN13Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN13A3 = DynamicCast<SpectrumWifiPhy>(wpN13A3);
//
//	//Set Parabolic Antenna3 to node 13
//	Ptr<ParabolicAntennaModel> parabolicAntennaN13A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN13A3->SetBeamwidth(90);
//	parabolicAntennaN13A3->SetOrientation(180);
//	swpN13A3->SetAntenna(parabolicAntennaN13A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN13Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(13));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN13A4 = netDeviceContainerParabolicN13Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN13A4 = DynamicCast<SpectrumWifiPhy>(wpN13A4);
//
//	//Set Parabolic Antenna4 to node 13
//	Ptr<ParabolicAntennaModel> parabolicAntennaN13A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN13A4->SetBeamwidth(90);
//	parabolicAntennaN13A4->SetOrientation(270);
//	swpN13A4->SetAntenna(parabolicAntennaN13A4);
//
//	//Consumer node: 14 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN14Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(14));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN14A1 = netDeviceContainerParabolicN14Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN14A1 = DynamicCast<SpectrumWifiPhy>(wpN14A1);
//
//	//Set Parabolic Antenna1 to node 14
//	Ptr<ParabolicAntennaModel> parabolicAntennaN14A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN14A1->SetBeamwidth(90);
//	parabolicAntennaN14A1->SetOrientation(0);
//	swpN14A1->SetAntenna(parabolicAntennaN14A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN14Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(14));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN14A2 = netDeviceContainerParabolicN14Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN14A2 = DynamicCast<SpectrumWifiPhy>(wpN14A2);
//
//	//Set Parabolic Antenna2 to node 14
//	Ptr<ParabolicAntennaModel> parabolicAntennaN14A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN14A2->SetBeamwidth(90);
//	parabolicAntennaN14A2->SetOrientation(90);
//	swpN14A2->SetAntenna(parabolicAntennaN14A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN14Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(14));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN14A3 = netDeviceContainerParabolicN14Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN14A3 = DynamicCast<SpectrumWifiPhy>(wpN14A3);
//
//	//Set Parabolic Antenna3 to node 14
//	Ptr<ParabolicAntennaModel> parabolicAntennaN14A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN14A3->SetBeamwidth(90);
//	parabolicAntennaN14A3->SetOrientation(180);
//	swpN14A3->SetAntenna(parabolicAntennaN14A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN14Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(14));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN14A4 = netDeviceContainerParabolicN14Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN14A4 = DynamicCast<SpectrumWifiPhy>(wpN14A4);
//
//	//Set Parabolic Antenna4 to node 14
//	Ptr<ParabolicAntennaModel> parabolicAntennaN14A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN14A4->SetBeamwidth(90);
//	parabolicAntennaN14A4->SetOrientation(270);
//	swpN14A4->SetAntenna(parabolicAntennaN14A4);
//
//	//Consumer node: 15 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN15Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(15));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN15A1 = netDeviceContainerParabolicN15Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN15A1 = DynamicCast<SpectrumWifiPhy>(wpN15A1);
//
//	//Set Parabolic Antenna1 to node 15
//	Ptr<ParabolicAntennaModel> parabolicAntennaN15A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN15A1->SetBeamwidth(90);
//	parabolicAntennaN15A1->SetOrientation(0);
//	swpN15A1->SetAntenna(parabolicAntennaN15A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN15Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(15));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN15A2 = netDeviceContainerParabolicN15Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN15A2 = DynamicCast<SpectrumWifiPhy>(wpN15A2);
//
//	//Set Parabolic Antenna2 to node 15
//	Ptr<ParabolicAntennaModel> parabolicAntennaN15A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN15A2->SetBeamwidth(90);
//	parabolicAntennaN15A2->SetOrientation(90);
//	swpN15A2->SetAntenna(parabolicAntennaN15A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN15Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(15));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN15A3 = netDeviceContainerParabolicN15Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN15A3 = DynamicCast<SpectrumWifiPhy>(wpN15A3);
//
//	//Set Parabolic Antenna3 to node 15
//	Ptr<ParabolicAntennaModel> parabolicAntennaN15A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN15A3->SetBeamwidth(90);
//	parabolicAntennaN15A3->SetOrientation(180);
//	swpN15A3->SetAntenna(parabolicAntennaN15A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN15Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(15));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN15A4 = netDeviceContainerParabolicN15Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN15A4 = DynamicCast<SpectrumWifiPhy>(wpN15A4);
//
//	//Set Parabolic Antenna4 to node 15
//	Ptr<ParabolicAntennaModel> parabolicAntennaN15A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN15A4->SetBeamwidth(90);
//	parabolicAntennaN15A4->SetOrientation(270);
//	swpN15A4->SetAntenna(parabolicAntennaN15A4);
//
//	//Consumer node: 16 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN16Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(16));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN16A1 = netDeviceContainerParabolicN16Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN16A1 = DynamicCast<SpectrumWifiPhy>(wpN16A1);
//
//	//Set Parabolic Antenna1 to node 16
//	Ptr<ParabolicAntennaModel> parabolicAntennaN16A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN16A1->SetBeamwidth(90);
//	parabolicAntennaN16A1->SetOrientation(0);
//	swpN16A1->SetAntenna(parabolicAntennaN16A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN16Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(16));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN16A2 = netDeviceContainerParabolicN16Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN16A2 = DynamicCast<SpectrumWifiPhy>(wpN16A2);
//
//	//Set Parabolic Antenna2 to node 16
//	Ptr<ParabolicAntennaModel> parabolicAntennaN16A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN16A2->SetBeamwidth(90);
//	parabolicAntennaN16A2->SetOrientation(90);
//	swpN16A2->SetAntenna(parabolicAntennaN16A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN16Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(16));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN16A3 = netDeviceContainerParabolicN16Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN16A3 = DynamicCast<SpectrumWifiPhy>(wpN16A3);
//
//	//Set Parabolic Antenna3 to node 16
//	Ptr<ParabolicAntennaModel> parabolicAntennaN16A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN16A3->SetBeamwidth(90);
//	parabolicAntennaN16A3->SetOrientation(180);
//	swpN16A3->SetAntenna(parabolicAntennaN16A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN16Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(16));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN16A4 = netDeviceContainerParabolicN16Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN16A4 = DynamicCast<SpectrumWifiPhy>(wpN16A4);
//
//	//Set Parabolic Antenna4 to node 16
//	Ptr<ParabolicAntennaModel> parabolicAntennaN16A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN16A4->SetBeamwidth(90);
//	parabolicAntennaN16A4->SetOrientation(270);
//	swpN16A4->SetAntenna(parabolicAntennaN16A4);
//
//	//Consumer node: 17 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN17Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(17));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN17A1 = netDeviceContainerParabolicN17Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN17A1 = DynamicCast<SpectrumWifiPhy>(wpN17A1);
//
//	//Set Parabolic Antenna1 to node 17
//	Ptr<ParabolicAntennaModel> parabolicAntennaN17A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN17A1->SetBeamwidth(90);
//	parabolicAntennaN17A1->SetOrientation(0);
//	swpN17A1->SetAntenna(parabolicAntennaN17A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN17Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(17));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN17A2 = netDeviceContainerParabolicN17Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN17A2 = DynamicCast<SpectrumWifiPhy>(wpN17A2);
//
//	//Set Parabolic Antenna2 to node 17
//	Ptr<ParabolicAntennaModel> parabolicAntennaN17A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN17A2->SetBeamwidth(90);
//	parabolicAntennaN17A2->SetOrientation(90);
//	swpN17A2->SetAntenna(parabolicAntennaN17A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN17Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(17));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN17A3 = netDeviceContainerParabolicN17Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN17A3 = DynamicCast<SpectrumWifiPhy>(wpN17A3);
//
//	//Set Parabolic Antenna3 to node 17
//	Ptr<ParabolicAntennaModel> parabolicAntennaN17A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN17A3->SetBeamwidth(90);
//	parabolicAntennaN17A3->SetOrientation(180);
//	swpN17A3->SetAntenna(parabolicAntennaN17A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN17Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(17));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN17A4 = netDeviceContainerParabolicN17Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN17A4 = DynamicCast<SpectrumWifiPhy>(wpN17A4);
//
//	//Set Parabolic Antenna4 to node 17
//	Ptr<ParabolicAntennaModel> parabolicAntennaN17A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN17A4->SetBeamwidth(90);
//	parabolicAntennaN17A4->SetOrientation(270);
//	swpN17A4->SetAntenna(parabolicAntennaN17A4);
//
//	//Consumer node: 18 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN18Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(18));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN18A1 = netDeviceContainerParabolicN18Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN18A1 = DynamicCast<SpectrumWifiPhy>(wpN18A1);
//
//	//Set Parabolic Antenna1 to node 18
//	Ptr<ParabolicAntennaModel> parabolicAntennaN18A1 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN18A1->SetBeamwidth(90);
//	parabolicAntennaN18A1->SetOrientation(0);
//	swpN18A1->SetAntenna(parabolicAntennaN18A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN18Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(18));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN18A2 = netDeviceContainerParabolicN18Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN18A2 = DynamicCast<SpectrumWifiPhy>(wpN18A2);
//
//	//Set Parabolic Antenna2 to node 18
//	Ptr<ParabolicAntennaModel> parabolicAntennaN18A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN18A2->SetBeamwidth(90);
//	parabolicAntennaN18A2->SetOrientation(90);
//	swpN18A2->SetAntenna(parabolicAntennaN18A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN18Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(18));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN18A3 = netDeviceContainerParabolicN18Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN18A3 = DynamicCast<SpectrumWifiPhy>(wpN18A3);
//
//	//Set Parabolic Antenna3 to node 18
//	Ptr<ParabolicAntennaModel> parabolicAntennaN18A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN18A3->SetBeamwidth(90);
//	parabolicAntennaN18A3->SetOrientation(180);
//	swpN18A3->SetAntenna(parabolicAntennaN18A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN18Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(18));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN18A4 = netDeviceContainerParabolicN18Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN18A4 = DynamicCast<SpectrumWifiPhy>(wpN18A4);
//
//	//Set Parabolic Antenna4 to node 18
//	Ptr<ParabolicAntennaModel> parabolicAntennaN18A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN18A4->SetBeamwidth(90);
//	parabolicAntennaN18A4->SetOrientation(270);
//	swpN18A4->SetAntenna(parabolicAntennaN18A4);
//
//	//Consumer node: 19 --------------------- 4 net devices 4 antennas
//
//	NetDeviceContainer netDeviceContainerParabolicN19Dev1 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(19));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN19A1 = netDeviceContainerParabolicN19Dev1.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN19A1 = DynamicCast<SpectrumWifiPhy>(wpN19A1);
//
//	//Set Parabolic Antenna1 to node 19
//	Ptr<ParabolicAntennaModel> parabolicAntennaN19A1= CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN19A1->SetBeamwidth(90);
//	parabolicAntennaN19A1->SetOrientation(0);
//	swpN19A1->SetAntenna(parabolicAntennaN19A1);
//
//	NetDeviceContainer netDeviceContainerParabolicN19Dev2 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(19));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN19A2 = netDeviceContainerParabolicN19Dev2.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN19A2 = DynamicCast<SpectrumWifiPhy>(wpN19A2);
//
//	//Set Parabolic Antenna2 to node 19
//	Ptr<ParabolicAntennaModel> parabolicAntennaN19A2 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN19A2->SetBeamwidth(90);
//	parabolicAntennaN19A2->SetOrientation(90);
//	swpN19A2->SetAntenna(parabolicAntennaN19A2);
//
//	NetDeviceContainer netDeviceContainerParabolicN19Dev3 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(19));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN19A3 = netDeviceContainerParabolicN19Dev3.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN19A3 = DynamicCast<SpectrumWifiPhy>(wpN19A3);
//
//	//Set Parabolic Antenna3 to node 19
//	Ptr<ParabolicAntennaModel> parabolicAntennaN19A3 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN19A3->SetBeamwidth(90);
//	parabolicAntennaN19A3->SetOrientation(180);
//	swpN19A3->SetAntenna(parabolicAntennaN19A3);
//
//	NetDeviceContainer netDeviceContainerParabolicN19Dev4 = wifi.Install(
//			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(19));
//
//	//Set SpectrumPhy from WifiPhy
//	Ptr<WifiPhy> wpN19A4 = netDeviceContainerParabolicN19Dev4.Get(0)->GetObject<
//			WifiNetDevice>()->GetPhy();
//	Ptr<SpectrumWifiPhy> swpN19A4 = DynamicCast<SpectrumWifiPhy>(wpN19A4);
//
//	//Set Parabolic Antenna4 to node 19
//	Ptr<ParabolicAntennaModel> parabolicAntennaN19A4 = CreateObject<
//			ParabolicAntennaModel>();
//	parabolicAntennaN19A4->SetBeamwidth(90);
//	parabolicAntennaN19A4->SetOrientation(270);
//	swpN19A4->SetAntenna(parabolicAntennaN19A4);


//	// Yans Devices
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
	consumerHelper1.Install(mobileNodes.Get(8));
//	consumerHelper1.SetTraceFile(traceFileString);

//	//Consumer
// 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
//	consumerHelper2.SetPrefix("/beacon");
//	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper2.Install(mobileNodes.Get(15));
////	consumerHelper2.SetTraceFile(traceFileString);
//
//	//Consumer
// 	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
//	consumerHelper3.SetPrefix("/beacon");
//	consumerHelper3.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper3.Install(mobileNodes.Get(19));
//	consumerHelper3.SetTraceFile(traceFileString);


	Simulator::Stop(Seconds(30.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
