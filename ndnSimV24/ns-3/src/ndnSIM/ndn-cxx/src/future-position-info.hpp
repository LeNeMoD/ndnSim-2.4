/*
 * futurePosition.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: domenico
 */

#ifndef NDN_FUTURE_POSITION_INFO_HPP
#define NDN_FUTURE_POSITION_INFO_HPP

#include "encoding/block.hpp"
#include "../../../../../ns-3/src/core/model/vector.h"
#include "util/time.hpp"
#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/time.hpp"
#include "name-component.hpp"
#include <list>



namespace ndn {
class FuturePositionInfo;
class FuturePositionInfo : public enable_shared_from_this<FuturePositionInfo>
{

public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  FuturePositionInfo();

  /**
   * @brief Create from wire encoding
   */
  explicit
  FuturePositionInfo(const Block& block);

  //explicit
  //FuturePositionInfo(FuturePositionInfo& futurePositionInfo);

  explicit
  FuturePositionInfo(const ns3::Vector futurePositionVector, int timeAtFuturePosition);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);


  public:

  ns3::Vector
  getFuturePositionVector();

  FuturePositionInfo&
  setFuturePositionVector(ns3::Vector position);

  int
  getFutureLocation_X() const;

  int
  getFutureLocation_Y() const;

  int
  getFutureLocation_Z() const;

  FuturePositionInfo&
  setFutureLocationX(int futurelocation_X);

  FuturePositionInfo&
  setFutureLocationY(int futureLocation_Y);

  FuturePositionInfo&
  setFutureLocationZ(int futureLocation_Z);


  int
  getTimeAtFutureLocation() const;

  FuturePositionInfo&
  setTimeAtFutureLocation(int time);


//  bool
//  isfuturePositionSet() const;
//
//  FuturePositionInfo&
//  setFuturePositionWasSet(bool wasItSet);
  int
  isfuturePositionSet() const;

  FuturePositionInfo&
  setFuturePositionWasSet(int wasItSet);


  public:
  	  int m_location_X_Coord;
  	  int m_location_Y_Coord;
  	  int m_location_Z_Coord_Velocity;
  	  int m_timeAtFuturePosition;
  	  int m_futurePositionWasSet;
  	  ns3::Vector m_futurePositionVector;


  public: // EqualityComparable concept
    bool
    operator==(const FuturePositionInfo& other) const;

    bool
    operator!=(const FuturePositionInfo& other) const;

private:
  	  std::list<Block> m_appFuturePositionInfo;
  	  mutable Block m_Wire_futurePositionInfo;
};

std::ostream&
operator<<(std::ostream& os, const FuturePositionInfo& info);


} //end Namespace ndn

#endif /* NDN_FUTUREPOSITION_HPP_ */
