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

	//MacHelper
	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

	//Mobility Helper
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-line-3n-far-Dist.txt");

	// Create Mobile nodes.
	NodeContainer mobileNodes;
	int nodeNr = 3;
	mobileNodes.Create(nodeNr);

	//Netdevice Container for Parabolic Antenna
	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wp0 = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swp0 = DynamicCast<SpectrumWifiPhy> (wp0);

	//Set Parabolic Antenna to node 0
	Ptr<ParabolicAntennaModel> parabolicAntenna0 = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna0->SetBeamwidth(10);
	parabolicAntenna0->SetOrientation(270);
	swp0->SetAntenna(parabolicAntenna0);

	NetDeviceContainer netDeviceContainerParabolicN1Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(1));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wp1 = netDeviceContainerParabolicN1Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swp1 = DynamicCast<SpectrumWifiPhy> (wp1);

	//Set Parabolic Antenna to node 1
	Ptr<ParabolicAntennaModel> parabolicAntenna1 = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna1->SetBeamwidth(10);
	parabolicAntenna1->SetOrientation(270);
	swp1->SetAntenna(parabolicAntenna1);


	NetDeviceContainer netDeviceContainerParabolicN2Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(2));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wp2 = netDeviceContainerParabolicN2Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swp2 = DynamicCast<SpectrumWifiPhy> (wp2);

	//Set Parabolic Antenna to node 2
	Ptr<ParabolicAntennaModel> parabolicAntenna2 = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna2->SetBeamwidth(10);
	parabolicAntenna2->SetOrientation(90);
	swp2->SetAntenna(parabolicAntenna2);

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

	//Consumer
 	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
	consumerHelper1.SetPrefix("/beacon");
	consumerHelper1.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper1.Install(mobileNodes.Get(1));

	//Consumer
 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/beacon");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper2.Install(mobileNodes.Get(2));

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
