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

#include "forwarder.hpp"
#include "algorithm.hpp"
#include "best-route-strategy2.hpp"
#include "core/logger.hpp"
#include "strategy.hpp"
#include "table/cleanup.hpp"
#include <ndn-cxx/lp/tags.hpp>
#include "face/null-face.hpp"
#include <boost/random/uniform_int_distribution.hpp>
#include "ns3/node.h"
#include "ns3/wifi-phy.h"
#include "ns3/node-list.h"
#include "ns3/wifi-phy-state-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "model/ndn-global-router.hpp"
#include "model/ndn-l3-protocol.hpp"
#include "model/ndn-net-device-transport.hpp"

#include "ns3/channel.h"
#include "ns3/net-device.h"

#include "ns3/ndnSIM/model/ndn-net-device-transport.hpp"

//Dome
//#include "../../../../mobility/model/constant-velocity-mobility-model.h"
#include "ns3/ns2-mobility-helper.h"
#include "../../../../antenna/model/parabolic-antenna-model.h"
#include "../../../../antenna/model/parabolic-antenna-model.h"
#include "ns3/ndnSIM/ndn-cxx/future-position-info.hpp"
//#include "ns3/parabolic-antenna-model.h"
#include "ns3/spectrum-wifi-phy.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/spectrum-analyzer.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-phy.h"
#include "ns3/ptr.h"
#include <math.h>
#include "../../../../antenna/model/angles.h"





namespace nfd {

NFD_LOG_INIT("Forwarder");

static Name
getDefaultStrategyName()
{
  return fw::BestRouteStrategy2::getStrategyName();
}

Forwarder::Forwarder()
  : m_unsolicitedDataPolicy(new fw::DefaultUnsolicitedDataPolicy())
  , m_fib(m_nameTree)
  , m_pit(m_nameTree)
  , m_measurements(m_nameTree)
  , m_strategyChoice(*this)
  , m_csFace(face::makeNullFace(FaceUri("contentstore://")))
{
  getFaceTable().addReserved(m_csFace, face::FACEID_CONTENT_STORE);

  m_faceTable.afterAdd.connect([this] (Face& face) {
    face.afterReceiveInterest.connect(
      [this, &face] (const Interest& interest) {
        this->startProcessInterest(face, interest);
      });
    face.afterReceiveData.connect(
      [this, &face] (const Data& data) {
        this->startProcessData(face, data);
      });
    face.afterReceiveNack.connect(
      [this, &face] (const lp::Nack& nack) {
        this->startProcessNack(face, nack);
      });
  });

  m_faceTable.beforeRemove.connect([this] (Face& face) {
    cleanupOnFaceRemoval(m_nameTree, m_fib, m_pit, face);
  });

  m_strategyChoice.setDefaultStrategy(getDefaultStrategyName());
}

Forwarder::~Forwarder() = default;

void
Forwarder::onIncomingInterest(Face& inFace, const Interest& interest)
{

	   // receive Interest
  NFD_LOG_DEBUG("onIncomingInterest face=" << inFace.getId() <<
                " interest=" << interest.getName());
  interest.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInInterests;

  // /localhost scope control
  bool isViolatingLocalhost = inFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(interest.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onIncomingInterest face=" << inFace.getId() <<
                  " interest=" << interest.getName() << " violates /localhost");
    // (drop)
    return;
  }

  // detect duplicate Nonce with Dead Nonce List
  bool hasDuplicateNonceInDnl = m_deadNonceList.has(interest.getName(), interest.getNonce());
  if (hasDuplicateNonceInDnl) {
    // goto Interest loop pipeline
    this->onInterestLoop(inFace, interest);
    return;
  }

  // strip forwarding hint if Interest has reached producer region
  if (!interest.getForwardingHint().empty() &&
      m_networkRegionTable.isInProducerRegion(interest.getForwardingHint())) {
    NFD_LOG_DEBUG("onIncomingInterest face=" << inFace.getId() <<
                  " interest=" << interest.getName() << " reaching-producer-region");
    const_cast<Interest&>(interest).setForwardingHint({});
  }

  // PIT insert
  shared_ptr<pit::Entry> pitEntry = m_pit.insert(interest).first;


  // detect duplicate Nonce in PIT entry
  int dnw = fw::findDuplicateNonce(*pitEntry, interest.getNonce(), inFace);
  bool hasDuplicateNonceInPit = dnw != fw::DUPLICATE_NONCE_NONE;
  if (inFace.getLinkType() == ndn::nfd::LINK_TYPE_POINT_TO_POINT) {
    // for p2p face: duplicate Nonce from same incoming face is not loop
    hasDuplicateNonceInPit = hasDuplicateNonceInPit && !(dnw & fw::DUPLICATE_NONCE_IN_SAME);
  }
  if (hasDuplicateNonceInPit) {
    // goto Interest loop pipeline
    this->onInterestLoop(inFace, interest);
    return;
  }

  // cancel unsatisfy & straggler timer
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

  //Dome outcommented because above print
  ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
    std::ostringstream addr[node->GetNDevices()];
      std::string currentMacAddresses[node->GetNDevices()];

      for(int index = 0; index < node->GetNDevices(); index++) {
    	  addr[index] << node->GetDevice(index)->GetAddress();
    	  currentMacAddresses[index] = addr[index].str().substr(6);
      }
      //Avoiding loops into the same node
      std::string a="eirini";
    auto transport = dynamic_cast<ns3::ndn::NetDeviceTransport*>(inFace.getTransport());
    if (transport!=NULL){
  	  a = transport->m_receivednetDevice;
  	  for (int i=0; i<node->GetNDevices(); i++) {
  		  if (a == currentMacAddresses[i]) {
  			  return;
  		  }
  	  }
  	  //Dome
    }
	 // ndn::FuturePositionInfo futPos =
  	  std::cout<< " on incoming interest node: " << node->GetId() << " comes from mac : " << a<< " interest name: "<< interest.getName()<<" position: "<<interest.getFuturePositionInfo().getFutureLocation_X()<<std::endl;

  const pit::InRecordCollection& inRecords = pitEntry->getInRecords();


  bool isPending = inRecords.begin() != inRecords.end();
  if (!isPending) {
    if (m_csFromNdnSim == nullptr) {
      m_cs.find(interest,
                bind(&Forwarder::onContentStoreHit, this, ref(inFace), pitEntry, _1, _2),
                bind(&Forwarder::onContentStoreMiss, this, ref(inFace), pitEntry, _1));
    }
    else {
          shared_ptr<Data> match = m_csFromNdnSim->Lookup(interest.shared_from_this());
          if (match != nullptr) {
            this->onContentStoreHit(inFace, pitEntry, interest, *match);
          }
          else {
        	  if (transport!=NULL){
        	    std::string a = transport->m_receivednetDevice;
            this->onContentStoreMiss2(inFace, pitEntry, interest, a,interest.getFuturePositionInfo());
        	  }
        	  else
        	  {this->onContentStoreMiss2(inFace, pitEntry, interest,"",interest.getFuturePositionInfo());}
          }
        }
      }
      else {
    	  if (transport!=NULL){
    	      	    std::string a = transport->m_receivednetDevice;
    	          this->onContentStoreMiss2(inFace, pitEntry, interest, a, interest.getFuturePositionInfo());
    	      	  }
    	      	  else
    	      	  {this->onContentStoreMiss2(inFace, pitEntry, interest,"",interest.getFuturePositionInfo());}
      }
    }

void
Forwarder::onInterestLoop(Face& inFace, const Interest& interest)
{
  // if multi-access or ad hoc face, drop
  if (inFace.getLinkType() != ndn::nfd::LINK_TYPE_POINT_TO_POINT) {
    NFD_LOG_DEBUG("onInterestLoop face=" << inFace.getId() <<
                  " interest=" << interest.getName() <<
                  " drop");
    return;
  }

  NFD_LOG_DEBUG("onInterestLoop face=" << inFace.getId() <<
                " interest=" << interest.getName() <<
                " send-Nack-duplicate");

  // send Nack with reason=DUPLICATE
  // note: Don't enter outgoing Nack pipeline because it needs an in-record.
  lp::Nack nack(interest);
  nack.setReason(lp::NackReason::DUPLICATE);
  inFace.sendNack(nack);
}

void
Forwarder::onContentStoreMiss(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
                              const Interest& interest)
{
  NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName());
  ++m_counters.nCsMisses;

  //Dome
//  std::cout<< "onContentStrorMiss interest= " << interest.getName()<<std::endl;
  // insert in-record
  pitEntry->insertOrUpdateInRecord(const_cast<Face&>(inFace), interest);

  // set PIT unsatisfy timer
  this->setUnsatisfyTimer(pitEntry);

  // has NextHopFaceId?
  shared_ptr<lp::NextHopFaceIdTag> nextHopTag = interest.getTag<lp::NextHopFaceIdTag>();
  if (nextHopTag != nullptr) {
    // chosen NextHop face exists?
    Face* nextHopFace = m_faceTable.get(*nextHopTag);
    if (nextHopFace != nullptr) {
      NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName() << " nexthop-faceid=" << nextHopFace->getId());
      // go to outgoing Interest pipeline
      // scope control is unnecessary, because privileged app explicitly wants to forward
      this->onOutgoingInterest(pitEntry, *nextHopFace, interest);
    }
    return;
  }

  // dispatch to strategy: after incoming Interest
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.afterReceiveInterest(inFace, interest, pitEntry); });
}
//void
//Forwarder::onContentStoreMiss2(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
//                              const Interest& interest, std::string mac)
//{
//  NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName());
//
//  //Dome
////  std::cout<< "onContentStrorMiss2 interest= " << interest.getName()<<std::endl;
//
//  // insert in-record
//  pitEntry->insertOrUpdateInRecord(const_cast<Face&>(inFace), mac, interest.getFuturePositionInfo(), interest);
////  std::cout<<"insertOrUpdateInRecord interest contStoreMiss futurePosX forwarder: "<<interest.getFuturePositionInfo().m_location_X_Coord<<std::endl;
////  std::cout<<"insertOrUpdateInRecord interest contStoreMiss futurePosY forwarder: "<<interest.getFuturePositionInfo().m_location_Y_Coord<<std::endl;
//
//
//
////  std::cout<<"pitEntry->getInRecords().size() futurePosX forwarder: "<<pitEntry->getInRecords().size()<<std::endl;
////   for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
//// 	  std::cout<<"iRecord contStoreMiss futurePosX forwarder: "<<inRecord.getFuturePositionInfo().m_location_X_Coord<<std::endl;
//// 	  std::cout<<"iRecord contStoreMiss futurePosY forwarder: "<<inRecord.getFuturePositionInfo().m_location_Y_Coord<<std::endl;
//// 	  std::cout<<"iRecord contStoreMiss asdfasdfasdf futurePosX forwarder: "<<inRecord.getFuturePositionY()<<std::endl;
//// 	  std::cout<<"iRecord contStoreMiss asdfasdfasdf futurePosY forwarder: "<<inRecord.getFuturePositionY()<<std::endl;
////   }
//
//  // set PIT unsatisfy timer
//  this->setUnsatisfyTimer(pitEntry);
//
//  // has NextHopFaceId?
//  shared_ptr<lp::NextHopFaceIdTag> nextHopTag = interest.getTag<lp::NextHopFaceIdTag>();
//  if (nextHopTag != nullptr) {
//    // chosen NextHop face exists?
//    Face* nextHopFace = m_faceTable.get(*nextHopTag);
//    if (nextHopFace != nullptr) {
//      NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName() << " nexthop-faceid=" << nextHopFace->getId());
//      // go to outgoing Interest pipeline
//      // scope control is unnecessary, because privileged app explicitly wants to forward
//      this->onOutgoingInterest(pitEntry, *nextHopFace, interest);
//    }
//    return;
//  }
//
//  // dispatch to strategy: after incoming Interest
//  this->dispatchToStrategy(*pitEntry,
//    [&] (fw::Strategy& strategy) { strategy.afterReceiveInterest(inFace, interest, pitEntry); });
//}
//Dome
void
Forwarder::onContentStoreMiss2(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
                              const Interest& interest, std::string mac, ndn::FuturePositionInfo futurePositionInfo)
{
  NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName());

  //Dome
//  std::cout<< "onContentStrorMiss2 mine interest= " << interest.getName()<<std::endl;
  // insert in-record
  pitEntry->insertOrUpdateInRecord(const_cast<Face&>(inFace), mac,futurePositionInfo, interest);

  std::cout<<"insertOrUpdateInRecord interest contStoreMiss--2 futurePosX forwarder: "<<futurePositionInfo.m_location_X_Coord<<" mac: "<< mac<<std::endl;
//   std::cout<<"insertOrUpdateInRecord interest contStoreMiss--2 futurePosY forwarder: "<<futurePositionInfo.m_location_Y_Coord<<std::endl;

  // set PIT unsatisfy timer
  this->setUnsatisfyTimer(pitEntry);

  // has NextHopFaceId?
  shared_ptr<lp::NextHopFaceIdTag> nextHopTag = interest.getTag<lp::NextHopFaceIdTag>();
  if (nextHopTag != nullptr) {
    // chosen NextHop face exists?
    Face* nextHopFace = m_faceTable.get(*nextHopTag);
    if (nextHopFace != nullptr) {
      NFD_LOG_DEBUG("onContentStoreMiss interest=" << interest.getName() << " nexthop-faceid=" << nextHopFace->getId());
      // go to outgoing Interest pipeline
      // scope control is unnecessary, because privileged app explicitly wants to forward
      this->onOutgoingInterest(pitEntry, *nextHopFace, interest);
    }
    return;
  }

  // dispatch to strategy: after incoming Interest
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.afterReceiveInterest(inFace, interest, pitEntry); });
}
void
Forwarder::onContentStoreHit(const Face& inFace, const shared_ptr<pit::Entry>& pitEntry,
                             const Interest& interest, const Data& data)
{
  NFD_LOG_DEBUG("onContentStoreHit interest=" << interest.getName());
  ++m_counters.nCsHits;

  //Dome
//  std::cout<< "onContentStrorHit interest= " << interest.getName()<<std::endl;

  beforeSatisfyInterest(*pitEntry, *m_csFace, data);
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.beforeSatisfyInterest(pitEntry, *m_csFace, data); });

  data.setTag(make_shared<lp::IncomingFaceIdTag>(face::FACEID_CONTENT_STORE));
  // XXX should we lookup PIT for other Interests that also match csMatch?

  //Dome
//  std::cout<<"pitEntry->getInRecords().size() on contentStoreHit futurePosX forwarder: "<<pitEntry->getInRecords().size()<<std::endl;
//    for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
//  	  std::cout<<"iRecord contStore-HIT futurePosX forwarder: "<<inRecord.getFuturePositionInfo().m_location_X_Coord<<std::endl;
//  	  std::cout<<"iRecord contStore-HIT futurePosY forwarder: "<<inRecord.getFuturePositionInfo().m_location_Y_Coord<<std::endl;
//  	 std::cout<<"iRecord contStore-HIT asdfasdfasdf futurePosX forwarder: "<<inRecord.getFuturePositionY()<<std::endl;
//  	 std::cout<<"iRecord contStore-HIT asdfasdfasdf futurePosY forwarder: "<<inRecord.getFuturePositionY()<<std::endl;
//    }

  // set PIT straggler timer
  this->setStragglerTimer(pitEntry, true, data.getFreshnessPeriod());
  ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
   auto transport = dynamic_cast<ns3::ndn::NetDeviceTransport*>(inFace.getTransport());
   std::string currentMacAddresses[node->GetNDevices()];
   std::ostringstream addr[node->GetNDevices()];
   for(int index = 0; index < node->GetNDevices(); index++) {
    	addr[index] << node->GetDevice(index)->GetAddress();
    	currentMacAddresses[index] = addr[index].str().substr(6);
    }
      if (transport!=NULL){
      	//LOOP DETECTION FOR 1 NODE
      	std::string a = transport->m_receivednetDevice;
      	for (int i=0; i< node->GetNDevices(); i++){
      		if (a== currentMacAddresses[i]){
      			return;
      		}
      	} targetmac=a;
      }
  // goto outgoing Data pipeline
  this->onOutgoingData(data, *const_pointer_cast<Face>(inFace.shared_from_this()));
}

void
Forwarder::onOutgoingInterest(const shared_ptr<pit::Entry>& pitEntry, Face& outFace, const Interest& interest, std::string mac)
{
	NFD_LOG_DEBUG(
			"onOutgoingInterest face=" << outFace.getId() << " interest=" << pitEntry->getName());

	// insert out-record
	pitEntry->insertOrUpdateOutRecord(outFace, interest);
	 std::cout<<"insertOrUpdateInRecord interest OutgoingInterest futurePosX forwarder: "<<interest.getFuturePositionInfo().getFutureLocation_X()<<" mac " << mac<< std::endl;
	// std::cout<<"insertOrUpdateInRecord interest OutgoingInterest futurePosY forwarder: "<<interest.getFuturePositionInfo().m_location_Y_Coord<<std::endl;

	targetmac = mac;

	// send Interest
	outFace.sendInterest(interest);
	++m_counters.nOutInterests;

}


void
Forwarder::onOutgoingInterest(const shared_ptr<pit::Entry>& pitEntry, Face& outFace, const Interest& interest)
{
  NFD_LOG_DEBUG("onOutgoingInterest face=" << outFace.getId() <<
                " interest=" << pitEntry->getName());
  // insert out-record
  pitEntry->insertOrUpdateOutRecord(outFace, interest);
   // send Interest
  outFace.sendInterest(interest);
  ++m_counters.nOutInterests;
}
void
Forwarder::onInterestReject(const shared_ptr<pit::Entry>& pitEntry)
{
  if (fw::hasPendingOutRecords(*pitEntry)) {
    NFD_LOG_ERROR("onInterestReject interest=" << pitEntry->getName() <<
                  " cannot reject forwarded Interest");
    return;
  }
  NFD_LOG_DEBUG("onInterestReject interest=" << pitEntry->getName());

  // cancel unsatisfy & straggler timer
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

  // set PIT straggler timer
  this->setStragglerTimer(pitEntry, false);
}

void
Forwarder::onInterestUnsatisfied(const shared_ptr<pit::Entry>& pitEntry)
{
  NFD_LOG_DEBUG("onInterestUnsatisfied interest=" << pitEntry->getName());

  // invoke PIT unsatisfied callback
  beforeExpirePendingInterest(*pitEntry);
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.beforeExpirePendingInterest(pitEntry); });

  // goto Interest Finalize pipeline
  this->onInterestFinalize(pitEntry, false);
}

void
Forwarder::onInterestFinalize(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
                              ndn::optional<time::milliseconds> dataFreshnessPeriod)
{
  NFD_LOG_DEBUG("onInterestFinalize interest=" << pitEntry->getName() <<
                (isSatisfied ? " satisfied" : " unsatisfied"));

  // Dead Nonce List insert if necessary
  this->insertDeadNonceList(*pitEntry, isSatisfied, dataFreshnessPeriod, 0);

  // PIT delete
  this->cancelUnsatisfyAndStragglerTimer(*pitEntry);
  m_pit.erase(pitEntry.get());
}

void
Forwarder::onIncomingData(Face& inFace, const Data& data)
{
  // receive Data
  NFD_LOG_DEBUG("onIncomingData face=" << inFace.getId() << " data=" << data.getName());
  data.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInData;


  // /localhost scope control
  bool isViolatingLocalhost = inFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(data.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onIncomingData face=" << inFace.getId() <<
                  " data=" << data.getName() << " violates /localhost");
    // (drop)
    return;
  }

  // PIT match
  pit::DataMatchResult pitMatches = m_pit.findAllDataMatches(data);
  if (pitMatches.begin() == pitMatches.end()) {
    // goto Data unsolicited pipeline
    this->onDataUnsolicited(inFace, data);
    return;
  }

  shared_ptr<Data> dataCopyWithoutTag = make_shared<Data>(data);
  dataCopyWithoutTag->removeTag<lp::HopCountTag>();

  // CS insert
  if (m_csFromNdnSim == nullptr)
    m_cs.insert(*dataCopyWithoutTag);
  else
    m_csFromNdnSim->Add(dataCopyWithoutTag);

  std::vector<Face*> pendingDownstreams;
  std::vector<std::string> macs;
  std::vector<ndn::FuturePositionInfo> futPosss;
  // foreach PitEntry
  auto now = time::steady_clock::now();
  for (const shared_ptr<pit::Entry>& pitEntry : pitMatches) {
    NFD_LOG_DEBUG("onIncomingData matching=" << pitEntry->getName());

    //Dome
    ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
//    std::cout<<"forwarder node : "<< node->GetId()<< " , on incomming Data : "<< data.getName()<<std::endl;
//
//    //    for(int i = 0 ; i<node->GetNDevices() ;i++){
//    //    	ns3::Ptr<ns3::NetDevice> netDev = node->GetDevice(i);
//    //    	ns3::Ptr<ns3::WifiPhy> spectWPhy = netDev->GetObject<ns3::WifiNetDevice>()->GetPhy();
//    //    	ns3::Ptr<ns3::SpectrumWifiPhy> swp0 = ns3::DynamicCast<ns3::SpectrumWifiPhy> (spectWPhy);
//    //    	ns3::Ptr<ns3::ParabolicAntennaModel> parab = ns3::DynamicCast<ns3::ParabolicAntennaModel> (swp0->GetRxAntenna());
//    //    	std::cout<<"parabolicAntenna NetDev"<< i << " of MAC: "<<netDev->GetAddress()<<" and node: " <<node->GetId() <<" orientation is is: "<< parab->GetOrientation() <<std::endl;
//    //    }
//
//    //  std::cout << " on Incoming Data node : " << node->GetId() << "at time : " << ns3::Simulator::Now() << std::endl;

//    std::cout<<"node: "<< node->GetId()<<" ,forwarder incoming data match in pit"<<std::endl;

    // cancel unsatisfy & straggler timer
    this->cancelUnsatisfyAndStragglerTimer(*pitEntry);

    // remember pending downstreams
    for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
      if (inRecord.getExpiry() > now) {
        pendingDownstreams.push_back(&inRecord.getFace());
        macs.push_back(inRecord.getMac());
        futPosss.push_back(inRecord.getFuturePositionInfo());


      }
    }

    //Redeclaration above of node ...
//    ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
    std::ostringstream addr[node->GetNDevices()];
       std::string currentMacAddresses[node->GetNDevices()];

       for(int index = 0; index < node->GetNDevices(); index++) {
       	addr[index] << node->GetDevice(index)->GetAddress();
       	currentMacAddresses[index] = addr[index].str().substr(6);
       }
       auto transport = dynamic_cast<ns3::ndn::NetDeviceTransport*>(inFace.getTransport());
       if (transport!=NULL){
       	//LOOP DETECTION FOR 1 NODE
       	std::string a = transport->m_receivednetDevice;
       	for (int i=0; i< node->GetNDevices(); i++){
       		if (a== currentMacAddresses[i]){
       			return;
       		}
       	}

    	//update position to Fib
       	//FIB POPULATION WHEN A DATA MSG IS COMING BACK
       	// Dome outcomment
//       	ns3::ndn::FibHelper::AddRoute(node, "/beacon", inFace.getId(), 111, a );

       	//Dome

    	ns3::Ptr<ns3::MobilityModel> mobilityModel = node->GetObject<ns3::MobilityModel>();
    	ns3::Vector pos = mobilityModel->GetPosition();

       	ndn::FuturePositionInfo futurePositionInfo = data.getFuturePositionInfo();

       	std::cout<<"addRoute in forwarder on incomming data node: "<<node->GetId()<<" name "<< data.getName()<<" mac :"<< a<< " futurePosition info "<< futurePositionInfo.getFuturePositionVector()<< std::endl;
       	ns3::ndn::FibHelper::AddRoute(node, "/beacon", inFace.getId(), 111, a,pos.x,pos.y,pos.z,futurePositionInfo.getFutureLocation_X(),futurePositionInfo.getFutureLocation_Y(),futurePositionInfo.getTimeAtFutureLocation(), futurePositionInfo.isfuturePositionSet());

     }
    // invoke PIT satisfy callback
    beforeSatisfyInterest(*pitEntry, inFace, data);
    this->dispatchToStrategy(*pitEntry,[&] (fw::Strategy& strategy) { strategy.beforeSatisfyInterest(pitEntry, inFace, data); });

    // Dead Nonce List insert if necessary (for out-record of inFace)
    this->insertDeadNonceList(*pitEntry, true, data.getFreshnessPeriod(), &inFace);





    //Dome Antenna Part
//      ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
//      std::cout<<"outgoing Data: "<< data.getName()<<" on node: " <<node->GetId()<<std::endl;
//      ndn::FuturePositionInfo futurePositionInfoObject = data.getFuturePositionInfo();
//      double futureX = futurePositionInfoObject.getFutureLocation_X();
//      double futureY = futurePositionInfoObject.getFutureLocation_Y();
//      double futureZ = futurePositionInfoObject.getFutureLocation_Z();
//      std::cout<<"forwarder before antenna part simulator time is : " << ns3::Simulator::Now() <<std::endl;
//      ns3::Ptr<ns3::MobilityModel> model = node->GetObject<ns3::MobilityModel>();
//      std::cout<<"future Position unpacked from data in forwarder on outgoing data"<<std::endl;
//      std::cout<<"node: "<<node->GetId()<<" has pos: " << model->GetPosition()<< " position in data is " << futurePositionInfoObject.getFuturePositionVector()<<std::endl;

//    pit::DataMatchResult pitDataMatchresults = m_pit.findAllDataMatches(data);
//		if (pitDataMatchresults.begin() != pitDataMatchresults.end()) {
//    for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
//			for (const shared_ptr<pit::Entry>& pEntry : pitDataMatchresults) { //		  for (int it = 0; it < pitDataMatchresults.size() ; it++) {
				//			pit::Entry pEntry = pitDataMatchresults;


//    ns3::Ptr<ns3::NetDevice> netDev = node->GetDevice(0);
//				ns3::Ptr<ns3::WifiPhy> spectWPhy = netDev->GetObject<ns3::WifiNetDevice>()->GetPhy();
//				ns3::Ptr<ns3::SpectrumWifiPhy> swp0 = ns3::DynamicCast<ns3::SpectrumWifiPhy>(spectWPhy);
//				ns3::Ptr<ns3::ParabolicAntennaModel> parab = ns3::DynamicCast<ns3::ParabolicAntennaModel>(swp0->GetRxAntenna());
//				std::cout<< "forwarder, on before send date : will turning Antenna for outgoing data to PitInRecord position: "<< std::endl;
////				double inRecordSize = pEntry->getInRecords().size();
////				std::cout << "inRecord size in forwarder: " << inRecordSize<< std::endl;
//				for (const pit::InRecord& inRecord : pitEntry->getInRecords()) {
//					ns3::Ptr<ns3::MobilityModel> model = node->GetObject<ns3::MobilityModel>();
//					std::cout<<"Forwarder Position from model: "<<model->GetPosition()<<std::endl;
//					std::cout<<"Forwarder the Position from futurePositionInfo in PIT: "<<inRecord.getFuturePositionInfo().getFuturePositionVector()<<std::endl;
////					std::cout<<"Forwarder the Position from futurePositionInfo in PIT: "<<inRecord.getFuturePositionX<<std::endl;
////					std::cout<<"Forwarder the Position from futurePositionInfo in PIT: "<<inRecord.getFuturePositionY<<std::endl;
//					double deltaX =(model->GetPosition().x- (inRecord.getFuturePositionX()));
//					double deltaY =(model->GetPosition().y- (inRecord.getFuturePositionY()));
//					double angleRad = atan2(deltaY, deltaX);
////					std::cout << "angle rad forwarder: " << angleRad<< std::endl;
////					double angle = ns3::RadiansToDegrees(((angleRad + 3.14)));
////					//				    	double agnle = angle+180;
////					//						double angle = ns3::RadiansToDegrees((abs(angleRad)+3.14));
////					parab->SetBeamwidth(20);
////					parab->SetOrientation(angle);
////					std::cout << "at time : " << ns3::Simulator::Now()<< std::endl;
////					std::cout << "inf Forwarder onDataIncoming parab turned to: " << angle
////							<< " degrees and set to beam " << 20 << std::endl;
//				}
//			}





    // mark PIT satisfied
    pitEntry->clearInRecords();
    pitEntry->deleteOutRecord(inFace);

    // set PIT straggler timer
    this->setStragglerTimer(pitEntry, true, data.getFreshnessPeriod());
  }

  // foreach pending downstream
  for (int it = 0; it < pendingDownstreams.size() ; ++it) {
   	  Face* pendingDownstream = pendingDownstreams[it] ;
    //for (Face* pendingDownstream : pendingDownstreams) {
   	  //ENTER TARGET MAC FOR SENDING IN A SPECIFIC NODE
  	  targetmac=macs[it];
  	  ndn::FuturePositionInfo targetFuturePosition = futPosss[it];
      if (pendingDownstream == &inFace) {
        continue;
      }

//      if(targetmac == ""){
      ns3::Ptr<ns3::Node> node = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
      ns3::Ptr<ns3::NetDevice> netDev = node->GetDevice(0);
      ns3::Ptr<ns3::WifiPhy> spectWPhy = netDev->GetObject<ns3::WifiNetDevice>()->GetPhy();
      ns3::Ptr<ns3::SpectrumWifiPhy> swp0 = ns3::DynamicCast<ns3::SpectrumWifiPhy>(spectWPhy);
      ns3::Ptr<ns3::ParabolicAntennaModel> parab = ns3::DynamicCast<ns3::ParabolicAntennaModel>(swp0->GetRxAntenna());
//      std::cout << " outgoing data node: " << node->GetId() << " target mac: " << targetmac << std::endl;
//      std::cout<< "forwarder, on before send data : will turning Antenna for outgoing data to PitInRecord position: "<< std::endl;
      ns3::Ptr<ns3::MobilityModel> model = node->GetObject<ns3::MobilityModel>();
//      std::cout<<"Forwarder Position from model outgoing data: "<<model->GetPosition()<<std::endl;
//      std::cout<<"Forwarder the Position from futurePositionInfo in PIT outgoing data: "<<targetFuturePosition.getFuturePositionVector()<<std::endl;
      int deltaX =(model->GetPosition().x- (targetFuturePosition.getFutureLocation_X()));
      int deltaY =(model->GetPosition().y- (targetFuturePosition.getFutureLocation_Y()));
      double angleRad = atan2(deltaY, deltaX);
//      std::cout << "angle rad forwarder: " << angleRad<< std::endl;
      double angle = ns3::RadiansToDegrees(((angleRad + 3.14)));
//      parab->SetBeamwidth(360);
      parab->SetOrientation(angle);
 //     std::cout << "at time : " << ns3::Simulator::Now()<< std::endl;
      std::cout << "inf Forwarder onDataIncoming parab turned to: " << angle << std::endl;


      // goto outgoing Data pipeline
      ns3::Ptr<ns3::Node> node2 = ns3::NodeList::GetNode(ns3::Simulator::GetContext());

   //   std::cout<<"node " << node2->GetId()<< " onOutgoingData to mac: "<< targetmac<<std::endl;
    //  std::cout<<"targetFuturePos is " << targetFuturePosition<<std::endl;
      this->onOutgoingData(data, *pendingDownstream);
    }
  }


void
Forwarder::onDataUnsolicited(Face& inFace, const Data& data)
{
  // accept to cache?
  fw::UnsolicitedDataDecision decision = m_unsolicitedDataPolicy->decide(inFace, data);
  if (decision == fw::UnsolicitedDataDecision::CACHE) {
    // CS insert
    if (m_csFromNdnSim == nullptr)
      m_cs.insert(data, true);
    else
      m_csFromNdnSim->Add(data.shared_from_this());
  }

  NFD_LOG_DEBUG("onDataUnsolicited face=" << inFace.getId() <<
                " data=" << data.getName() <<
                " decision=" << decision);
}

void
Forwarder::onOutgoingData(const Data& data, Face& outFace)
{
  if (outFace.getId() == face::INVALID_FACEID) {
    NFD_LOG_WARN("onOutgoingData face=invalid data=" << data.getName());
    return;
  }
  NFD_LOG_DEBUG("onOutgoingData face=" << outFace.getId() << " data=" << data.getName());




  //Dome  FuturePosition to the data
  ns3::Ptr<ns3::Node> node2 = ns3::NodeList::GetNode(ns3::Simulator::GetContext());
  //std::cout<<"node: " << node2->GetId() << " Has incomming interest " <<std::endl;
  //std::cout<<"node: " << node2->GetId() << " responding with Data: "<< data.getName() <<std::endl;

//  std::cout<<"node has number of applications running : "<<   node->GetNApplications()<<std::endl;

//  Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
// 	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-3n.txt");
	ns3::Ns2MobilityHelper ns2MobHelper = ns3::Ns2MobilityHelper("ns-movements-RSU-To-Moving-2n.txt");

  ns3::Time time = (ns3::Simulator::Now());
  int at = std::ceil(time.GetSeconds());
 // std::cout<< "time from simulator to take futurePosition is in Forwarder outGoingData :" << at <<std::endl;

 // std::cout<<"forwarder node on outData would request data"<<node2->GetId()<<std::endl;
  ns3::Vector3D futurePositionVector = ns2MobHelper.GetPositionFromTCLFileForNodeAtTime("forwarder node  on outData",node2->GetId(),at);
  int posX = futurePositionVector.x;
  int posY = futurePositionVector.y ;
  int posZ = futurePositionVector.z ;

  ndn::FuturePositionInfo futPos;

  futPos.setFutureLocationX(posX);
  futPos.setFutureLocationY(posY);
  futPos.setFutureLocationZ(posZ);
  futPos.setTimeAtFutureLocation(at);
  int wasSet = 1;
  futPos.setFuturePositionWasSet(wasSet);

  //std::cout<<"futpos set x,y,z :"<<posX<<" , "<<posY<<" , "<<posZ<<" , "<<" at time: "<< at<<std::endl;

  shared_ptr<Data> data2= make_shared<Data>(data);

  data2->setFuturePositionInfo(futPos);
 // std::cout<<"FuturePosition was set from the TLC-File to the DATA in forwarder"<<std::endl;

//  std::cout<<"data futurPos in forwarder contains: "<< data2->getFuturePositionInfo().m_location_X_Coord <<std::endl;
//  std::cout<<"data futurPos in forwarder contains: "<< data2->getFuturePositionInfo().m_location_Y_Coord <<std::endl;

  // /localhost scope control
  bool isViolatingLocalhost = outFace.getScope() == ndn::nfd::FACE_SCOPE_NON_LOCAL &&
                              scope_prefix::LOCALHOST.isPrefixOf(data.getName());
  if (isViolatingLocalhost) {
    NFD_LOG_DEBUG("onOutgoingData face=" << outFace.getId() <<
                  " data=" << data.getName() << " violates /localhost");
    // (drop)
    return;
  }

  // TODO traffic manager


//  std::cout<<"node: "<<node->GetId()<<" has # of net devices: "<<node->GetNDevices()<<std::endl;
//  std::ostringstream addr[node->GetNDevices()];
//  std::string currentMacAddresses[node->GetNDevices()];
//
//  //check welche device den input erhält und drehe diese ...
////	for (int index = 0; index < node->GetNDevices(); index++) {
////		addr[index] << node->GetDevice(index)->GetAddress();
////		currentMacAddresses[index] = addr[index].str().substr(6);
////	}
////	auto transport = dynamic_cast<ns3::ndn::NetDeviceTransport*>(outFace.getTransport());
////	std::cout<< "forwarder, on outgoing data : netDevTransport is: "<< transport->GetNetDevice()->GetAddress()<< std::endl;
//
////  cout <<"device 0 pf node "<< node->GetId()<< " is: " <<typeid(node->GetDevice(0)).name() << endl;
////  cout <<"device 1 pf node "<< node->GetId()<< " is: " <<typeid(node->GetDevice(1)).name() << endl;
////
//////  for(int i = 1 ;  i == node->GetNDevices(); i++){
//////	  if(node->GetDevice(i))
//////  }
//
//  ns3::Ptr<ns3::NetDevice> netDev = node->GetDevice(0);
////  std::cout<< "forwarder, net Dev number = " <<node->GetDevice(0) <<" is used"<< std::endl;
//
//  ns3::Ptr<ns3::WifiPhy> spectWPhy = netDev->GetObject<ns3::WifiNetDevice>()->GetPhy();
//  ns3::Ptr<ns3::SpectrumWifiPhy> swp0 = ns3::DynamicCast<ns3::SpectrumWifiPhy> (spectWPhy);
////  std::cout<<"parabolicAntenna is: "<< swp0->GetRxAntenna() <<std::endl;
//  ns3::Ptr<ns3::ParabolicAntennaModel> parab = ns3::DynamicCast<ns3::ParabolicAntennaModel> (swp0->GetRxAntenna());
//  ns3::Ptr<ns3::MobilityModel> model = node->GetObject<ns3::MobilityModel>();
//  std::cout<<" outgoing data node: " << node->GetId() << " target mac: " << targetmac << " name of data: " << data.getName()
//		  <<" outgoing node position : "<< model->GetPosition() << "FuturePosition Contains: x, y, z :"<< futureX<<" ,"<<futureY
//		  <<" position is valid: "<< futurePositionInfoObject.isfuturePositionSet()<< std::endl;
//
//  bool parabHasToTurn = false;
//  if(futurePositionInfoObject.isfuturePositionSet()==1 && targetmac== ""){
//		std::cout<< "forwarder, on outgoing data : will turning Antenna to futureNodePosition: "<< std::endl;
//		double degrees = calculateAngleBtw2Nodes(futureY, model->GetPosition().y, futureX, model->GetPosition().x);
//		parabHasToTurn = true;
//		parab->SetBeamwidth(20);
//		parab->SetOrientation(degrees);
//		std::cout<<"parab turned to: "<<degrees<<" degrees and set to beam "<< 20<<std::endl;
//  }
  //Dome
//  std::cout<< " outgoing data node: " << node->GetId() << " target mac: " << targetmac << " name of data: " << data.getName() << std::endl;

//  }
    std::cout<< " outgoing data node: " << node2->GetId() << " target mac: " << targetmac << " name of data: " << data.getName() << " futurePositoin of data: "<< data2->getFuturePositionInfo().getFutureLocation_X()<<std::endl;
  // send Data

	outFace.sendData(*data2);
	++m_counters.nOutData;
}

void
Forwarder::onIncomingNack(Face& inFace, const lp::Nack& nack)
{
  // receive Nack
  nack.setTag(make_shared<lp::IncomingFaceIdTag>(inFace.getId()));
  ++m_counters.nInNacks;

  // if multi-access or ad hoc face, drop
  if (inFace.getLinkType() != ndn::nfd::LINK_TYPE_POINT_TO_POINT) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " face-is-multi-access");
    return;
  }

  // PIT match
  shared_ptr<pit::Entry> pitEntry = m_pit.find(nack.getInterest());
  // if no PIT entry found, drop
  if (pitEntry == nullptr) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " no-PIT-entry");
    return;
  }

  // has out-record?
  pit::OutRecordCollection::iterator outRecord = pitEntry->getOutRecord(inFace);
  // if no out-record found, drop
  if (outRecord == pitEntry->out_end()) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " no-out-record");
    return;
  }

  // if out-record has different Nonce, drop
  if (nack.getInterest().getNonce() != outRecord->getLastNonce()) {
    NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                  " nack=" << nack.getInterest().getName() <<
                  "~" << nack.getReason() << " wrong-Nonce " <<
                  nack.getInterest().getNonce() << "!=" << outRecord->getLastNonce());
    return;
  }

  NFD_LOG_DEBUG("onIncomingNack face=" << inFace.getId() <<
                " nack=" << nack.getInterest().getName() <<
                "~" << nack.getReason() << " OK");

  // record Nack on out-record
  outRecord->setIncomingNack(nack);

  // trigger strategy: after receive NACK
  this->dispatchToStrategy(*pitEntry,
    [&] (fw::Strategy& strategy) { strategy.afterReceiveNack(inFace, nack, pitEntry); });
}

void
Forwarder::onOutgoingNack(const shared_ptr<pit::Entry>& pitEntry, const Face& outFace,
                          const lp::NackHeader& nack)
{
  if (outFace.getId() == face::INVALID_FACEID) {
    NFD_LOG_WARN("onOutgoingNack face=invalid" <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " no-in-record");
    return;
  }

  // has in-record?
  pit::InRecordCollection::iterator inRecord = pitEntry->getInRecord(outFace);

  // if no in-record found, drop
  if (inRecord == pitEntry->in_end()) {
    NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " no-in-record");
    return;
  }

  // if multi-access or ad hoc face, drop
  if (outFace.getLinkType() != ndn::nfd::LINK_TYPE_POINT_TO_POINT) {
    NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                  " nack=" << pitEntry->getInterest().getName() <<
                  "~" << nack.getReason() << " face-is-multi-access");
    return;
  }

  NFD_LOG_DEBUG("onOutgoingNack face=" << outFace.getId() <<
                " nack=" << pitEntry->getInterest().getName() <<
                "~" << nack.getReason() << " OK");

  // create Nack packet with the Interest from in-record
  lp::Nack nackPkt(inRecord->getInterest());
  nackPkt.setHeader(nack);

  // erase in-record

  pitEntry->deleteInRecord(outFace);

  // send Nack on face
  const_cast<Face&>(outFace).sendNack(nackPkt);
  ++m_counters.nOutNacks;
}

static inline bool
compare_InRecord_expiry(const pit::InRecord& a, const pit::InRecord& b)
{
  return a.getExpiry() < b.getExpiry();
}

void
Forwarder::setUnsatisfyTimer(const shared_ptr<pit::Entry>& pitEntry)
{
  pit::InRecordCollection::iterator lastExpiring =
    std::max_element(pitEntry->in_begin(), pitEntry->in_end(), &compare_InRecord_expiry);

  time::steady_clock::TimePoint lastExpiry = lastExpiring->getExpiry();
  time::nanoseconds lastExpiryFromNow = lastExpiry - time::steady_clock::now();
  if (lastExpiryFromNow <= time::seconds::zero()) {
    // TODO all in-records are already expired; will this happen?
  }

  scheduler::cancel(pitEntry->m_unsatisfyTimer);
  pitEntry->m_unsatisfyTimer = scheduler::schedule(lastExpiryFromNow,
    bind(&Forwarder::onInterestUnsatisfied, this, pitEntry));
}

void
Forwarder::setStragglerTimer(const shared_ptr<pit::Entry>& pitEntry, bool isSatisfied,
                             ndn::optional<time::milliseconds> dataFreshnessPeriod)
{
  time::nanoseconds stragglerTime = time::milliseconds(100);

  scheduler::cancel(pitEntry->m_stragglerTimer);
  pitEntry->m_stragglerTimer = scheduler::schedule(stragglerTime,
    bind(&Forwarder::onInterestFinalize, this, pitEntry, isSatisfied, dataFreshnessPeriod));
}

void
Forwarder::cancelUnsatisfyAndStragglerTimer(pit::Entry& pitEntry)
{
  scheduler::cancel(pitEntry.m_unsatisfyTimer);
  scheduler::cancel(pitEntry.m_stragglerTimer);
}

static inline void
insertNonceToDnl(DeadNonceList& dnl, const pit::Entry& pitEntry,
                 const pit::OutRecord& outRecord)
{
  dnl.add(pitEntry.getName(), outRecord.getLastNonce());
}

void
Forwarder::insertDeadNonceList(pit::Entry& pitEntry, bool isSatisfied,
                               ndn::optional<time::milliseconds> dataFreshnessPeriod, Face* upstream)
{
  // need Dead Nonce List insert?
  bool needDnl = true;
  if (isSatisfied) {
    BOOST_ASSERT(dataFreshnessPeriod);
    BOOST_ASSERT(*dataFreshnessPeriod >= time::milliseconds::zero());
    needDnl = static_cast<bool>(pitEntry.getInterest().getMustBeFresh()) &&
              *dataFreshnessPeriod < m_deadNonceList.getLifetime();
  }

  if (!needDnl) {
    return;
  }

  // Dead Nonce List insert
  if (upstream == nullptr) {
    // insert all outgoing Nonces
    const pit::OutRecordCollection& outRecords = pitEntry.getOutRecords();
    std::for_each(outRecords.begin(), outRecords.end(),
                  bind(&insertNonceToDnl, ref(m_deadNonceList), cref(pitEntry), _1));
  }
  else {
    // insert outgoing Nonce of a specific face
    pit::OutRecordCollection::iterator outRecord = pitEntry.getOutRecord(*upstream);
    if (outRecord != pitEntry.getOutRecords().end()) {
      m_deadNonceList.add(pitEntry.getName(), outRecord->getLastNonce());
    }
  }
}

} // namespace nfd
