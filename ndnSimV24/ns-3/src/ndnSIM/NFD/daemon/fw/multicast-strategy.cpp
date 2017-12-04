/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2017,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "multicast-strategy.hpp"
#include "algorithm.hpp"
#include "core/logger.hpp"
#include "ns3/node-list.h"
#include "ns3/node.h"

//Dome
#include "ns3/spectrum-wifi-phy.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/spectrum-analyzer.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-phy.h"
#include "ns3/ptr.h"
#include <math.h>
#include "../../../../antenna/model/angles.h"
#include "../../../../antenna/model/parabolic-antenna-model.h"
#include "../../../../antenna/model/parabolic-antenna-model.h"

namespace nfd {
namespace fw {

NFD_REGISTER_STRATEGY(MulticastStrategy);

NFD_LOG_INIT("MulticastStrategy");

const time::milliseconds MulticastStrategy::RETX_SUPPRESSION_INITIAL(10);
const time::milliseconds MulticastStrategy::RETX_SUPPRESSION_MAX(250);

MulticastStrategy::MulticastStrategy(Forwarder& forwarder, const Name& name)
  : Strategy(forwarder)
  , ProcessNackTraits(this)
  , m_retxSuppression(RETX_SUPPRESSION_INITIAL,
                      RetxSuppressionExponential::DEFAULT_MULTIPLIER,
                      RETX_SUPPRESSION_MAX)
{
  ParsedInstanceName parsed = parseInstanceName(name);
  if (!parsed.parameters.empty()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("MulticastStrategy does not accept parameters"));
  }
  if (parsed.version && *parsed.version != getStrategyName()[-1].toVersion()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument(
      "MulticastStrategy does not support version " + std::to_string(*parsed.version)));
  }
  this->setInstanceName(makeInstanceName(name, getStrategyName()));
}

const Name&
MulticastStrategy::getStrategyName()
{
  static Name strategyName("/localhost/nfd/strategy/multicast/%FD%03");
  return strategyName;
}

void
MulticastStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest,
                                        const shared_ptr<pit::Entry>& pitEntry)
{
  const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
  const fib::NextHopList& nexthops = fibEntry.getNextHops();
  ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
           std::ostringstream addr[node->GetNDevices()];
              std::string currentMacAddresses[node->GetNDevices()];

              for(int index = 0; index < node->GetNDevices(); index++) {
            	  addr[index] << node->GetDevice(index)->GetAddress();
            	  currentMacAddresses[index] = addr[index].str().substr(6);
              }
  int nEligibleNextHops = 0;

  bool isSuppressed = false;

  //Dome
  //print to check what the FIB contains
  shared_ptr<Interest> interest2= make_shared<Interest>(interest);

	std::cout<< "time in multicastStrategy: " << ns3::Simulator::Now() << std::endl;
	int flagFibContainsEmptyPosition=0;
  	for (fib::NextHopList::const_iterator it = nexthops.begin();
  			it != nexthops.end(); ++it) {

  		std::cout << " FIB of NODE: " << node->GetId() << "contains interest name: "
  				<< interest.getName() << " face " << it->getFace()
				<< " mac : " << it->getMac() << std::endl
				<< " position-X : "<< it->getPositionX() << " position-Y : " << it->getPositionY() << " position-Z : "<< it->getPositionZ() <<std::endl
				<< " Future-position-X : " << it->getFuturePositionX() << " Future-position-Y : " << it->getFuturePositionY() << std::endl
				<< " Time-At-FuturePos : "<< it->getTimeAtFuturePosition()<<" , is it a valid position n/y->0/1: "<<it->getFuturePositonWasSet()<< std::endl
				<< std::endl;

  		if(it->getPositionX()==3215){
  			flagFibContainsEmptyPosition=1;
  		}
  	}
  	if(flagFibContainsEmptyPosition==1){
  	  //Dome add NodeFuturePosition to the interest
  	//  Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
  	// 	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
  	//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-3n.txt");
  		ns3::Ns2MobilityHelper ns2MobHelper = ns3::Ns2MobilityHelper("ns-movements-RSU-To-Moving-2n.txt");

  	  ns3::Time time = (ns3::Simulator::Now());
  	  double at = std::ceil(time.GetSeconds())+1;
  	  std::cout<< "time from simulator to take futurePosition is  :" << at <<std::endl;

  	  ns3::Vector3D futurePositionVector = ns2MobHelper.GetPositionFromTCLFileForNodeAtTime("in Strategy requesting ",node->GetId(),at);
  	  double posX = futurePositionVector.x;
  	  double posY = futurePositionVector.y ;
  	  double posZ = futurePositionVector.z ;

  	  ndn::FuturePositionInfo futPos(interest2->getFuturePositionInfo());
  	  futPos.setFutureLocationX(posX);
  	  futPos.setFutureLocationY(posY);
  	  futPos.setFutureLocationZ(posZ);
  	  futPos.setFuturePositionWasSet(1);
  	  interest2->setFuturePositionInfo(futPos);
  	  std::cout<<"FuturePosition was set from the TLC-File to the interest in strategy: "<<futPos <<std::endl;
  	}

  	for (fib::NextHopList::const_iterator it = nexthops.begin(); it != nexthops.end(); ++it) {
  		Face& outFace = it->getFace();
  		RetxSuppressionResult suppressResult = m_retxSuppression.decidePerUpstream(*pitEntry, outFace);

    if (suppressResult == RetxSuppressionResult::SUPPRESS) {
      NFD_LOG_DEBUG(interest << " from=" << inFace.getId()
                    << "to=" << outFace.getId() << " suppressed");
      isSuppressed = true;
      continue;
    }

    if ((outFace.getId() == inFace.getId() && outFace.getLinkType() != ndn::nfd::LINK_TYPE_AD_HOC) ||
        wouldViolateScope(inFace, interest, outFace)) {
      continue;
    }

	int isItInternalMac=0;
    	//Since there are many MAC addresses in each node, we avoid looping between the same node
    	for (int index = 0; index < node->GetNDevices(); index++ ){
    		if (it->getMac()== currentMacAddresses[index]){
    			isItInternalMac=1;
    		}
    	}
    	if (isItInternalMac==0){

    		ndn::FuturePositionInfo futPos2;
    		futPos2.setFutureLocationX(it->getFuturePositionX());
    		futPos2.setFutureLocationY(it->getFuturePositionY());
    		futPos2.setFuturePositionWasSet(1);
    		interest2->setFuturePositionInfo(futPos2);
    	  	std::cout<<"FuturePosition was set from the FIB to the interest in strategy: "<<futPos2 <<std::endl;

    	  	if (it->getMac() != "eirini") {
				ns3::Ptr<ns3::NetDevice> netDev = node->GetDevice(0);
				ns3::Ptr<ns3::WifiPhy> spectWPhy = netDev->GetObject<ns3::WifiNetDevice>()->GetPhy();
				ns3::Ptr<ns3::SpectrumWifiPhy> swp0 = ns3::DynamicCast<ns3::SpectrumWifiPhy>(spectWPhy);
				ns3::Ptr<ns3::ParabolicAntennaModel> parab = ns3::DynamicCast<ns3::ParabolicAntennaModel>(swp0->GetRxAntenna());
				std::cout << " outgoing interest node: " << node->GetId()
						<< " target mac: " << it->getMac() << std::endl;
				std::cout
						<< "multycastStrat, on before send interest : will turning Antenna for outgoing interest to FIB position: "
						<< std::endl;
				ns3::Ptr<ns3::MobilityModel> model = node->GetObject<ns3::MobilityModel>();
				double deltaX = (model->GetPosition().x-(it->getFuturePositionX()));
				double deltaY = (model->GetPosition().y-(it->getFuturePositionY()));
				double angleRad = atan2(deltaY,deltaX);
				std::cout<<"angle rad multycast: "<<angleRad<<std::endl;
				double angle = ns3::RadiansToDegrees(((angleRad+3.14)));
//				double agnle = angle+180;
//				double angle = ns3::RadiansToDegrees((abs(angleRad)+3.14));
				parab->SetBeamwidth(20);
				parab->SetOrientation(angle);
				std::cout<<"at time : "<< ns3::Simulator::Now() << std::endl;
				std::cout << "parab turned to: " << angle
						<< " degrees and set to beam " << 20 << std::endl;
			}

      this->sendInterest(pitEntry, outFace, *interest2, it->getMac());
      break;
    	}



    NFD_LOG_DEBUG(interest << " from=" << inFace.getId()
                           << " pitEntry-to=" << outFace.getId());

    if (suppressResult == RetxSuppressionResult::FORWARD) {
      m_retxSuppression.incrementIntervalForOutRecord(*pitEntry->getOutRecord(outFace));
    }
    ++nEligibleNextHops;
  }

  /*if (nEligibleNextHops == 0 && !isSuppressed) {
    NFD_LOG_DEBUG(interest << " from=" << inFace.getId() << " noNextHop");

    lp::NackHeader nackHeader;
    nackHeader.setReason(lp::NackReason::NO_ROUTE);
    this->sendNack(pitEntry, inFace, nackHeader);

    this->rejectPendingInterest(pitEntry);
  }*/
}

void
MulticastStrategy::afterReceiveNack(const Face& inFace, const lp::Nack& nack,
                                    const shared_ptr<pit::Entry>& pitEntry)
{
  this->processNack(inFace, nack, pitEntry);
}

} // namespace fw
} // namespace nfd
