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
  FuturePositionInfo(const ns3::Vector futurePositionVector, double timeAtFuturePosition);

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

  double
  getFutureLocation_X() const;

  double
  getFutureLocation_Y() const;

  double
  getFutureLocation_Z() const;

  FuturePositionInfo&
  setFutureLocationX(double futurelocation_X);

  FuturePositionInfo&
  setFutureLocationY(double futureLocation_Y);

  FuturePositionInfo&
  setFutureLocationZ(double futureLocation_Z);


  double
  getTimeAtFutureLocation() const;

  FuturePositionInfo&
  setTimeAtFutureLocation(double time);


//  bool
//  isfuturePositionSet() const;
//
//  FuturePositionInfo&
//  setFuturePositionWasSet(bool wasItSet);
  double
  isfuturePositionSet() const;

  FuturePositionInfo&
  setFuturePositionWasSet(double wasItSet);


  public:
  	  double m_location_X_Coord;
  	  double m_location_Y_Coord;
  	  double m_location_Z_Coord_Velocity;
  	  double m_timeAtFuturePosition;
  	  double m_futurePositionWasSet;
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
