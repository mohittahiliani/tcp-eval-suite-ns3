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

// Implement an object that collects statistics at the bottleneck router.

#include "eval-stats.h"

namespace ns3 {

EvalStats::EvalStats (double bandwidth, Time rttp, std::string fileName)
{
  m_totalUtilization = 0;
  m_totalQueueSize = 0;
  m_totalDroppedPacketRate = 0;
  m_evalStatsFileName.assign (fileName);
  m_bytesOut = 0;
  m_sumQueueLength = 0;
  m_nthSampleInInterval = 0;
  this->m_bandwidth = bandwidth;
  this->m_rttp = rttp;
}

EvalStats::~EvalStats ()
{
  InsertIntoFile ();
  m_evalStatsFile.close ();
}

// Computes link utilization and mean queue size
void
EvalStats::ComputeMetrics ()
{
  m_totalUtilization += (double) m_bytesOut * 8.0 / ( m_bandwidth * 1024 * 1024 );
  m_totalQueueSize += ((m_nthSampleInInterval == 0) ? 0 : ((double) (m_sumQueueLength / m_nthSampleInInterval)));
  m_sumQueueLength = 0;
  m_nthSampleInInterval = 0;
  m_bytesOut = 0;
  Simulator::Schedule (Seconds (1.0), &EvalStats::ComputeMetrics, this);
}

// Inserts the values computed in the ComputeMetrics into the file
void
EvalStats::InsertIntoFile ()
{
  uint32_t tcDroppedPackets = m_queueDisc->GetTotalDroppedPackets ();
  uint32_t netDeviceDroppedPackets = m_queue->GetTotalDroppedPackets ();
  uint32_t tcRequeuedPackets = m_queueDisc->GetTotalRequeuedPackets ();
  uint32_t actualDroppedPackets = tcDroppedPackets + netDeviceDroppedPackets - tcRequeuedPackets;
  uint32_t totalPackets = m_queueDisc->GetTotalReceivedPackets ();

  m_totalDroppedPacketRate = ((actualDroppedPackets == 0) ? 0 : (100.0 * (double) actualDroppedPackets / (double) totalPackets));

  m_evalStatsFile.open (m_evalStatsFileName.c_str (), std::ios::app);
  m_evalStatsFile << m_bandwidth << std::setw (15) << m_rttp.GetSeconds () << std::setw (15) << m_numFtpFlows;
  m_evalStatsFile << std::setw (15) << (m_totalUtilization / (m_simulationTime.ToDouble (Time::S)) * 100);
  m_evalStatsFile << std::setw (15) << m_totalQueueSize / (m_simulationTime.ToDouble (Time::S));
  m_evalStatsFile << std::setw (15) << m_totalDroppedPacketRate;

  m_evalStatsFile << std::endl;
}

// Called during the PhyTxBegin event at the netdevice.
// Gets the size of the packet and stores it in bytesOut variable.
void
EvalStats::AggregateOverInterval (Ptr<const Packet> packet)
{
  m_bytesOut += packet->GetSize ();
}

// Called during the Enqueue event at the queue.
// Gets the number of packets in the queue and the number of times it is sampled.
void
EvalStats::AggregateQueue ()
{
  m_sumQueueLength += m_queue->GetNPackets ();
  m_nthSampleInInterval++;
  Simulator::Schedule (Seconds (0.01), &EvalStats::AggregateQueue, this);
}

// It takes node as input and gets references to netdevice and queue in that node.
// It then connects the trace sources PhyTxBegin and Enqueue to the netdevice and queue respectively
// and makes callbacks to the methods AggregateOverInterval and AggregateQueue.
void
EvalStats::Install (Ptr<Node> node, Ptr<TrafficParameters> traffic)
{
  m_simulationTime = traffic->GetSimulationTime ();
  m_numFtpFlows = traffic->GetNumOfFwdFtpFlows ();

  Ptr<NetDevice> netDevice = node->GetDevice (0);
  m_netDeviceP2p = DynamicCast<PointToPointNetDevice> (netDevice);

  m_queue = m_netDeviceP2p->GetQueue ();
  Ptr<TrafficControlLayer> tc = node->GetObject<TrafficControlLayer> ();
  m_queueDisc = tc->GetRootQueueDiscOnDevice (netDevice);

  m_netDeviceP2p->TraceConnectWithoutContext ("PhyTxBegin", MakeCallback (&EvalStats::AggregateOverInterval, this));

  Simulator::Schedule (Seconds (0.01), &EvalStats::AggregateQueue, this);
  Simulator::Schedule (Seconds (1.0), &EvalStats::ComputeMetrics, this);
}
}
