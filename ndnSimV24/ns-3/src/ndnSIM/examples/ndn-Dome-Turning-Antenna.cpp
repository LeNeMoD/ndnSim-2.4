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
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Diamond-5n-with-moving.txt");

	// Create Mobile nodes.
	NodeContainer mobileNodes;
	int nodeNr = 3;
	mobileNodes.Create(nodeNr);

	//Netdevice Container for Parabolic Antenna
	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install (spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wp = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swp = DynamicCast<SpectrumWifiPhy> (wp);

	//Set Parabolic Antenna to node
	Ptr<ParabolicAntennaModel> parabolicAntenna = CreateObject<ParabolicAntennaModel>();
	parabolicAntenna->SetBeamwidth(10);
	parabolicAntenna->SetOrientation(0);
	swp->SetAntenna(parabolicAntenna);


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
	ndn::AppHelper producerHelper("ns3::ndn::Producer");
	producerHelper.SetPrefix("/beacon");
	producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper.Install(mobileNodes.Get(0));

	//Consumer
 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/beacon");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper2.Install(mobileNodes.Get(1));


	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
