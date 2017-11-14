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


    ns3::Ns2MobilityHelper ns2MobHelper = ns3::Ns2MobilityHelper("ns-movements-test2-n3.txt");
	std::cout<< ns2MobHelper.GetPositionFromTCLFileForNodeAtTime("Multicast-Strategy",node->GetId(),5) << "  is scheduled position for node "<< node->GetId() << "at time 5 " <<std::endl;

	std::cout << ns3::Simulator::Now() << std::endl;
  	for (fib::NextHopList::const_iterator it = nexthops.begin();
  			it != nexthops.end(); ++it) {

  		std::cout << " FIB is containing : NODE: " << node->GetId() << " name: "
  				<< interest.getName() << " face " << it->getFace()
				<< " mac : " << it->getMac() << std::endl
				<< " position-X : "<< it->getPositionX() << " position-Y : " << it->getPositionY() << " position-Z : "<< it->getPositionZ() <<std::endl
				<< " Future-position-X : " << it->getFuturePositionX() << " Future-position-Y : " << it->getFuturePositionY() << std::endl
				<< " Time-At-FuturePos : "<< it->getTimeAtFuturePosition()<< std::endl
				<< std::endl;
  	}

 // for (const auto& nexthop : nexthops) {
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

	int a=0;
    	//Since there are many MAC addresses in each node, we avoid looping between the same node
    	for (int index = 0; index < node->GetNDevices(); index++ ){
    		if (it->getMac()== currentMacAddresses[index]){
    			a=1;
    		}
    	}
    	if (a==0){
      this->sendInterest(pitEntry, outFace, interest, it->getMac());
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
