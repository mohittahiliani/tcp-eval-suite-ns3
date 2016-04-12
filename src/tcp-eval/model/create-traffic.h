/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Dharmendra Kumar Mishra <dharmendra.nitk@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// Define an object to create traffic in tcp-eval.

#ifndef CREATE_TRAFFIC_H
#define CREATE_TRAFFIC_H

#include <stdint.h>

#include "traffic-parameters.h"
#include "ns3/object.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

/**
 * \brief Create Traffic for dumbbell and parking-lot topology
 *
 * The methods of this class take Topology as its input parameter
 * and then generate the traffic accordingly
 */
class CreateTraffic : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   *
   * The start time of flows is randomized by using
   * a uniform random variable
   */
  CreateTraffic (void);

  /**
   * \brief Destructor
   */
  ~CreateTraffic (void);

  /**
   * \brief Generate next random value which will be used to
   * randomize the flow start times
   *
   * \return random value
   */
  double GetRandomValue (void) const;

  /**
   * \brief Create forward FTP traffic for dumbbell topology
   *
   * \param dumbbell Object of dumbbell topology
   * \param flows Number of forward FTP flows
   * \param offset Index of a chain of nodes that generate forward FTP traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateFwdFtpTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create reverse FTP traffic for dumbbell topology
   *
   * \param dumbbell Object of dumbbell topology
   * \param flows Number of reverse FTP flows
   * \param offset Index of a chain of nodes that generate reverse FTP traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateRevFtpTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create two-way voice traffic for dumbbell topology
   *
   * \param dumbbell Object of dumbbell topology
   * \param flows Number of two-way voice traffic flows
   * \param offset Index of a chain of nodes that generate two-way voice traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateVoiceTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create forward streaming traffic for dumbbell topology
   *
   * \param dumbbell Object of dumbbell topology
   * \param flows Number of forward streaming flows
   * \param offset Index of a chain of nodes that generate forward streaming traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateFwdStreamingTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create reverse streaming traffic for dumbbell topology
   *
   * \param dumbbell Object of dumbbell topology
   * \param flows Number of reverse streaming flows
   * \param offset Index of a chain of nodes that generate reverse streaming traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateRevStreamingTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create forward FTP traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param flows Number of forward FTP flows
   * \param offset Index of a chain of nodes that generate forward FTP traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateFwdFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create reverse FTP traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param flows Number of reverse FTP flows
   * \param offset Index of a chain of nodes that generate reverse FTP traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateRevFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create cross FTP traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param crossFlows Number of cross FTP flows
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateCrossFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t crossFlows, Ptr<TrafficParameters> traffic);

  /**\brief Create two-way voice traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param flows Number of two-way voice traffic flows
   * \param offset Index of a chain of nodes that generate two-way voice traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   *
   */
  void CreateVoiceTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create forward streaming traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param flows Number of forward streaming flows
   * \param offset Index of a chain of nodes that generate forward streaming traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateFwdStreamingTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Create reverse streaming traffic for parking-lot topology
   *
   * \param parkingLot Object of parking-lot topology
   * \param flows Number of reverse streaming flows
   * \param offset Index of a chain of nodes that generate reverse streaming traffic
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   */
  void CreateRevStreamingTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows, uint32_t offset, Ptr<TrafficParameters> traffic);

  /**
   * \brief Converts a value to string.
   *
   * This method is used because std::to_string() works with c++11
   * and std::itoa is not a standard library
   *
   * \param data The value which is to be converted to string.
   */
  template <typename T>
  std::string to_string (const T& data);

private:
  Ptr<UniformRandomVariable> m_randVar;         //!< Random variable to randomize the start time for traffic flows

};

}

#endif /* CREATE_TRAFFIC_H */

