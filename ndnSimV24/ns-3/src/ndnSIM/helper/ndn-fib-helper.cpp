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

#include "ndn-fib-helper.hpp"

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/net-device.h"
#include "ns3/channel.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/core-config.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/callback.h"
#include "ns3/node-list.h"
#include "ns3/data-rate.h"

#include "daemon/mgmt/fib-manager.hpp"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"

//Dome
#include "ns3/mobility-module.h"
#include "../../mobility/model/constant-velocity-mobility-model.h"



namespace ns3 {
namespace ndn {

NS_LOG_COMPONENT_DEFINE("ndn.FibHelper");

void
FibHelper::AddNextHop(const ControlParameters& parameters, Ptr<Node> node)
{
  NS_LOG_DEBUG("Add Next Hop command was initialized");
  //Dome
  std::cout << "Add Next Hop command was initialized" << std::endl;

  Block encodedParameters(parameters.wireEncode());

  Name commandName("/localhost/nfd/fib");
  commandName.append("add-nexthop");
  commandName.append(encodedParameters);

  shared_ptr<Interest> command(make_shared<Interest>(commandName));
  StackHelper::getKeyChain().sign(*command);

  Ptr<L3Protocol> l3protocol = node->GetObject<L3Protocol>();
  l3protocol->injectInterest(*command);
}

void
FibHelper::RemoveNextHop(const ControlParameters& parameters, Ptr<Node> node)
{
  NS_LOG_DEBUG("Remove Next Hop command was initialized");
  Block encodedParameters(parameters.wireEncode());

  Name commandName("/localhost/nfd/fib");
  commandName.append("remove-nexthop");
  commandName.append(encodedParameters);

  shared_ptr<Interest> command(make_shared<Interest>(commandName));
  StackHelper::getKeyChain().sign(*command);

  Ptr<L3Protocol> l3protocol = node->GetObject<L3Protocol>();
  l3protocol->injectInterest(*command);
}

void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, shared_ptr<Face> face, int32_t metric)
{
  NS_LOG_LOGIC("[" << node->GetId() << "]$ route add " << prefix << " via " << face->getLocalUri()
                   << " metric " << metric);

  // Get L3Protocol object
  Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
  // Get the forwarder instance
  shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

  ControlParameters parameters;
  parameters.setName(prefix);
  parameters.setFaceId(face->getId());
  parameters.setCost(metric);

  AddNextHop(parameters, node);
}

void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, shared_ptr<Face> face, int32_t metric, std::string macAddress)
{
  NS_LOG_LOGIC("[" << node->GetId() << "]$ route add " << prefix << " via " << face->getLocalUri()
                   << " metric " << metric);

  // Get L3Protocol object
  Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
  // Get the forwarder instance
  shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

  ControlParameters parameters;
  parameters.setName(prefix);
  parameters.setFaceId(face->getId());
  parameters.setCost(metric);
  parameters.setMac(macAddress);

  AddNextHop(parameters, node);
}
//Dome
void FibHelper::AddRoute(Ptr<Node> node, const Name& prefix,
		shared_ptr<Face> face, int32_t metric, std::string macAddress,
		double positionX, double positionY, double positionZ,
		double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	NS_LOG_LOGIC(
			"[" << node->GetId() << "]$ route add " << prefix << " via " << face->getLocalUri() << " metric " << metric);

	std::cout << "fibhelper is called" << std::endl;
	// Get L3Protocol object
	Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
	// Get the forwarder instance
	shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

	std::cout<<"ndn-Fib-Helper- addRoute (my method) :for Node "<<node->GetId()<<" positionX: "<< positionX <<std::endl;
	std::cout<<"ndn-Fib-Helper- addRoute (my method) :for Node "<<node->GetId()<<" positionY: "<< positionY <<std::endl;

	ControlParameters parameters;
	parameters.setName(prefix);
	parameters.setFaceId(face->getId());
	parameters.setCost(metric);
	parameters.setMac(macAddress);
	parameters.setPositionX(positionX);
	parameters.setPositionY(positionY);
	parameters.setPositionZ(positionZ);
	parameters.setFuturePositionX(futurePositionX);
	parameters.setFuturePositionY(futurePositionY);
	parameters.setTimeAtFuturePosition(timeAtFuturePosition);
	parameters.setFuturePositionSettedInfo(isFuturePositionSet);

	AddNextHop(parameters, node);
}


void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId, int32_t metric)
{
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  AddRoute(node, prefix, face, metric);
}

void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId, int32_t metric, std::string macAddress)
{
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  AddRoute(node, prefix, face, metric, macAddress);
}
//Dome
void FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId,
		int32_t metric, std::string macAddress, double positionX,
		double positionY, double positionZ, double futurePositionX,
		double futurePositionY, double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
	NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

	shared_ptr<Face> face = ndn->getFaceById(faceId);
	NS_ASSERT_MSG(face != 0,
			"Face with ID [" << faceId << "] does not exist on node [" << node->GetId() << "]");

	AddRoute(node, prefix, face, metric, macAddress, positionX, positionY,
			positionZ, futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}

void
FibHelper::AddRoute(const std::string& nodeName, const Name& prefix, uint32_t faceId,
                    int32_t metric)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node [" << nodeName
                                            << "]");

  AddRoute(node, prefix, face, metric);
}

void
FibHelper::AddRoute(const std::string& nodeName, const Name& prefix, uint32_t faceId,
                    int32_t metric, std::string macAddress)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node [" << nodeName
                                            << "]");

  AddRoute(node, prefix, face, metric, macAddress);
}
//Dome
void FibHelper::AddRoute(const std::string& nodeName, const Name& prefix,
		uint32_t faceId, int32_t metric, std::string macAddress,
		double positionX, double positionY, double positionZ,
		double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<Node> node = Names::Find<Node>(nodeName);
	NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

	Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
	NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

	shared_ptr<Face> face = ndn->getFaceById(faceId);
	NS_ASSERT_MSG(face != 0,
			"Face with ID [" << faceId << "] does not exist on node [" << nodeName << "]");

	AddRoute(node, prefix, face, metric, macAddress, positionX, positionY,
			positionZ, futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}


void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, Ptr<Node> otherNode, int32_t metric)
{
  for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
    Ptr<PointToPointNetDevice> netDevice =
      DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
    if (netDevice == 0)
      continue;

    Ptr<Channel> channel = netDevice->GetChannel();
    if (channel == 0)
      continue;

    if (channel->GetDevice(0)->GetNode() == otherNode
        || channel->GetDevice(1)->GetNode() == otherNode) {
      Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
      NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

      shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
      NS_ASSERT_MSG(face != 0, "There is no face associated with the p2p link");

      AddRoute(node, prefix, face, metric);

      return;
    }
  }

  NS_FATAL_ERROR("Cannot add route: Node# " << node->GetId() << " and Node# " << otherNode->GetId()
                                            << " are not connected");
}

void
FibHelper::AddRoute(Ptr<Node> node, const Name& prefix, Ptr<Node> otherNode, int32_t metric, std::string macAddress)
{
  for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
    Ptr<PointToPointNetDevice> netDevice =
      DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
    if (netDevice == 0)
      continue;

    Ptr<Channel> channel = netDevice->GetChannel();
    if (channel == 0)
      continue;

    if (channel->GetDevice(0)->GetNode() == otherNode
        || channel->GetDevice(1)->GetNode() == otherNode) {
      Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
      NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

      shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
      NS_ASSERT_MSG(face != 0, "There is no face associated with the p2p link");

      AddRoute(node, prefix, face, metric, macAddress);

      return;
    }
  }

  NS_FATAL_ERROR("Cannot add route: Node# " << node->GetId() << " and Node# " << otherNode->GetId()
                                            << " are not connected");
}
//Dome
void FibHelper::AddRoute(Ptr<Node> node, const Name& prefix,
		Ptr<Node> otherNode, int32_t metric, std::string macAddress,
		double positionX, double positionY, double positionZ,
		double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
		Ptr<PointToPointNetDevice> netDevice =
				DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
		if (netDevice == 0)
			continue;

		Ptr<Channel> channel = netDevice->GetChannel();
		if (channel == 0)
			continue;

		if (channel->GetDevice(0)->GetNode() == otherNode
				|| channel->GetDevice(1)->GetNode() == otherNode) {
			Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
			NS_ASSERT_MSG(ndn != 0,
					"Ndn stack should be installed on the node");

			shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
			NS_ASSERT_MSG(face != 0,
					"There is no face associated with the p2p link");

			AddRoute(node, prefix, face, metric, macAddress, positionX,
					positionY, positionZ, futurePositionX, futurePositionY,
					timeAtFuturePosition, isFuturePositionSet);

			return;
		}
	}

	NS_FATAL_ERROR(
			"Cannot add route: Node# " << node->GetId() << " and Node# " << otherNode->GetId() << " are not connected");
}


void
FibHelper::AddRoute(const std::string& nodeName, const Name& prefix,
                    const std::string& otherNodeName, int32_t metric)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
  NS_ASSERT_MSG(otherNode != 0, "Node [" << otherNodeName << "] does not exist");

  AddRoute(node, prefix, otherNode, metric);
}

void
FibHelper::AddRoute(const std::string& nodeName, const Name& prefix,
                    const std::string& otherNodeName, int32_t metric, std::string macAddress)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
  NS_ASSERT_MSG(otherNode != 0, "Node [" << otherNodeName << "] does not exist");

  AddRoute(node, prefix, otherNode, metric, macAddress);
}
//Dome
void FibHelper::AddRoute(const std::string& nodeName, const Name& prefix,
		const std::string& otherNodeName, int32_t metric,
		std::string macAddress, double positionX, double positionY,
		double positionZ, double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<Node> node = Names::Find<Node>(nodeName);
	NS_ASSERT_MSG(node != 0, "Node [" << nodeName << "] does not exist");

	Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
	NS_ASSERT_MSG(otherNode != 0,
			"Node [" << otherNodeName << "] does not exist");

	AddRoute(node, prefix, otherNode, metric, macAddress, positionX, positionY,
			positionZ, futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, shared_ptr<Face> face)
{
  // Get L3Protocol object
  Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
  // Get the forwarder instance
  shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

  ControlParameters parameters;
  parameters.setName(prefix);
  parameters.setFaceId(face->getId());

  RemoveNextHop(parameters, node);
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, shared_ptr<Face> face, std::string macAddress)
{
  // Get L3Protocol object
  Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
  // Get the forwarder instance
  shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

  ControlParameters parameters;
  parameters.setName(prefix);
  parameters.setFaceId(face->getId());
  parameters.setMac(macAddress);

  RemoveNextHop(parameters, node);
}
//Dome
void FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix,
		shared_ptr<Face> face, std::string macAddress, double positionX,
		double positionY, double positionZ, double futurePositionX,
		double futurePositionY, double timeAtFuturePosition, double isFuturePositionSet) {
	// Get L3Protocol object
	Ptr<L3Protocol> L3protocol = node->GetObject<L3Protocol>();
	// Get the forwarder instance
	shared_ptr<nfd::Forwarder> m_forwarder = L3protocol->getForwarder();

	ControlParameters parameters;
	parameters.setName(prefix);
	parameters.setFaceId(face->getId());
	parameters.setMac(macAddress);
	parameters.setPositionX(positionX);
	parameters.setPositionY(positionY);
	parameters.setPositionZ(positionZ);
	parameters.setFuturePositionX(futurePositionX);
	parameters.setFuturePositionY(futurePositionY);
	parameters.setTimeAtFuturePosition(timeAtFuturePosition);
	parameters.setFuturePositionSettedInfo(isFuturePositionSet);

	RemoveNextHop(parameters, node);
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId)
{
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  RemoveRoute(node, prefix, face);
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId, std::string macAddress)
{
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  RemoveRoute(node, prefix, face, macAddress);
}
//Dome
void FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, uint32_t faceId,
		std::string macAddress, double positionX, double positionY,
		double positionZ, double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
	NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

	shared_ptr<Face> face = ndn->getFaceById(faceId);
	NS_ASSERT_MSG(face != 0,
			"Face with ID [" << faceId << "] does not exist on node [" << node->GetId() << "]");

	RemoveRoute(node, prefix, face, macAddress, positionX, positionY, positionZ,
			futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}


void
FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix, uint32_t faceId)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  RemoveRoute(node, prefix, face);
}

void
FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix, uint32_t faceId, std::string macAddress)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
  NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

  shared_ptr<Face> face = ndn->getFaceById(faceId);
  NS_ASSERT_MSG(face != 0, "Face with ID [" << faceId << "] does not exist on node ["
                                            << node->GetId() << "]");

  RemoveRoute(node, prefix, face, macAddress);
}
//Dome
void FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix,
		uint32_t faceId, std::string macAddress, double positionX,
		double positionY, double positionZ, double futurePositionX,
		double futurePositionY, double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<Node> node = Names::Find<Node>(nodeName);
	Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
	NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

	shared_ptr<Face> face = ndn->getFaceById(faceId);
	NS_ASSERT_MSG(face != 0,
			"Face with ID [" << faceId << "] does not exist on node [" << node->GetId() << "]");

	RemoveRoute(node, prefix, face, macAddress, positionX, positionY, positionZ,
			futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, Ptr<Node> otherNode)
{
  for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
    Ptr<PointToPointNetDevice> netDevice =
      DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
    if (netDevice == 0)
      continue;

    Ptr<Channel> channel = netDevice->GetChannel();
    if (channel == 0)
      continue;

    if (channel->GetDevice(0)->GetNode() == otherNode
        || channel->GetDevice(1)->GetNode() == otherNode) {
      Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
      NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

      shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
      NS_ASSERT_MSG(face != 0, "There is no face associated with the p2p link");

      RemoveRoute(node, prefix, face);

      return;
    }
  }

  NS_FATAL_ERROR("Cannot remove route: Node# " << node->GetId() << " and Node# " << otherNode->GetId()
                                            << " are not connected");
}

void
FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix, Ptr<Node> otherNode, std::string macAddress)
{
  for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
    Ptr<PointToPointNetDevice> netDevice =
      DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
    if (netDevice == 0)
      continue;

    Ptr<Channel> channel = netDevice->GetChannel();
    if (channel == 0)
      continue;

    if (channel->GetDevice(0)->GetNode() == otherNode
        || channel->GetDevice(1)->GetNode() == otherNode) {
      Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
      NS_ASSERT_MSG(ndn != 0, "Ndn stack should be installed on the node");

      shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
      NS_ASSERT_MSG(face != 0, "There is no face associated with the p2p link");

      RemoveRoute(node, prefix, face, macAddress);

      return;
    }
  }

  NS_FATAL_ERROR("Cannot remove route: Node# " << node->GetId() << " and Node# " << otherNode->GetId()
                                            << " are not connected");
}
//Dome

void FibHelper::RemoveRoute(Ptr<Node> node, const Name& prefix,
		Ptr<Node> otherNode, std::string macAddress, double positionX,
		double positionY, double positionZ, double futurePositionX,
		double futurePositionY, double timeAtFuturePosition, double isFuturePositionSet) {
	for (uint32_t deviceId = 0; deviceId < node->GetNDevices(); deviceId++) {
		Ptr<PointToPointNetDevice> netDevice =
				DynamicCast<PointToPointNetDevice>(node->GetDevice(deviceId));
		if (netDevice == 0)
			continue;

		Ptr<Channel> channel = netDevice->GetChannel();
		if (channel == 0)
			continue;

		if (channel->GetDevice(0)->GetNode() == otherNode
				|| channel->GetDevice(1)->GetNode() == otherNode) {
			Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();
			NS_ASSERT_MSG(ndn != 0,
					"Ndn stack should be installed on the node");

			shared_ptr<Face> face = ndn->getFaceByNetDevice(netDevice);
			NS_ASSERT_MSG(face != 0,
					"There is no face associated with the p2p link");

			RemoveRoute(node, prefix, face, macAddress, positionX, positionY,
					positionZ, futurePositionX, futurePositionY,
					timeAtFuturePosition, isFuturePositionSet);

			return;
		}
	}

	NS_FATAL_ERROR(
			"Cannot remove route: Node# " << node->GetId() << " and Node# " << otherNode->GetId() << " are not connected");
}

void
FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix,
                    const std::string& otherNodeName)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
  RemoveRoute(node, prefix, otherNode);
}

void
FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix,
                    const std::string& otherNodeName, std::string macAddress)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
  RemoveRoute(node, prefix, otherNode, macAddress);
}
//Dome
void FibHelper::RemoveRoute(const std::string& nodeName, const Name& prefix,
		const std::string& otherNodeName, std::string macAddress,
		double positionX, double positionY, double positionZ,
		double futurePositionX, double futurePositionY,
		double timeAtFuturePosition, double isFuturePositionSet) {
	Ptr<Node> node = Names::Find<Node>(nodeName);
	Ptr<Node> otherNode = Names::Find<Node>(otherNodeName);
	RemoveRoute(node, prefix, otherNode, macAddress, positionX, positionY,
			positionZ, futurePositionX, futurePositionY, timeAtFuturePosition, isFuturePositionSet);
}


} // namespace ndn

} // namespace ns
