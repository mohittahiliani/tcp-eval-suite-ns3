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

// Implement an object to create parking-lot topology in tcp-eval.

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include "parking-lot-topology.h"
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

NS_LOG_COMPONENT_DEFINE ("ParkingLotTopology");

NS_OBJECT_ENSURE_REGISTERED (ParkingLotTopology);

TypeId
ParkingLotTopology::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ParkingLotTopology")
    .SetParent<ConfigureTopology> ()
    .SetGroupName ("TcpEvaluationSuite")
    .AddAttribute ("CrossLinkDelay",
                   "Delay of cross links in seconds",
                   TimeValue (Seconds (10)),
                   MakeTimeAccessor (&ParkingLotTopology::m_crossLinkDelay),
                   MakeTimeChecker ())
  ;
  return tid;
}

ParkingLotTopology::ParkingLotTopology (void)
{
}

ParkingLotTopology::~ParkingLotTopology (void)
{
}

void
ParkingLotTopology::SetCrossLinkDelay (Time crossLinkDelay)
{
  m_crossLinkDelay = crossLinkDelay;
}

Time
ParkingLotTopology::GetCrossLinkDelay (void) const
{
  return m_crossLinkDelay;
}

void
ParkingLotTopology::CreateParkingLotTopology (Ptr<TrafficParameters> trafficParams, std::string fileName)
{
  // Set default parameters for topology
  SetTopologyParameters (trafficParams, BottleneckCount ());

  PointToPointHelper pointToPointRouter, pointToPointLeaf, pointToPointCrossLinks;
  pointToPointRouter.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_bottleneckBandwidth) + std::string ("Mbps")));
  pointToPointRouter.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_bottleneckDelay.ToDouble (Time::S)) + std::string ("s")));

  pointToPointLeaf.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_nonBottleneckBandwidth) + std::string ("Mbps")));
  pointToPointLeaf.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_nonBottleneckDelay.ToDouble (Time::S)) + std::string ("s")));
  pointToPointLeaf.SetQueue ("ns3::DropTailQueue",
                             "Mode", StringValue ("QUEUE_MODE_PACKETS"),
                             "MaxPackets", UintegerValue (m_nonBottleneckBuffer));

  pointToPointCrossLinks.SetDeviceAttribute  ("DataRate", StringValue (to_string<double> (m_nonBottleneckBandwidth) + std::string ("Mbps")));
  pointToPointCrossLinks.SetChannelAttribute ("Delay", StringValue (to_string<double> (m_crossLinkDelay.ToDouble (Time::S)) + std::string ("s")));
  pointToPointCrossLinks.SetQueue ("ns3::DropTailQueue",
                                   "Mode", StringValue ("QUEUE_MODE_PACKETS"),
                                   "MaxPackets", UintegerValue (m_nonBottleneckBuffer));

  // If AQM is used, install RED queue at the bottleneck link
  // else install DropTail queue
  if (trafficParams->IsAqmUsed () == true)
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

  uint32_t nFwdFtpFlow = trafficParams->GetNumOfFwdFtpFlows ();
  uint32_t nRevFtpFlow = trafficParams->GetNumOfRevFtpFlows ();
  uint32_t nVoiceFlow = trafficParams->GetNumOfVoiceFlows ();
  uint32_t nFwdStreamingFlow = trafficParams->GetNumOfFwdStreamingFlows ();
  uint32_t nRevStreamingFlow = trafficParams->GetNumOfRevStreamingFlows ();

  // Calculate total leaf nodes at each side
  uint32_t nLeftLeaf = nFwdFtpFlow + nRevFtpFlow + nVoiceFlow + nFwdStreamingFlow + nRevStreamingFlow;
  uint32_t nRightLeaf = nLeftLeaf;

  uint32_t nCrossFtpFlow = trafficParams->GetNumOfCrossFtpFlows ();

  PointToPointParkingLotHelper parkingLot (nLeftLeaf, pointToPointLeaf,
                                           nRightLeaf, pointToPointLeaf,
                                           BottleneckCount (), nCrossFtpFlow,
                                           pointToPointCrossLinks, pointToPointRouter);

  // Install Stack
  InternetStackHelper stack;
  parkingLot.InstallStack (stack);

  // Assign IP Addresses
  parkingLot.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                  Ipv4AddressHelper ("10.10.1.0", "255.255.255.0"),
                                  Ipv4AddressHelper ("10.50.1.0", "255.255.255.0"),
                                  Ipv4AddressHelper ("10.100.1.0", "255.255.255.0"),
                                  Ipv4AddressHelper ("10.150.1.0", "255.255.255.0"));

  // offset helps in iterating over the topology by keeping track of
  // the nodes created for a particular traffic
  uint32_t offset = 0;
  Ptr<CreateTraffic> createTraffic = CreateObject<CreateTraffic> ();

  if (nFwdFtpFlow > 0)
    {
      // Create forward FTP traffic
      createTraffic->CreateFwdFtpTrafficParking (parkingLot, nFwdFtpFlow, offset, trafficParams);
      offset += nFwdFtpFlow;
    }

  if (nRevFtpFlow > 0)
    {
      // Create reverse FTP traffic
      createTraffic->CreateRevFtpTrafficParking (parkingLot, nRevFtpFlow, offset, trafficParams);
      offset += nRevFtpFlow;
    }

  if (trafficParams->GetNumOfCrossFtpFlows () > 0)
    {
      // Create cross FTP traffic
      createTraffic->CreateCrossFtpTrafficParking (parkingLot, nCrossFtpFlow, trafficParams);
    }

  if (nVoiceFlow > 0)
    {
      // Create voice traffic
      createTraffic->CreateVoiceTrafficParking (parkingLot, nVoiceFlow, offset, trafficParams);
      offset += nVoiceFlow;
    }
  if (nFwdStreamingFlow > 0)
    {
      // Create forward streaming traffic
      createTraffic->CreateFwdStreamingTrafficParking (parkingLot, nFwdStreamingFlow, offset, trafficParams);
      offset += nFwdStreamingFlow;
    }
  if (nRevStreamingFlow > 0)
    {
      // Create reverse streaming traffic
      createTraffic->CreateRevStreamingTrafficParking (parkingLot, nRevStreamingFlow, offset, trafficParams);
      offset += nRevStreamingFlow;
    }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Push the stats of left most router to a file
  Ptr<Node> left = parkingLot.GetRouter (0);
  Ptr<EvalStats> evalStats = CreateObject<EvalStats> (m_bottleneckBandwidth, m_rttp , fileName);
  evalStats->Install (left, trafficParams);

  Simulator::Stop (Time::FromDouble (((trafficParams->GetSimulationTime ()).ToDouble (Time::S) + 5), Time::S));
  Simulator::Run ();
  Simulator::Destroy ();
}

template <typename T>
std::string ParkingLotTopology::to_string (const T& data)
{
  std::ostringstream conv;
  conv << data;
  return conv.str ();
}

}
