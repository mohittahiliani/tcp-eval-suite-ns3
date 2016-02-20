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

// This example is a part of TCL evaluation suite and 
// creates a parking-lot scenario.

#include "ns3/core-module.h"
#include "ns3/configure-topology.h"
#include "ns3/traffic-parameters.h"
#include "ns3/parking-lot-topology.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpEvalParkingLotExample");

int
main (int argc, char *argv[])
{
  // Set default values for topology
  double        bottleneckBandwidth = 10;
  double        rtt = 0.08;
  double        rttDiff = 0.0;
  Time          rttp;
  Time          rttDifference;

  // Set default values for traffic
  uint32_t      nFwdFtpFlows = 5;
  uint32_t      nRevFtpFlows = 5;
  uint32_t      nCrossFtpFlows = 5;
  uint32_t      nVoiceFlows = 5;
  uint32_t      nFwdStreamingFlows = 5;
  uint32_t      nRevStreamingFlows = 5;
  double        streamingRate = 640;
  double        simTime = 100;
  double        crsLinkDelay = 0.01;
  uint32_t      streamingPacketSize = 840;
  bool          useAqm = false;
  Time          crossLinkDelay;
  Time          simulationTime;

  // Set default TCP variant
  std::string tcp_variant = "TcpNewReno";

  // Default filename to store results
  std::string fileName = "TcpEvalParkingLot";

  // Allow the user to change values by command line arguments
  CommandLine cmd;
  cmd.AddValue ("bottleneckBandwidth", "Bandwidth of bottleneck link in Mbps", bottleneckBandwidth);
  cmd.AddValue ("rttp", "Round trip propagation delay in seconds", rtt);
  cmd.AddValue ("rttDifference", "Flow RTT difference in seconds", rttDiff);
  cmd.AddValue ("nFwdFtpFlows", "Number of FTP flows on forward path", nFwdFtpFlows);
  cmd.AddValue ("nRevFtpFlows", "Number of FTP flows on reverse path", nRevFtpFlows);
  cmd.AddValue ("nCrossFtpFlows", "Number of cross FTP flows", nCrossFtpFlows);
  cmd.AddValue ("nVoiceFlows", "Number of two-way voice flows", nVoiceFlows);
  cmd.AddValue ("nFwdStreamingFlows", "Number of streaming flows on forward path", nFwdStreamingFlows);
  cmd.AddValue ("nRevStreamingFlows", "Number of streaming flows on reverse path", nRevStreamingFlows);
  cmd.AddValue ("streamingRate", "Bit rate of streaming flows in Kbps", streamingRate);
  cmd.AddValue ("streamingPacketSize", "Packet size of streaming flows in bytes", streamingPacketSize);
  cmd.AddValue ("useAqm", "Enable or disable AQM in routers", useAqm);
  cmd.AddValue ("crossLinkDelay", "Cross link delay in seconds", crsLinkDelay);
  cmd.AddValue ("simulationTime", "Total simulation time in seconds", simTime);
  cmd.AddValue ("tcp_variant", "Change the TCP variant", tcp_variant);
  cmd.AddValue ("fileName", "File to store the results", fileName);
  cmd.Parse (argc, argv);

  // Convert time from double to seconds
  rttp = Time::FromDouble (rtt, Time::S);
  rttDifference = Time::FromDouble (rttDiff, Time::S);
  simulationTime = Time::FromDouble (simTime, Time::S);
  crossLinkDelay = Time::FromDouble (crsLinkDelay, Time::S);

  // Set topology parameters
  Config::SetDefault ("ns3::ConfigureTopology::BottleneckBandwidth", DoubleValue (bottleneckBandwidth));
  Config::SetDefault ("ns3::ConfigureTopology::RTTP", TimeValue (rttp));
  Config::SetDefault ("ns3::ConfigureTopology::RttDiff", TimeValue (rttDifference));

  // Set traffic parameters
  Config::SetDefault ("ns3::TrafficParameters::FwdFtpFlows", UintegerValue (nFwdFtpFlows));
  Config::SetDefault ("ns3::TrafficParameters::RevFtpFlows", UintegerValue (nRevFtpFlows));
  Config::SetDefault ("ns3::TrafficParameters::CrossFtpFlows", UintegerValue (nCrossFtpFlows));
  Config::SetDefault ("ns3::TrafficParameters::NumOfVoiceFlows", UintegerValue (nVoiceFlows));
  Config::SetDefault ("ns3::TrafficParameters::FwdStreamingFlows", UintegerValue (nFwdStreamingFlows));
  Config::SetDefault ("ns3::TrafficParameters::RevStreamingFlows", UintegerValue (nRevStreamingFlows));
  Config::SetDefault ("ns3::TrafficParameters::StreamingRate", DoubleValue (streamingRate));
  Config::SetDefault ("ns3::TrafficParameters::StreamingPacketSize", UintegerValue (streamingPacketSize));
  Config::SetDefault ("ns3::TrafficParameters::UseAqm", BooleanValue (useAqm));
  Config::SetDefault ("ns3::TrafficParameters::SimulationTime", TimeValue (simulationTime));
  Config::SetDefault ("ns3::ParkingLotTopology::CrossLinkDelay", TimeValue (crossLinkDelay));

  // Set TCP variant
  if (tcp_variant.compare ("TcpTahoe") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpTahoe::GetTypeId ()));
    }
  else if (tcp_variant.compare ("TcpReno") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpReno::GetTypeId ()));
    }
  else if (tcp_variant.compare ("TcpNewReno") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId ()));
    }
  else if (tcp_variant.compare ("TcpWestwood") == 0)
    { // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
  else if (tcp_variant.compare ("TcpWestwoodPlus") == 0)
    {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
    }
  else
    {
      NS_LOG_DEBUG ("Invalid TCP version");
      exit (1);
    }

  Ptr<TrafficParameters> trafficParams = CreateObject <TrafficParameters> ();
  Ptr<ParkingLotTopology> parkingLot = CreateObject<ParkingLotTopology> ();
  parkingLot->CreateParkingLotTopology (trafficParams, fileName);

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
