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

// Define an object to create a dumbbell topology in tcp-eval.

#ifndef DUMBBELL_TOPOLOGY_H
#define DUMBBELL_TOPOLOGY_H

#include <stdint.h>

#include "configure-topology.h"
#include "traffic-parameters.h"
#include "create-traffic.h"

namespace ns3 {

/**
 * \brief Configures Dumbbell topology and simulates the traffic accordingly.
 *
 *  It extends ConfigureTopology and invokes PointToPointDumbbellHelper to create a
 *  the topology. Total number of left and right nodes are created by considering
 *  all different types of traffic flows configured by the user.
 *  Example: if there are 5 forwardFtp flows, 4 reverseFtp flows and 2 voiceFlows,
 *  nodes 0-4 initiate forwardFtp flows (left -> right), nodes 5-8 initiate reverseFtp
 *  flows (right -> left) and nodes 9-10 initiate voiceFlows (left <-> right)
 */
class DumbbellTopology : public ConfigureTopology
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
  DumbbellTopology (void);

  /**
   * \brief Destructor
   */
  ~DumbbellTopology (void);

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

  /**
   * \brief Invokes methods for creating dumbbell topology and simulating traffic
   *
   * It configures PointToPointChannel, invokes topology creation class and then calls methods
   * to create traffic on this topology. Finally, this method invokes Stats class to trace the
   * required statistics.
   *
   * \param traffic Object of TrafficParameters class that contains the
   *                information of traffic related parameters.
   * \param fileName the name of the file where stats are dumped.
   */
  void CreateDumbbellTopology (Ptr<TrafficParameters> traffic, std::string fileName);
};

}

#endif /*DUMBBELL_TOPOLOGY_H*/
