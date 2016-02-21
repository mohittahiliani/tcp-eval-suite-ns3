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

// Implement an object to configure topology in tcp-eval.

#include "configure-topology.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ConfigureTopology");

NS_OBJECT_ENSURE_REGISTERED (ConfigureTopology);

TypeId
ConfigureTopology::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ConfigureTopology")
    .SetParent<Object> ()
    .SetGroupName ("TcpEvaluationSuite")
    .AddAttribute ("BottleneckBandwidth",
                   "Bottleneck link capacity in Mbps",
                   DoubleValue (10),
                   MakeDoubleAccessor (&ConfigureTopology::m_bottleneckBandwidth),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("BottleneckCount", "Number of bottleneck links",
                   UintegerValue (1),
                   MakeUintegerAccessor (&ConfigureTopology::m_nBottlenecks),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RTTP",
                   "Round trip propagation delay in seconds",
                   TimeValue (Seconds (0.08)),
                   MakeTimeAccessor (&ConfigureTopology::m_rttp),
                   MakeTimeChecker ())
    .AddAttribute ("RttDiff",
                   "Flow RTT difference in seconds",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ConfigureTopology::m_rttDifference),
                   MakeTimeChecker ())
  ;
  return tid;
}

ConfigureTopology::ConfigureTopology (void)
{
}

ConfigureTopology::~ConfigureTopology (void)
{
}

void
ConfigureTopology::SetTopologyParameters (Ptr<TrafficParameters> traffic, uint32_t nBottlenecks)
{
  // m_rttp is in seconds. To perform calculations on TimeValue, they are first converted to double
  double rttp = m_rttp.ToDouble (Time::S);
  double rttDifference = m_rttDifference.ToDouble (Time::S);

  m_bottleneckDelay = Seconds ((rttp * 0.5 * 0.8) / nBottlenecks);
  m_delayDifference = Seconds ((rttDifference * 1.0) / 4.0);
  m_nonBottleneckDelay = Seconds ((rttp * 0.5 * 0.2) / 2.0);
  m_nonBottleneckBandwidth = m_bottleneckBandwidth * 1.0 * 2;

  uint32_t numOfFtpFwd = traffic->GetNumOfFwdFtpFlows ();
  uint32_t numOfFtpRev = traffic->GetNumOfRevFtpFlows ();
  uint32_t minBottleneckBuffer = (2 * 1.0 * (numOfFtpFwd + numOfFtpRev));
  m_bottleneckBufferBdp = 1.0;

  double avgRtt = (rttp + ((rttDifference * 1.0 * (numOfFtpFwd - 1)) / 2)) * 1000;
  m_bottleneckBuffer = (m_bottleneckBufferBdp * m_bottleneckBandwidth * avgRtt) / 8.0;

  if (m_bottleneckBuffer < minBottleneckBuffer)
    {
      m_bottleneckBuffer = minBottleneckBuffer;
    }

  m_nonBottleneckBuffer = m_bottleneckBuffer;
}

void
ConfigureTopology::SetRedParameters ()
{
  Config::SetDefault ("ns3::RedQueue::Mode", StringValue ("QUEUE_MODE_PACKETS"));
  Config::SetDefault ("ns3::RedQueue::MinTh", DoubleValue (0.6 * m_bottleneckBuffer));
  Config::SetDefault ("ns3::RedQueue::MaxTh", DoubleValue (0.8 * m_bottleneckBuffer));
  Config::SetDefault ("ns3::RedQueue::QW", DoubleValue (0.001));
  Config::SetDefault ("ns3::RedQueue::LInterm", DoubleValue (10));
  Config::SetDefault ("ns3::RedQueue::Gentle", BooleanValue (true));
  Config::SetDefault ("ns3::RedQueue::QueueLimit", UintegerValue (m_bottleneckBuffer));
}

void
ConfigureTopology::SetBottleneckBandwidth (double bottleneckBandwidth)
{
  m_bottleneckBandwidth = bottleneckBandwidth;
}

double
ConfigureTopology::GetBottleneckBandwidth (void) const
{
  return m_bottleneckBandwidth;
}

void
ConfigureTopology::SetBottleneckCount (uint32_t nBottlenecks)
{
  m_nBottlenecks = nBottlenecks;
}

uint32_t
ConfigureTopology::BottleneckCount (void) const
{
  return m_nBottlenecks;
}

void
ConfigureTopology::SetRttp (Time rttp)
{
  m_rttp = rttp;
}

Time
ConfigureTopology::GetRttp (void) const
{
  return m_rttp;
}

void
ConfigureTopology::SetRttDifference (Time rttDifference)
{
  m_rttDifference = rttDifference;
}

Time
ConfigureTopology::GetRttDifference (void) const
{
  return m_rttDifference;
}

Time
ConfigureTopology::GetDelayDifference (void) const
{
  return m_delayDifference;
}

Time
ConfigureTopology::GetBottleneckDelay (void) const
{
  return m_bottleneckDelay;
}

uint32_t
ConfigureTopology::GetBottleneckBuffer (void) const
{
  return m_bottleneckBuffer;
}

Time
ConfigureTopology::GetNonBottleneckDelay (void) const
{
  return m_nonBottleneckDelay;
}

double
ConfigureTopology::GetNonBottleneckBandwidth (void) const
{
  return m_nonBottleneckBandwidth;
}

uint32_t
ConfigureTopology::GetNonBottleneckBuffer (void) const
{
  return m_nonBottleneckBuffer;
}

}
