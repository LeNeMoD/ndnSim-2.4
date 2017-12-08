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

#include "ns3/spectrum-helper.h"
#include "ns3/antenna-model.h"
#include "ns3/parabolic-antenna-model.h"
#include "ns3/spectrum-wifi-phy.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/angles.h"

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


//	YansWifiChannelHelper wifiChannel;
//	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
//	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
//	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
//
//	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
//	wifiPhyHelper.SetChannel(wifiChannel.Create());
//	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
//	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));



//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
//	std::string traceFileString = "ns-movements-Slow-Fast-3n-10s.txt";
//	std::string traceFileString = "ns-movements-test2-n3.txt";
	std::string traceFileString = "ns-movements-upmiddledown-3n-40s.txt";



	//MacHelper
	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

	//Mobility Helper
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper(traceFileString);

	// Create Mobile nodes.
	NodeContainer mobileNodes;
//	mobileNodes.setTraceFileString(traceFileString);
	int nodeNr = 5;
	mobileNodes.Create(nodeNr);

	//Producer------------------- 4 Net dev 4 Antennas
	//NDev node 0 Parabolic 1
	NetDeviceContainer netDeviceContainerParabolicN0Dev1 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A1 = netDeviceContainerParabolicN0Dev1.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A1 = DynamicCast<SpectrumWifiPhy>(wpN0A1);

	//Set Parabolic Antenna1 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntenna0 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntenna0->SetBeamwidth(90);
	parabolicAntenna0->SetOrientation(0);
	swpN0A1->SetAntenna(parabolicAntenna0);

	//NDev node 0 Parabolic 2
	NetDeviceContainer netDeviceContainerParabolicN0Dev2 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A2 = netDeviceContainerParabolicN0Dev2.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A2 = DynamicCast<SpectrumWifiPhy>(wpN0A2);

	//Set Parabolic Antenna2 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A2 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN0A2->SetBeamwidth(90);
	parabolicAntennaN0A2->SetOrientation(90);
	swpN0A2->SetAntenna(parabolicAntennaN0A2);

	//NDev node 0 Parabolic 3
	NetDeviceContainer netDeviceContainerParabolicN0Dev3 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A3 = netDeviceContainerParabolicN0Dev3.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A3 = DynamicCast<SpectrumWifiPhy>(wpN0A3);

	//Set Parabolic Antenna3 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A3 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN0A3->SetBeamwidth(90);
	parabolicAntennaN0A3->SetOrientation(180);
	swpN0A3->SetAntenna(parabolicAntennaN0A3);

	//NDev node 0 Parabolic 4
	NetDeviceContainer netDeviceContainerParabolicN0Dev4 = wifi.Install(
			spectrumWifiPhyHelper, wifiMacHelper, mobileNodes.Get(0));

	//Set SpectrumPhy from WifiPhy
	Ptr<WifiPhy> wpN0A4 = netDeviceContainerParabolicN0Dev4.Get(0)->GetObject<
			WifiNetDevice>()->GetPhy();
	Ptr<SpectrumWifiPhy> swpN0A4 = DynamicCast<SpectrumWifiPhy>(wpN0A4);

	//Set Parabolic Antenna4 to node 0
	Ptr<ParabolicAntennaModel> parabolicAntennaN0A4 = CreateObject<
			ParabolicAntennaModel>();
	parabolicAntennaN0A4->SetBeamwidth(90);
	parabolicAntennaN0A4->SetOrientation(270);
	swpN0A4->SetAntenna(parabolicAntennaN0A4);

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

	// node: 2 --------------------- 4 net devices 4 antennas

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



	// configure movements for each node, while reading trace file
	ns2MobHelper.Install();

	////////////////
	// 1. Install Wifi
//	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, mobileNodes);
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
	consumerHelper.Install(mobileNodes.Get(4));

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
