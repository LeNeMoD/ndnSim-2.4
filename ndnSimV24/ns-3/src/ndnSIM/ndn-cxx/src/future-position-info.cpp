/*
 * futurePosition.cpp
 *
 *  Created on: Nov 5, 2017
 *      Author: domenico
 */

#include "future-position-info.hpp"

#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FuturePositionInfo>));
BOOST_CONCEPT_ASSERT((WireEncodable<FuturePositionInfo>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<FuturePositionInfo>));
BOOST_CONCEPT_ASSERT((WireDecodable<FuturePositionInfo>));
static_assert(std::is_base_of<tlv::Error, FuturePositionInfo::Error>::value,
              "FuturePositionInfo::Error must inherit from tlv::Error");


//Ask Eirini how to initialize properly the future position with his variables.
FuturePositionInfo::FuturePositionInfo() {
	m_location_X_Coord = 3215;
	m_location_Y_Coord = 3215;
	m_location_Z_Coord_Velocity = 3215;
	m_timeAtFuturePosition = 3215;
	m_futurePositionWasSet = 0;

}

FuturePositionInfo::FuturePositionInfo(const Block& block)
{
	wireDecode(block);
}

FuturePositionInfo::FuturePositionInfo(FuturePositionInfo& futurePositionInfo) {
	m_location_X_Coord = futurePositionInfo.getFutureLocation_X();
	m_location_Y_Coord = futurePositionInfo.getFutureLocation_Y();
	m_location_Z_Coord_Velocity = futurePositionInfo.getFutureLocation_Z();
	m_timeAtFuturePosition = futurePositionInfo.getTimeAtFutureLocation();
	m_futurePositionWasSet = 1;

}

FuturePositionInfo::FuturePositionInfo(ns3::Vector positionVector, double timeAtFutureLocation) {
	m_location_X_Coord = positionVector.x;
	m_location_Y_Coord = positionVector.y;
	m_location_Z_Coord_Velocity = positionVector.z;
	m_timeAtFuturePosition = timeAtFutureLocation;
	m_futurePositionWasSet = 1;

}

template<encoding::Tag TAG>
size_t
FuturePositionInfo::wireEncode(EncodingImpl<TAG>& encoder) const {
	size_t totalLength = 0;

	// if futurePositions were set the encode time
	if (m_futurePositionWasSet) {
				totalLength += prependNonNegativeIntegerBlock(encoder,
						tlv::FuturePositionZ, m_location_Z_Coord_Velocity);
			}

	// if futurePositions were set then encode them
	if (m_futurePositionWasSet) {

			totalLength += prependNonNegativeIntegerBlock(encoder,
					tlv::TimeAtFuturePosition, m_timeAtFuturePosition);
		}

	if (m_futurePositionWasSet) {
				totalLength += prependNonNegativeIntegerBlock(encoder,
						tlv::FuturePositionY, m_location_Y_Coord);
			}

	if (m_futurePositionWasSet) {
		totalLength += prependNonNegativeIntegerBlock(encoder,
				tlv::FuturePositionX, m_location_X_Coord);
	}

	if (m_futurePositionWasSet) {
		totalLength += prependNonNegativeIntegerBlock(encoder,
				tlv::FuturePositionZ, m_location_Z_Coord_Velocity);
	}

	if (m_futurePositionWasSet) {
		totalLength += prependNonNegativeIntegerBlock(encoder,
				tlv::FuturePositionWasSet, m_futurePositionWasSet);
	}

	totalLength += encoder.prependVarNumber(totalLength);
	totalLength += encoder.prependVarNumber(tlv::FuturePositionInfoData);
	return totalLength;

}

const Block&
FuturePositionInfo::wireEncode() const {
	{
		if (m_Wire_futurePositionInfo.hasWire())
			return m_Wire_futurePositionInfo;

		EncodingEstimator estimator;
		size_t estimatedSize = wireEncode(estimator);

		EncodingBuffer buffer(estimatedSize, 0);
		wireEncode(buffer);

		m_Wire_futurePositionInfo = buffer.block();
		return m_Wire_futurePositionInfo;
	}

}

template size_t
FuturePositionInfo::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
FuturePositionInfo::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

void
FuturePositionInfo::wireDecode(const Block& wire) {

	m_Wire_futurePositionInfo = wire;
	m_Wire_futurePositionInfo.parse();

	//Dome !!! missing else statements if decoding fails
	//FuturePosistion
	Block::element_const_iterator val= m_Wire_futurePositionInfo.find(tlv::FuturePositionX);
	if (val != m_Wire_futurePositionInfo.elements_end()) {
		m_location_X_Coord = readNonNegativeInteger(*val);
		++val;
	}
	val= m_Wire_futurePositionInfo.find(tlv::FuturePositionY);
	if (val != m_Wire_futurePositionInfo.elements_end()) {
		m_location_Y_Coord = readNonNegativeInteger(*val);

		++val;
	}
	val= m_Wire_futurePositionInfo.find(tlv::FuturePositionZ);
	if (val != m_Wire_futurePositionInfo.elements_end()) {
		m_location_Z_Coord_Velocity = readNonNegativeInteger(*val);
		++val;
	}
	val= m_Wire_futurePositionInfo.find(tlv::TimeAtFuturePosition);
	//TimeAtFuturePosition
	if (val != m_Wire_futurePositionInfo.elements_end())
		{
		m_timeAtFuturePosition = readNonNegativeInteger(*val);
		++val;
	}

	val = m_Wire_futurePositionInfo.find(tlv::FuturePositionWasSet);
	//IsFuturePositionSet
	if (val != m_Wire_futurePositionInfo.elements_end()) {
		m_futurePositionWasSet = readNonNegativeInteger(*val);
		++val;
	}

}

std::ostream&
operator<<(std::ostream& os, const FuturePositionInfo& info)
{
  // FuturePositionX
  os << "FuturePositionX: " << info.getFutureLocation_X();

  // FuturePositionY
  os << ", FuturePositionY: " << info.getFutureLocation_Y();

  // FuturePositionZ
  os << ", FuturePositionZ: " << info.getFutureLocation_Z();

  // TimeAtFuturePosition
  os << ", TimeAtFuturePosition: " << info.getTimeAtFutureLocation();

  // Position Was setted
  os << ", FuturePositionWasSet: " << info.isfuturePositionSet();

  return os;
}

bool
FuturePositionInfo::operator ==(const FuturePositionInfo& other) const{
	return wireEncode() == other.wireEncode();
}

double
FuturePositionInfo::getTimeAtFutureLocation() const{
	return m_timeAtFuturePosition;
}

FuturePositionInfo&
FuturePositionInfo::setTimeAtFutureLocation(double timeAtFutureLocation){
	m_Wire_futurePositionInfo.reset();
	m_timeAtFuturePosition = timeAtFutureLocation;
	return *this;
}

ns3::Vector
FuturePositionInfo::getFuturePositionVector() {
	m_futurePositionVector.x = m_location_X_Coord;
	m_futurePositionVector.y = m_location_Y_Coord;
	m_futurePositionVector.z = m_location_Z_Coord_Velocity;
	return m_futurePositionVector;
}


FuturePositionInfo&
FuturePositionInfo::setFuturePositionVector(ns3::Vector positionVector){
	m_Wire_futurePositionInfo.reset();
	m_futurePositionVector = positionVector;
	return *this;
}

FuturePositionInfo&
FuturePositionInfo::setFutureLocationX(double futureLocation_X){
	m_Wire_futurePositionInfo.reset();
	m_location_X_Coord = futureLocation_X;
	return *this;
}

FuturePositionInfo&
FuturePositionInfo::setFutureLocationY(double futureLocation_Y){
	m_Wire_futurePositionInfo.reset();
	m_location_Y_Coord = futureLocation_Y;
	return *this;
}

FuturePositionInfo&
FuturePositionInfo::setFutureLocationZ(double futureLocation_Z){
	m_Wire_futurePositionInfo.reset();
	m_location_Z_Coord_Velocity = futureLocation_Z;
	return *this;
}

double
FuturePositionInfo::getFutureLocation_X() const
{
  return m_location_X_Coord;
}

double
FuturePositionInfo::getFutureLocation_Y() const
{
  return m_location_Y_Coord;
}

double
FuturePositionInfo::getFutureLocation_Z() const
{
  return m_location_Z_Coord_Velocity;
}

double
FuturePositionInfo::isfuturePositionSet() const
{
	return m_futurePositionWasSet;
}

FuturePositionInfo&
FuturePositionInfo::setFuturePositionWasSet(double wasItSet){
	m_Wire_futurePositionInfo.reset();
	m_futurePositionWasSet = wasItSet;
	return *this;
}

}

