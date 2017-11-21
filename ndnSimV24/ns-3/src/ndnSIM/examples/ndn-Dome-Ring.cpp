

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/ndnSIM-module.h"

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

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
	wifiPhyHelper.SetChannel(wifiChannel.Create());
	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-3n.txt");
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-Ring-7n.txt");



	// Create Moble nodes.
	NodeContainer mobileNodes;
	mobileNodes.Create(7);

	// configure movements for each node, while reading trace file
	ns2MobHelper.Install();

	////////////////
	// 1. Install Wifi
	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes);
	// 2. Install Mobility model

	// 3. Install NDN stack
	NS_LOG_INFO("Installing NDN stack");
	ndn::StackHelper ndnHelper;
	// ndnHelper.AddNetDeviceFaceCreateCallback (WifiNetDevice::GetTypeId (), MakeCallback(MyNetDeviceFaceCallback));
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.Install(mobileNodes);

	ndn::StrategyChoiceHelper::Install(mobileNodes, "/", "/localhost/nfd/strategy/multicast");


	// 4. Set up applications
	NS_LOG_INFO("Installing Applications");

	ndn::AppHelper producerHelper("ns3::ndn::Producer");
//	producerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	producerHelper.SetPrefix("/beacon");
	producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper.Install(mobileNodes.Get(6));

	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper.SetPrefix("/beacon");
	consumerHelper.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper.Install(mobileNodes.Get(0));

 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/beacon");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper2.Install(mobileNodes.Get(1));

	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper3.SetPrefix("/beacon");
	consumerHelper3.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper3.Install(mobileNodes.Get(2));

	ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper4.SetPrefix("/beacon");
	consumerHelper4.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper4.Install(mobileNodes.Get(3));

	ndn::AppHelper consumerHelper5("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper5.SetPrefix("/beacon");
	consumerHelper5.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper5.Install(mobileNodes.Get(4));

	ndn::AppHelper consumerHelper6("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper6.SetPrefix("/beacon");
	consumerHelper6.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper6.Install(mobileNodes.Get(5));


	////////////////

	Simulator::Stop(Seconds(30.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
