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

// Implement an object to create traffic in tcp-eval.

#include <iostream>

#include "create-traffic.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CreateTraffic");

NS_OBJECT_ENSURE_REGISTERED (CreateTraffic);

TypeId
CreateTraffic::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CreateTraffic")
    .SetParent<Object> ()
    .SetGroupName ("TcpEvaluationSuite")
  ;
  return tid;
}

CreateTraffic::CreateTraffic (void)
{
  m_randVar = CreateObject<UniformRandomVariable> ();
  m_randVar->SetAttribute ("Min", DoubleValue (0.001));
  m_randVar->SetAttribute ("Max", DoubleValue (0.3));
}

CreateTraffic::~CreateTraffic (void)
{
}

double
CreateTraffic::GetRandomValue (void) const
{
  return m_randVar->GetValue ();
}

void
CreateTraffic::CreateFwdFtpTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows,
                                    uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50000;

  // offset is used to identify the starting node among left leaf nodes
  // that generate forward FTP flows. Iterate through the leaf nodes from
  // offset till numberOfFwdFtpFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; i++)
    {
      // Install bulk send application on left side nodes.
      // i'th left node acts as a source and i'th right node acts as a sink.
      AddressValue remoteAddress (InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));

      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("MaxBytes", UintegerValue (0));

      // Both source and sink apps are added to a single ApplicationContainer.
      // This is done to avoid the case when source starts before sink or
      // vice-versa due to random time generation.
      ApplicationContainer sourceApp = ftp.Install (dumbbell.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));
      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      ApplicationContainer sinkApp = sinkHelper.Install (dumbbell.GetRight (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateRevFtpTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows,
                                    uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50001;

  // offset is used to identify the starting node among right leaf nodes
  // that generate reverse FTP flows. Iterate through the leaf nodes from
  // offset till numberOfRevFtpFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; i++)
    {
      // Install bulk send application on right side nodes.
      // i'th right node acts as a source and i'th left node acts as a sink.
      AddressValue remoteAddress (InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port1));

      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("MaxBytes", UintegerValue (0));

      // Both source and sink apps are added to a single ApplicationContainer.
      // This is done to avoid the case when source starts before sink or
      // vice-versa due to random time generation.
      ApplicationContainer sourceApp = ftp.Install (dumbbell.GetRight (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port1));
      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      ApplicationContainer sinkApp = sinkHelper.Install (dumbbell.GetLeft (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateVoiceTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows,
                                   uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50002;
  uint32_t port2 = 50003;

  // offset is used to identify the starting node among the leaf nodes
  // that generate two-way voice traffic. Iterate through the leaf nodes from
  // offset till numberOfVoiceFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      OnOffHelper voiceFwd ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));
      voiceFwd.SetAttribute ("PacketSize",UintegerValue (172));
      voiceFwd.SetAttribute ("DataRate", DataRateValue (DataRate ("64kb/s")));
      voiceFwd.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.35]"));

      ApplicationContainer sourceAppFwd = voiceFwd.Install (dumbbell.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceAppFwd.Start (Seconds (startTime));
      sourceAppFwd.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSinkFwd ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));
      ApplicationContainer sinkAppFwd = packetSinkFwd.Install (dumbbell.GetRight (i));
      sinkAppFwd.Start (Seconds (startTime));
      sinkAppFwd.Stop (Seconds (stopTime));

      OnOffHelper voiceRev ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port2));
      voiceRev.SetAttribute ("PacketSize",UintegerValue (172));
      voiceRev.SetAttribute ("DataRate", DataRateValue (DataRate ("64kb/s")));
      voiceRev.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.35]"));

      ApplicationContainer sourceAppRev = voiceRev.Install (dumbbell.GetRight (i));
      sourceAppRev.Start (Seconds (startTime));
      sourceAppRev.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSinkRev ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port2));
      ApplicationContainer sinkAppRev = packetSinkRev.Install (dumbbell.GetLeft (i));
      sinkAppRev.Start (Seconds (startTime));
      sinkAppRev.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateFwdStreamingTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows,
                                          uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50004;

  // offset is used to identify the starting node among left leaf nodes
  // that generate forward streaming flows. Iterate through the leaf nodes from
  // offset till numberOfFwdStreamingFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      std::string streamingDataRate = to_string<double> (traffic->GetStreamingRate ()) + std::string ("Kbps");
      OnOffHelper streaming ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));
      streaming.SetAttribute ("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=10]"));
      streaming.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      streaming.SetAttribute ("PacketSize",UintegerValue (traffic->GetStreamingPacketSize ()));
      streaming.SetAttribute ("DataRate", DataRateValue (DataRate (streamingDataRate)));

      ApplicationContainer sourceApp = streaming.Install (dumbbell.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSink ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (i), port1));
      ApplicationContainer sinkApp = packetSink.Install (dumbbell.GetRight (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateRevStreamingTraffic (PointToPointDumbbellHelper dumbbell, uint32_t flows,
                                          uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50005;

  // offset is used to identify the starting node among right leaf nodes
  // that generate reverse streaming flows. Iterate through the leaf nodes from
  // offset till numberOfRevStreamingFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      std::string streamingDataRate = to_string<double> (traffic->GetStreamingRate ()) + std::string ("Kbps");
      OnOffHelper streaming ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port1));
      streaming.SetAttribute ("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=10]"));
      streaming.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      streaming.SetAttribute ("PacketSize",UintegerValue (traffic->GetStreamingPacketSize ()));
      streaming.SetAttribute ("DataRate", DataRateValue (DataRate (streamingDataRate)));

      ApplicationContainer sourceApp = streaming.Install (dumbbell.GetRight (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSink ("ns3::UdpSocketFactory",InetSocketAddress (dumbbell.GetLeftIpv4Address (i), port1));
      ApplicationContainer sinkApp = packetSink.Install (dumbbell.GetLeft (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateFwdFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows,
                                           uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50000;

  // offset is used to identify the starting node among left leaf nodes
  // that generate forward FTP flows. Iterate through the leaf nodes from
  // offset till numberOfFwdFtpFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      // Install bulk send application on left side nodes.
      // i'th left node acts as a source and i'th right node acts as a sink.
      AddressValue remoteAddress (InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));

      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("MaxBytes", UintegerValue (0));

      // Both source and sink apps are added to a single ApplicationContainer.
      // This is done to avoid the case when source starts before sink or
      // vice-versa due to random time generation.
      ApplicationContainer sourceApp = ftp.Install (parkingLot.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));
      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      ApplicationContainer sinkApp = sinkHelper.Install (parkingLot.GetRight (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateRevFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows,
                                           uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50001;

  // offset is used to identify the starting node among right leaf nodes
  // that generate reverse FTP flows. Iterate through the leaf nodes from
  // offset till numberOfRevFtpFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      AddressValue remoteAddress (InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port1));

      // Install bulk send application on right side nodes.
      // i'th right node acts as a source and i'th left node acts as a sink.
      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("MaxBytes", UintegerValue (0));

      // Both source and sink apps are added to a single ApplicationContainer.
      // This is done to avoid the case when source starts before sink or
      // vice-versa due to random time generation.
      ApplicationContainer sourceApp = ftp.Install (parkingLot.GetRight (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port1));
      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      ApplicationContainer sinkApp = sinkHelper.Install (parkingLot.GetLeft (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateVoiceTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows,
                                          uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50002;
  uint32_t port2 = 50003;

  // offset is used to identify the starting node among the leaf nodes
  // that generate two-way voice traffic. Iterate through the leaf nodes from
  // offset till numberOfVoiceFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      OnOffHelper voiceFwd ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));
      voiceFwd.SetAttribute ("PacketSize",UintegerValue (172));
      voiceFwd.SetAttribute ("DataRate", DataRateValue (DataRate ("64kb/s")));
      voiceFwd.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.35]"));

      ApplicationContainer sourceAppFwd = voiceFwd.Install (parkingLot.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceAppFwd.Start (Seconds (startTime));
      sourceAppFwd.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSinkFwd ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));
      ApplicationContainer sinkAppFwd = packetSinkFwd.Install (parkingLot.GetRight (i));
      sinkAppFwd.Start (Seconds (startTime));
      sinkAppFwd.Stop (Seconds (stopTime));

      OnOffHelper voiceRev ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port2));
      voiceRev.SetAttribute ("PacketSize",UintegerValue (172));
      voiceRev.SetAttribute ("DataRate", DataRateValue (DataRate ("64kb/s")));
      voiceRev.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.35]"));

      ApplicationContainer sourceAppRev = voiceRev.Install (parkingLot.GetRight (i));
      sourceAppRev.Start (Seconds (startTime));
      sourceAppRev.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSinkRev ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port2));
      ApplicationContainer sinkAppRev = packetSinkRev.Install (parkingLot.GetLeft (i));
      sinkAppRev.Start (Seconds (startTime));
      sinkAppRev.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateFwdStreamingTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows,
                                                 uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50004;

  // offset is used to identify the starting node among left leaf nodes
  // that generate forward streaming flows. Iterate through the leaf nodes from
  // offset till numberOfFwdStreamingFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      std::string streamingDataRate = to_string<double> (traffic->GetStreamingRate ()) + std::string ("Kbps");
      OnOffHelper streaming ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));
      streaming.SetAttribute ("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=10]"));
      streaming.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      streaming.SetAttribute ("PacketSize",UintegerValue (traffic->GetStreamingPacketSize ()));
      streaming.SetAttribute ("DataRate", DataRateValue (DataRate (streamingDataRate)));

      ApplicationContainer sourceApp = streaming.Install (parkingLot.GetLeft (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSink ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetRightIpv4Address (i), port1));
      ApplicationContainer sinkApp = packetSink.Install (parkingLot.GetRight (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateRevStreamingTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t flows,
                                                 uint32_t offset, Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50005;

  // offset is used to identify the starting node among right leaf nodes
  // that generate reverse streaming flows. Iterate through the leaf nodes from
  // offset till numberOfRevStreamingFlow nodes are traversed.
  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = offset; i < flows + offset; ++i)
    {
      std::string streamingDataRate = to_string<double> (traffic->GetStreamingRate ()) + std::string ("Kbps");
      OnOffHelper streaming ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port1));
      streaming.SetAttribute ("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=10]"));
      streaming.SetAttribute ("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      streaming.SetAttribute ("PacketSize",UintegerValue (traffic->GetStreamingPacketSize ()));
      streaming.SetAttribute ("DataRate", DataRateValue (DataRate (streamingDataRate)));

      ApplicationContainer sourceApp = streaming.Install (parkingLot.GetRight (i));
      double startTime = GetRandomValue ();
      sourceApp.Start (Seconds (startTime));
      sourceApp.Stop (Seconds (stopTime - 3));

      PacketSinkHelper packetSink ("ns3::UdpSocketFactory",InetSocketAddress (parkingLot.GetLeftIpv4Address (i), port1));
      ApplicationContainer sinkApp = packetSink.Install (parkingLot.GetLeft (i));
      sinkApp.Start (Seconds (startTime));
      sinkApp.Stop (Seconds (stopTime));
    }
}

void
CreateTraffic::CreateCrossFtpTrafficParking (PointToPointParkingLotHelper parkingLot, uint32_t crossFlows,
                                             Ptr<TrafficParameters> traffic)
{
  uint32_t port1 = 50006;
  uint32_t range = parkingLot.RouterCount () - 1;

  double stopTime = traffic->GetSimulationTime ().GetSeconds ();
  for (uint32_t i = 0; i < range; ++i)
    {
      for (uint32_t j = 0; j < crossFlows; ++j)
        {
          // it will be (i, j) and not (i + 1, j) because indexing of both cross source and sink starts from 0
          AddressValue remoteAddress (InetSocketAddress (parkingLot.GetCrossSinkIpv4Address (i, j), port1));
          BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
          ftp.SetAttribute ("Remote", remoteAddress);
          ftp.SetAttribute ("MaxBytes", UintegerValue (0));

          ApplicationContainer sourceApp = ftp.Install (parkingLot.GetCrossSource (i,j));
          double startTime = GetRandomValue ();
          sourceApp.Start (Seconds (startTime));
          sourceApp.Stop (Seconds (stopTime - 3));

          PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (parkingLot.GetCrossSinkIpv4Address (i, j), port1));
          sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
          ApplicationContainer sinkApp = sinkHelper.Install (parkingLot.GetCrossSink (i,j));
          sinkApp.Start (Seconds (startTime));
          sinkApp.Stop (Seconds (stopTime - 3));
        }
    }
}

template <typename T>
std::string CreateTraffic::to_string (const T& data)
{
  std::ostringstream conv;
  conv << data;
  return conv.str ();
}
}
