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

// Implement an object to set traffic parameters in tcp-eval.

#include <iostream>

#include "traffic-parameters.h"
#include "ns3/log.h"
#include "ns3/core-module.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TrafficParameters");

NS_OBJECT_ENSURE_REGISTERED (TrafficParameters);

TypeId
TrafficParameters::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TrafficParameters")
    .SetParent<Object> ()
    .SetGroupName ("TcpEvaluationSuite")
    .AddAttribute ("FwdFtpFlows", "Number of forward FTP flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nFwdFtpFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RevFtpFlows", "Number of reverse FTP flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nRevFtpFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CrossFtpFlows", "Number of cross FTP flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nCrossFtpFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("NumOfVoiceFlows", "Number of voice flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nVoiceFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("FwdStreamingFlows", "Number of forward streaming flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nFwdStreamingFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RevStreamingFlows", "Number of reverse streaming flows",
                   UintegerValue (5),
                   MakeUintegerAccessor (&TrafficParameters::m_nRevStreamingFlows),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("StreamingRate", "Streaming bit rate in Kbps",
                   DoubleValue (640),
                   MakeDoubleAccessor (&TrafficParameters::m_streamingRate),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("StreamingPacketSize", "Packet size in bytes",
                   UintegerValue (840),
                   MakeUintegerAccessor (&TrafficParameters::m_streamingPacketSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("UseAqm", "Enable or disable AQM in routers",
                   BooleanValue (false),
                   MakeBooleanAccessor (&TrafficParameters::m_useAqm),
                   MakeBooleanChecker ())
    .AddAttribute ("SimulationTime",
                   "Total simulation time in seconds",
                   TimeValue (Seconds (10)),
                   MakeTimeAccessor (&TrafficParameters::m_simulationTime),
                   MakeTimeChecker ())
  ;
  return tid;
}

TrafficParameters::TrafficParameters (void)
{
}

TrafficParameters::~TrafficParameters (void)
{
}

void
TrafficParameters::SetNumOfFwdFtpFlows (uint32_t nFwdFtpFlows)
{
  m_nFwdFtpFlows = nFwdFtpFlows;
}

uint32_t
TrafficParameters::GetNumOfFwdFtpFlows (void) const
{
  return m_nFwdFtpFlows;
}

void
TrafficParameters::SetNumOfRevFtpFlows (uint32_t nRevFtpFlows)
{
  m_nRevFtpFlows = nRevFtpFlows;
}

uint32_t
TrafficParameters::GetNumOfRevFtpFlows (void) const
{
  return m_nRevFtpFlows;
}

void
TrafficParameters::SetNumOfCrossFtpFlows (uint32_t nCrossFtpFlows)
{
  m_nCrossFtpFlows = nCrossFtpFlows;
}

uint32_t
TrafficParameters::GetNumOfCrossFtpFlows (void) const
{
  return m_nCrossFtpFlows;
}

void
TrafficParameters::SetNumOfVoiceFlows (uint32_t nVoiceFlows)
{
  m_nVoiceFlows = nVoiceFlows;
}

uint32_t
TrafficParameters::GetNumOfVoiceFlows (void) const
{
  return m_nVoiceFlows;
}

void
TrafficParameters::SetNumOfFwdStreamingFlows (uint32_t nFwdStreamingFlows)
{
  m_nFwdStreamingFlows = nFwdStreamingFlows;
}

uint32_t
TrafficParameters::GetNumOfFwdStreamingFlows (void) const
{
  return m_nFwdStreamingFlows;
}

void
TrafficParameters::SetNumOfRevStreamingFlows (uint32_t nRevStreamingFlows)
{
  m_nRevStreamingFlows = nRevStreamingFlows;
}

uint32_t
TrafficParameters::GetNumOfRevStreamingFlows (void) const
{
  return m_nRevStreamingFlows;
}

void
TrafficParameters::SetStreamingRate (double streamingRate)
{
  m_streamingRate = streamingRate;
}

double
TrafficParameters::GetStreamingRate (void) const
{
  return m_streamingRate;
}

void
TrafficParameters::SetStreamingPacketSize (uint32_t streamingPacketSize)
{
  m_streamingPacketSize = streamingPacketSize;
}

uint32_t
TrafficParameters::GetStreamingPacketSize (void) const
{
  return m_streamingPacketSize;
}

void
TrafficParameters::SetAqmUsed (bool useAqm)
{
  m_useAqm = useAqm;
}

bool
TrafficParameters::IsAqmUsed (void) const
{
  return m_useAqm;
}

void
TrafficParameters::SetSimulationTime (Time simulationTime)
{
  m_simulationTime = simulationTime;
}

Time
TrafficParameters::GetSimulationTime (void) const
{
  return m_simulationTime;
}

}
