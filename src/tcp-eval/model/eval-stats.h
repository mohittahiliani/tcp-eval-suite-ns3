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
 * Authors: Pratheek B. <pratheekbkasaragod@gmail.com>
 *          Sourabh S. Shenoy <sourabhsshenoy@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// Define an object that collects statistics at the bottleneck router.

#ifndef STATS_TCP_H
#define STATS_TCP_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-parameters.h"

#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"

namespace ns3 {

/**
 * \brief Calculates the bottleneck metrics and outputs it to files.
 *
 *  When installed on a node, it computes metrics such as link utilization,
 *  mean queue length and packet drop rate.
 */
class EvalStats : public Object
{
public:

  /**
   * \brief Constructor
   *
   * \param bandwidth Bandwidth used in the current simulation.
   * \param rttp Round Trip Time used in the current simulation.
   * \param filename The file to which the computed values are stored.
   */
  EvalStats (uint32_t bandwidth, Time rttp, std::string fileName);

  /**
   * \brief Destructor
   */
  ~EvalStats ();

  /**
   * \brief Calculates metrics every second.
   *
   * Computes metrics such as link utilization, queue size every second and
   * stores the value so the overall utilization and queue size can be computed later
   */
  void ComputeMetrics ();

  /**
   * \brief Computes the number of bytes sent
   *
   * It is called everytime a PhyTxBegin event occurs. We calculate the size of the
   * packet sent and store the value to compute the link utilization at a later stage.
   *
   * \param packet Packet whose size is to be obtained
   */
  void AggregateOverInterval (Ptr<const Packet> packet);

  /**
   * \brief Computes the size of the queue
   *
   * It is called everytime an Enqueue event occurs. We calculate the number of packets in
   * the queue and store the value to compute the mean queue size at a later stage.
   *
   * \param packet To conform with the signature of the method associated with the callback.
   *
   */
  void AggregateQueue (Ptr<const Packet> packet);

  /**
   * \brief Writes the metrics into a file.
   *
   * Metrics computed in the ComputeMetrics are written into a file.
   */
  void InsertIntoFile ();

  /**
    * \brief Connects the Trace Source of the node passed, to the callback functions
    *
    * Obtains references to netdevice and the queue from the node passed to it.
    * Connects the PhyTxBegin trace source of netdevice to AggregateOverInterval Callback and
    * Enqueue trace source to the AggregateQueue callback method
    *
    * \param node To obtain references to the netdevice and subsequently the queue.
    * \param traffic To obtain simulation time.
    *
    */
  void Install (Ptr<Node> node, Ptr<TrafficParameters> traffic);

private:
  uint32_t                    m_bytesOut;		//!< Number of bytes sent per second
  uint32_t                    m_bandwidth;		//!< Bandwidth of bottleneck link in Mbps
  uint32_t                    m_sumQueueLength;		//!< Sum of sampled queue lengths
  uint32_t                    m_nthSampleInInterval;	//!< Number of samples for queue lengths
  uint32_t                    m_numFtpFlows;		//!< Number of forward FTP Flows
  double                      m_totalUtilization;	//!< Running sum of all utilization values
  double                      m_totalQueueSize;		//!< Running sum of queue size
  double                      m_totalDroppedPacketRate; //!< Running sum of dropped packet rates
  std::string                 m_bottleneckQueue;	//!< Type of bottleneck Queue
  Time                        m_simulationTime;		//!< Simulation time in seconds
  Time                        m_rttp;			//!< RTT value for the simulation
  Ptr<PointToPointNetDevice>  m_netDevice;		//!< The netdevice from which stats are collected
  Ptr<Queue>                  m_queue;			//!< The queue of the node from which stats are collected
  std::string                 m_evalStatsFileName;	//!< Name of file where the output is stored
  std::ofstream               m_evalStatsFile;		//!< The file for storing the output
};

}

#endif /*STATS_TCP_H*/
