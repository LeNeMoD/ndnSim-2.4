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

#include "ndn-producer.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

//Dome
//#include "ndn-cxx/src/futurePositionInfo.hpp"
//#include "../ndn-cxx/src/futurePositionInfo.hpp"
#include "ns3/ndnSIM/ndn-cxx/futurePositionInfo.hpp"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/parabolic-antenna-model.h"



NS_LOG_COMPONENT_DEFINE("ndn.Producer");

ndn::FuturePositionInfo futurePositionInfo;

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Producer);
TypeId
Producer::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::Producer")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<Producer>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&Producer::m_prefix), MakeNameChecker())
      .AddAttribute(
         "Postfix",
         "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
         StringValue("/"), MakeNameAccessor(&Producer::m_postfix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&Producer::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())
      .AddAttribute("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
                    TimeValue(Seconds(0)), MakeTimeAccessor(&Producer::m_freshness),
                    MakeTimeChecker())
      .AddAttribute(
         "Signature",
         "Fake signature, 0 valid signature (default), other values application-specific",
         UintegerValue(0), MakeUintegerAccessor(&Producer::m_signature),
         MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator",
                    "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(&Producer::m_keyLocator), MakeNameChecker());
  return tid;
}

Producer::Producer()
{
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
Producer::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);

  if(ns3::Simulator::Now()/1000000>5){
	  std::cout<<"turning Antenna to 90 at time s: " <<ns3::Simulator::Now()/1000000<<std::endl;
	  ns3::Ptr<ns3::Node> node = GetNode();
	  Ptr<ParabolicAntennaModel> parabolicAntennaPtr = node->GetObject<ParabolicAntennaModel>();
	  double degrees = 90;
	  parabolicAntennaPtr->TurnAntenna(degrees);
  }
}

void
Producer::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}

void
Producer::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;

  Name dataName(interest->getName());
  // dataName.append(m_postfix);
  // dataName.appendVersion();

  auto data = make_shared<Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

  data->setContent(make_shared< ::ndn::Buffer>(m_virtualPayloadSize));

  Signature signature;
  SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

  if (m_keyLocator.size() > 0) {
    signatureInfo.setKeyLocator(m_keyLocator);
  }

  signature.setInfo(signatureInfo);
  signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

  data->setSignature(signature);

  NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << data->getName());

  //Dome
  ns3::Ptr<ns3::Node> node = GetNode();

//  Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-test2-n3.txt");
// 	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-Slow-Fast-3n-10s.txt");
//	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-3n.txt");
	Ns2MobilityHelper ns2MobHelper = Ns2MobilityHelper("ns-movements-stationary-Ring-7n.txt");




  double at = 5;

  double posX = ns2MobHelper.GetPositionFromTCLFileForNodeAtTime("ndn-producer",node->GetId(),at).x;
  double posY = ns2MobHelper.GetPositionFromTCLFileForNodeAtTime("ndn-producer",node->GetId(),at).y ;


//  std::cout<< "check position-X +5s pass in producer  :" << posX << " node id: " << node->GetId() <<std::endl;
//  std::cout<< "check position-Y +5s pass in producer  :" << posY << " node id: " << node->GetId() <<std::endl;

  futurePositionInfo.setFutureLocationX(posX);
  futurePositionInfo.setFutureLocationY(posY);
  futurePositionInfo.setTimeAtFutureLocation(at);

  data->setFuturePositionInfo(futurePositionInfo);

//  std::cout<<"data futurPos in producer contains: "<< data->getFuturePositionInfo().m_location_X_Coord <<std::endl;
//  std::cout<<"data futurPos in producer contains: "<< data->getFuturePositionInfo().m_location_Y_Coord <<std::endl;

  // to create real wire encoding
  data->wireEncode();

  m_transmittedDatas(data, this, m_face);
  m_appLink->onReceiveData(*data);
}

} // namespace ndn
} // namespace ns3
