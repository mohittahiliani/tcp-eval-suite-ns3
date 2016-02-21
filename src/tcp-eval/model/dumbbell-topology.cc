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

// Implement an object to create dumbbell topology in tcp-eval.

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include "dumbbell-topology.h"
#include "eval-stats.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DumbbellTopology");

NS_OBJECT_ENSURE_REGISTERED (DumbbellTopology);

TypeId
DumbbellTopology::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DumbbellTopology")
    .SetParent<ConfigureTopology> ()
    .SetGroupName ("TcpEvaluationSuite")
  ;
  return tid;
}

DumbbellTopology::DumbbellTopology (void)
{
}

DumbbellTopology::~DumbbellTopology (void)
{
}

void
DumbbellTopology::CreateDumbbellTopology (Ptr<TrafficParameters> traffic, std::string fileName)
{
  uint32_t nBottlenecks = 1;

  // Set default parameters for topology
  SetTopologyParameters (traffic, nBottlenecks);

  PointToPointHelper pointToPointRouter, pointToPointLeaf;
  pointToPointRouter.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_bottleneckBandwidth) + std::string ("Mbps")));
  pointToPointRouter.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_bottleneckDelay.ToDouble (Time::S)) + std::string ("s")));

  pointToPointLeaf.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_nonBottleneckBandwidth) + std::string ("Mbps")));
  pointToPointLeaf.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_nonBottleneckDelay.ToDouble (Time::S)) + std::string ("s")));
  pointToPointLeaf.SetQueue ("ns3::DropTailQueue",
                             "Mode", StringValue ("QUEUE_MODE_PACKETS"),
                             "MaxPackets", UintegerValue (m_nonBottleneckBuffer));

  // If AQM is used, install RED queue at the bottleneck link
  // else install DropTail queue
  if (traffic->IsAqmUsed () == true)
    {
      SetRedParameters ();
      pointToPointRouter.SetQueue ("ns3::RedQueue",
                                   "LinkBandwidth", DataRateValue (DataRate (to_string<double> (m_bottleneckBandwidth) + std::string ("Mbps"))),
                                   "LinkDelay", TimeValue (m_bottleneckDelay),
                                   "QueueLimit", UintegerValue (m_bottleneckBuffer));
    }
  else
    {
      pointToPointRouter.SetQueue ("ns3::DropTailQueue",
                                   "Mode", StringValue ("QUEUE_MODE_PACKETS"),
                                   "MaxPackets", UintegerValue (m_bottleneckBuffer));
    }

  uint32_t nFwdFtpFlow = traffic->GetNumOfFwdFtpFlows ();
  uint32_t nRevFtpFlow = traffic->GetNumOfRevFtpFlows ();
  uint32_t nVoiceFlow = traffic->GetNumOfVoiceFlows ();
  uint32_t nFwdStreamingFlow = traffic->GetNumOfFwdStreamingFlows ();
  uint32_t nRevStreamingFlow = traffic->GetNumOfRevStreamingFlows ();

  // Calculate total leaf nodes at each side
  uint32_t nLeftLeaf = nFwdFtpFlow + nRevFtpFlow + nVoiceFlow + nFwdStreamingFlow + nRevStreamingFlow;
  uint32_t nRightLeaf = nLeftLeaf;

  PointToPointDumbbellHelper dumbbell (nLeftLeaf, pointToPointLeaf,
                                       nRightLeaf, pointToPointLeaf,
                                       pointToPointRouter);

  // Install Stack
  InternetStackHelper stack;
  dumbbell.InstallStack (stack);

  // Assign IP Addresses
  dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.10.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.100.1.0", "255.255.255.0"));

  // offset helps in iterating over the topology by keeping track of
  // the nodes created for a particular traffic
  uint32_t offset = 0;
  Ptr<CreateTraffic> createTraffic = CreateObject<CreateTraffic> ();
  if (nFwdFtpFlow > 0)
    {
      // Create forward FTP traffic
      createTraffic->CreateFwdFtpTraffic (dumbbell, nFwdFtpFlow, offset, traffic);
      offset += nFwdFtpFlow;
    }

  if (nRevFtpFlow > 0)
    {
      // Create reverse FTP traffic
      createTraffic->CreateRevFtpTraffic (dumbbell, nRevFtpFlow, offset, traffic);
      offset += nRevFtpFlow;
    }
  if (nVoiceFlow > 0)
    {
      // Create voice traffic
      createTraffic->CreateVoiceTraffic (dumbbell, nVoiceFlow, offset, traffic);
      offset += nVoiceFlow;
    }
  if (nFwdStreamingFlow > 0)
    {
      // Create forward streaming traffic
      createTraffic->CreateFwdStreamingTraffic (dumbbell, nFwdStreamingFlow, offset, traffic);
      offset += nFwdStreamingFlow;
    }
  if (nRevStreamingFlow > 0)
    {
      // Create reverse streaming traffic
      createTraffic->CreateRevStreamingTraffic (dumbbell, nRevStreamingFlow, offset, traffic);
      offset += nRevStreamingFlow;
    }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Push the stats of left most router to a file
  Ptr<Node> left = dumbbell.GetLeft ();
  Ptr<EvalStats> evalStats = CreateObject<EvalStats> (m_bottleneckBandwidth, m_rttp , fileName);
  evalStats->Install (left, traffic);

  Simulator::Stop (Time::FromDouble (((traffic->GetSimulationTime ()).ToDouble (Time::S) + 5), Time::S));
  Simulator::Run ();
  Simulator::Destroy ();
}

template <typename T>
std::string DumbbellTopology::to_string (const T& data)
{
  std::ostringstream conv;
  conv << data;
  return conv.str ();
}

}
