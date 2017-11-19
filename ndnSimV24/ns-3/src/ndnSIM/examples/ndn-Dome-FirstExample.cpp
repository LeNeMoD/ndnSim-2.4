/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/ndnSIM-module.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.FirsExampleDome");


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

	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
	//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");

	// Create Moble nodes.
	NodeContainer mobileNodes;
	mobileNodes.Create(3);

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

	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
//	consumerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	consumerHelper.SetPrefix("/beacon");
	consumerHelper.SetAttribute("Frequency", DoubleValue(10.0));
	consumerHelper.Install(mobileNodes.Get(0));

// 	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
//	consumerHelper2.SetPrefix("/test/prefix");
//	consumerHelper2.SetAttribute("Frequency", DoubleValue(10.0));
//	consumerHelper2.Install(MobileNodes.Get(2));

	ndn::AppHelper producerHelper("ns3::ndn::Producer");
//	producerHelper.SetTraceFile("ns-movements-test2-n3.txt");
	producerHelper.SetPrefix("/beacon");
	producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper.Install(mobileNodes.Get(1));

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
