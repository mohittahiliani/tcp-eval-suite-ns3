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

EvalStats::EvalStats (uint32_t bandwidth, Time rttp, std::string fileName)
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
  m_totalUtilization += (double) m_bytesOut * 8.0 / ( m_bandwidth * 1000 * 1000 );
  m_bytesOut = 0;

  UintegerValue queueSize;
  if (m_bottleneckQueue.compare ("RED") == 0)
    {
      m_queue->GetAttribute ("QueueLimit", queueSize);
    }
  else
    {
      m_queue->GetAttribute ("MaxPackets", queueSize);
    }

  m_totalQueueSize += ((m_nthSampleInInterval == 0) ? 0 : ((double) (m_sumQueueLength / m_nthSampleInInterval) / queueSize.Get () * 100));
  m_sumQueueLength = 0;
  m_nthSampleInInterval = 0;
}

// Inserts the values computed in the ComputeMetrics into the file
void
EvalStats::InsertIntoFile ()
{
  uint32_t totalPackets = m_queue->GetTotalReceivedPackets ();
  uint32_t droppedPackets = m_queue->GetTotalDroppedPackets ();
  m_totalDroppedPacketRate = ((droppedPackets == 0) ? 0 : (100.0 * (double) droppedPackets / (double) totalPackets));

  m_evalStatsFile.open (m_evalStatsFileName.c_str (), std::ios::app);
  m_evalStatsFile << m_bandwidth << std::setw (15) << m_rttp.GetSeconds () << std::setw (15) << m_numFtpFlows;
  m_evalStatsFile << std::setw (15) << (m_totalUtilization / (m_simulationTime.ToDouble (Time::S)) * 100);

  UintegerValue queueSize;
  if (m_bottleneckQueue.compare ("RED") == 0)
    {
      m_queue->GetAttribute ("QueueLimit", queueSize);
    }
  else
    {
      m_queue->GetAttribute ("MaxPackets", queueSize);
    }
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
EvalStats::AggregateQueue (Ptr<const Packet> packet)
{
  m_sumQueueLength += m_queue->GetNPackets ();
  m_nthSampleInInterval++;
}


// It takes node as input and gets references to netdevice and queue in that node.
// It then connects the trace sources PhyTxBegin and Enqueue to the netdevice and queue respectively
// and makes callbacks to the methods AggregateOverInterval and AggregateQueue.
void
EvalStats::Install (Ptr<Node> node, Ptr<TrafficParameters> traffic)
{
  m_simulationTime = traffic->GetSimulationTime ();
  m_numFtpFlows = traffic->GetNumOfFwdFtpFlows ();
  if (traffic->IsAqmUsed () == true)
    {
      m_bottleneckQueue = "RED";
    }
  else
    {
      m_bottleneckQueue = "DropTail";
    }
  m_netDevice = node->GetDevice (0)->GetObject<PointToPointNetDevice> ();
  m_queue = m_netDevice->GetQueue ();

  m_netDevice->TraceConnectWithoutContext ("PhyTxBegin", MakeCallback (&EvalStats::AggregateOverInterval, this));
  m_queue->TraceConnectWithoutContext ("Enqueue", MakeCallback (&EvalStats::AggregateQueue, this));

  for (uint32_t i = 1; i <= m_simulationTime.ToInteger (Time::S); ++i)
    {
      Simulator::Schedule (Seconds (i), &EvalStats::ComputeMetrics, this);
    }
}
}
