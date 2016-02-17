/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK, Surathkal
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

// Define an object to configure topology.

#ifndef CONFIGURE_TOPOLOGY_H
#define CONFIGURE_TOPOLOGY_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/core-module.h"
#include "ns3/traffic-parameters.h"
#include "ns3/create-graph.h"

namespace ns3 {

/**
 * \brief base class to configure the topology
 *
 * It initializes parameters used in the configuration of the topology.
 * Other classes like Dumbbell and ParkingLot should extend this class.
 */
class ConfigureTopology : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   */
  ConfigureTopology (void);

  /**
   * \brief Destructor
   */
  ~ConfigureTopology (void);

  /**
   * \brief Configures the traffic and graph parameters, and stores the
   *        results in a given file
   *
   * \param traffic Object of TrafficParameters class that contains the 
   *                information of traffic related parameters.
   * \param createGraph Object of CreateGraph class that contains the 
   *                    information of graph related parameters.
   * \param fileName the name of the file where stats are dumped.
   */
  virtual void CreateTopology (Ptr<TrafficParameters> traffic, Ptr<CreateGraph> createGraph, std::string fileName) = 0;

  /**
   * \brief Initializes the topology parameters
   *
   * \param traffic Object of TrafficParameters class that contains traffic information
   * \param nBottlenecks Number of Bottleneck links. For dumbbell it is 1 and for other topologies >= 1
   */
  void SetTopologyParameters (Ptr<TrafficParameters> traffic, uint32_t nBottlenecks);

  /**
   * \brief Sets RED queue's parameters. 
   * This is called when AQM is used in bottleneck links.
   */
  void SetRedParameters ();

  /**
   * \brief Get the bandwidth of bottleneck link
   *
   * \return the bandwidth of bottleneck link
   */
  double GetBottleneckBandwidth (void) const;

  /**
   * \brief Get the number of bottleneck links
   *
   * \return the number of bottleneck links
   */
  uint32_t BottleneckCount (void) const;

  /**
   * \brief Set round trip propagation delay in seconds
   *
   * \param rttp Round trip propagation delay
   */
  void SetRttp (Time rttp);

  /**
   * \brief Get round trip propagation delay in seconds
   *
   * \return round trip propagation delay in seconds
   */
  Time GetRttp (void) const;

  /**
   * \brief Set RTT difference in seconds
   *
   * \param rttDifference RTT difference in seconds
   */
  void SetRttDifference (Time rttDifference);

  /**
   * \brief Get RTT difference in seconds
   *
   * \return RTT difference in seconds
   */
  Time GetRttDifference (void) const;

  /**
   * \brief Get delay difference in seconds
   *
   * \return delay difference in seconds
   */
  Time GetDelayDifference (void) const;

  /**
   * \brief Get bottleneck link delay in seconds
   *
   * \return bottleneck link delay in seconds
   */
  Time GetBottleneckDelay (void) const;

  /**
   * \brief Get the size of buffer on bottleneck link
   *
   * \return the size of buffer on bottleneck link
   */
  uint32_t GetBottleneckBuffer (void) const;

  /**
   * \brief Get non-bottleneck link delay
   *
   * \return non-bottleneck link delay
   */
  Time GetNonBottleneckDelay (void) const;

  /**
   * \brief Get the bandwidth of non-bottleneck link
   *
   * \return the bandwidth of non-bottleneck link
   */
  double GetNonBottleneckBandwidth (void) const;

  /**
   * \brief Get the size of buffer on non-bottleneck link
   *
   * \return the size of buffer on non-bottleneck link
   */
  uint32_t GetNonBottleneckBuffer (void) const;

  /**
   * \brief Checks whether Tmix is enabled
   *
   * \return true if Tmix is enabled
   */
  bool IsTmixEnabled (void) const;

protected:
  double   m_bottleneckBandwidth;       //!< Bandwidth of bottleneck link in Mbps
  uint32_t m_nBottlenecks;              //!< Number of bottleneck links.
  Time     m_rttp;                      //!< Round trip propagation delay in seconds
  Time     m_rttDifference;             //!< Flow RTT difference in seconds
  Time     m_delayDifference;           //!< Delay difference in seconds
  Time     m_bottleneckDelay;           //!< Delay of bottleneck link in seconds
  uint32_t m_bottleneckBuffer;          //!< Size of the bottleneck buffer
  double   m_nonBottleneckBandwidth;    //!< Bandwidth of non-bottleneck link in Mbps
  Time     m_nonBottleneckDelay;        //!< Delay of non-bottleneck link in seconds
  uint32_t m_nonBottleneckBuffer;       //!< Size of the non-bottleneck buffer
  bool     m_tmixEnabled;               //!< If true, generate the traffic using Tmix
  double   m_bottleneckBufferBdp;       //!< Bandwidth-Delay Product for the bottleneck link
};
}

#endif /* CONFIGURE_TOPOLOGY_H */