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

// Implement an object to create a parking lot topology.

#include <cmath>
#include <iostream>
#include <sstream>

// ns3 includes
#include "ns3/log.h"
#include "ns3/point-to-point-parking-lot.h"

#include "ns3/node-list.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/string.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PointToPointParkingLotHelper");

PointToPointParkingLotHelper::PointToPointParkingLotHelper (uint32_t nLeftLeaf,
                                                            PointToPointHelper leftHelper,
                                                            uint32_t nRightLeaf,
                                                            PointToPointHelper rightHelper,
                                                            uint32_t nBottleneck,
                                                            uint32_t nCrossSourceAtEachRouter,
                                                            PointToPointHelper crossHelper,
                                                            PointToPointHelper bottleneckHelper)
{
  // Set nRouters to the number of bottleneck links + 1
  uint32_t nRouters = nBottleneck + 1;

  // Create the leaf nodes
  m_leftLeaf.Create (nLeftLeaf);
  m_rightLeaf.Create (nRightLeaf);

  // Install point-to-point bottleneck links across nRouters
  for (uint32_t i = 0; i < nRouters; ++i)
    {
      m_routers.Create (1);
      if (i > 0)
        {
          m_routerDevices.Add (bottleneckHelper.Install (m_routers.Get (i - 1),
                                                         m_routers.Get (i)));
        }
    }

  // Add the left side links
  for (uint32_t i = 0; i < nLeftLeaf; ++i)
    {
      NetDeviceContainer c = leftHelper.Install (m_routers.Get (0),
                                                 m_leftLeaf.Get (i));
      m_leftRouterDevices.Add (c.Get (0));
      m_leftLeafDevices.Add (c.Get (1));
    }

  // Add the right side links
  for (uint32_t i = 0; i < nRightLeaf; ++i)
    {
      NetDeviceContainer c = rightHelper.Install (m_routers.Get (nBottleneck),
                                                  m_rightLeaf.Get (i));
      m_rightRouterDevices.Add (c.Get (0));
      m_rightLeafDevices.Add (c.Get (1));
    }

  // Add the cross links
  for (uint32_t i = 0; i < nBottleneck; ++i)
    {
      NodeContainer crossSource, crossSink;
      NetDeviceContainer crossSourceDevices, crossSinkDevices, routerToCrossSourceDevices, routerToCrossSinkDevices;

      crossSource.Create (nCrossSourceAtEachRouter);
      crossSink.Create (nCrossSourceAtEachRouter);

      for (uint32_t j = 0; j < nCrossSourceAtEachRouter; ++j)
        {
          NetDeviceContainer c1 = crossHelper.Install (m_routers.Get (i),
                                                       crossSource.Get (j));


          NetDeviceContainer c2 = crossHelper.Install (m_routers.Get (i + 1),
                                                       crossSink.Get (j));
          routerToCrossSourceDevices.Add (c1.Get (0));
          crossSourceDevices.Add (c1.Get (1));

          routerToCrossSinkDevices.Add (c2.Get (0));
          crossSinkDevices.Add (c2.Get (1));
        }

      m_crossSources.push_back (crossSource);
      m_crossSinks.push_back (crossSink);
      m_crossSourceDevices.push_back (crossSourceDevices);
      m_crossSinkDevices.push_back (crossSinkDevices);
      m_routerToCrossSourceDevices.push_back (routerToCrossSourceDevices);
      m_routerToCrossSinkDevices.push_back (routerToCrossSinkDevices);
    }
}

PointToPointParkingLotHelper::~PointToPointParkingLotHelper ()
{
}

void
PointToPointParkingLotHelper::InstallStack (InternetStackHelper stack)
{
  stack.Install (m_routers);
  stack.Install (m_leftLeaf);
  stack.Install (m_rightLeaf);

  //Install stack on Cross Sources and Cross Sinks
  for (uint32_t i = 0; i < m_crossSources.size (); ++i)
    {
      NodeContainer crossSourcesAtRouterI = m_crossSources[i];
      stack.Install (crossSourcesAtRouterI);

      NodeContainer crossSinkAtRouterI = m_crossSinks[i];
      stack.Install (crossSinkAtRouterI);
    }
}


void
PointToPointParkingLotHelper::AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                                                   Ipv4AddressHelper rightIp,
                                                   Ipv4AddressHelper routerIp,
                                                   Ipv4AddressHelper crossSourceIp,
                                                   Ipv4AddressHelper crossSinkIp)
{
  // Assign to left side
  for (uint32_t i = 0; i < LeftCount (); ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (m_leftLeafDevices.Get (i));
      ndc.Add (m_leftRouterDevices.Get (i));
      Ipv4InterfaceContainer ifc = leftIp.Assign (ndc);
      m_leftLeafInterfaces.Add (ifc.Get (0));
      m_leftRouterInterfaces.Add (ifc.Get (1));
      leftIp.NewNetwork ();
    }

  // Assign to right side
  for (uint32_t i = 0; i < RightCount (); ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (m_rightLeafDevices.Get (i));
      ndc.Add (m_rightRouterDevices.Get (i));
      Ipv4InterfaceContainer ifc = rightIp.Assign (ndc);
      m_rightLeafInterfaces.Add (ifc.Get (0));
      m_rightRouterInterfaces.Add (ifc.Get (1));
      rightIp.NewNetwork ();
    }

  // Assign to router network
  for (uint32_t i = 0; i < m_routerDevices.GetN (); i += 2)
    {
      m_routerInterfaces.Add (routerIp.Assign (m_routerDevices.Get (i)));
      m_routerInterfaces.Add (routerIp.Assign (m_routerDevices.Get (i + 1)));
      routerIp.NewNetwork ();
    }

  // Assign to cross sources and sinks
  for (uint32_t i = 0; i < RouterCount () - 1; ++i)
    {
      uint32_t crossSourcesAtRouterI = m_crossSourceDevices[i].GetN ();
      Ipv4InterfaceContainer routerToCrossSourceInterfaces, crossSourceInterfaces, routerToCrossSinkInterfaces, crossSinkInterfaces;
      for (uint32_t j = 0; j < crossSourcesAtRouterI; ++j)
        {
          NetDeviceContainer ndc;
          ndc.Add (m_routerToCrossSourceDevices[i].Get (j));
          ndc.Add (m_crossSourceDevices[i].Get (j));
          Ipv4InterfaceContainer ifc = crossSourceIp.Assign (ndc);
          routerToCrossSourceInterfaces.Add (ifc.Get (0));
          crossSourceInterfaces.Add (ifc.Get (1));
          crossSourceIp.NewNetwork ();

          NetDeviceContainer ndc2;
          ndc2.Add (m_routerToCrossSinkDevices[i].Get (j));
          ndc2.Add (m_crossSinkDevices[i].Get (j));
          Ipv4InterfaceContainer ifc2 = crossSinkIp.Assign (ndc2);

          routerToCrossSinkInterfaces.Add (ifc2.Get (0));
          crossSinkInterfaces.Add (ifc2.Get (1));
          crossSinkIp.NewNetwork ();
        }
      m_routerToCrossSourceInterfaces.push_back (routerToCrossSourceInterfaces);
      m_crossSourceInterfaces.push_back (crossSourceInterfaces);
      m_routerToCrossSinkInterfaces.push_back (routerToCrossSinkInterfaces);
      m_crossSinkInterfaces.push_back (crossSinkInterfaces);
    }
}

Ptr<Node> PointToPointParkingLotHelper::GetLeft (uint32_t i) const
{ // Get the i'th left side leaf
  return m_leftLeaf.Get (i);
}

Ptr<Node> PointToPointParkingLotHelper::GetRight (uint32_t i) const
{ // Get the i'th right side leaf
  return m_rightLeaf.Get (i);
}

Ptr<Node> PointToPointParkingLotHelper::GetRouter (uint32_t i) const
{ // Get the i'th router node
  return m_routers.Get (i);
}

Ptr<Node> PointToPointParkingLotHelper::GetCrossSource (uint32_t routerIndex, uint32_t crossSourceIndex) const
{ // Get the i'th cross source leaf
  return m_crossSources[routerIndex].Get (crossSourceIndex);
}

Ptr<Node> PointToPointParkingLotHelper::GetCrossSink (uint32_t routerIndex, uint32_t crossSinkIndex) const
{ // Get the i'th cross sink leaf
  return m_crossSinks[routerIndex].Get (crossSinkIndex);
}

Ipv4Address PointToPointParkingLotHelper::GetLeftIpv4Address (uint32_t i) const
{
  return m_leftLeafInterfaces.GetAddress (i);
}

Ipv4Address PointToPointParkingLotHelper::GetRightIpv4Address (uint32_t i) const
{
  return m_rightLeafInterfaces.GetAddress (i);
}

Ipv4Address PointToPointParkingLotHelper::GetCrossSourceIpv4Address (uint32_t routerIndex, uint32_t crossSourceIndex) const
{
  return m_crossSourceInterfaces[routerIndex].GetAddress (crossSourceIndex);
}

Ipv4Address PointToPointParkingLotHelper::GetCrossSinkIpv4Address (uint32_t routerIndex, uint32_t crossSinkIndex) const
{
  return m_crossSinkInterfaces[routerIndex].GetAddress (crossSinkIndex);
}

Ipv4Address PointToPointParkingLotHelper::GetRouterCrossSourceIpv4Address (uint32_t routerIndex, uint32_t crossSourceIndex) const
{
  return m_routerToCrossSourceInterfaces[routerIndex].GetAddress (crossSourceIndex);
}

Ipv4Address PointToPointParkingLotHelper::GetRouterCrossSinkIpv4Address (uint32_t routerIndex, uint32_t crossSinkIndex) const
{
  return m_routerToCrossSinkInterfaces[routerIndex].GetAddress (crossSinkIndex);
}

Ipv4Address PointToPointParkingLotHelper::GetRouterToRouterIpv4Address (uint32_t fromRouterIndex, uint32_t toRouterIndex) const
{
  if (fromRouterIndex < toRouterIndex)
    {
      // m_routerInterfaces contains the information of intermediate node twice because of different interfaces
      return m_routerInterfaces.GetAddress (2 * fromRouterIndex);
    }
  return m_routerInterfaces.GetAddress ((2 * fromRouterIndex) - 1);
}

uint32_t  PointToPointParkingLotHelper::LeftCount () const
{ // Number of left side nodes
  return m_leftLeaf.GetN ();
}

uint32_t  PointToPointParkingLotHelper::RightCount () const
{ // Number of right side nodes
  return m_rightLeaf.GetN ();
}

uint32_t  PointToPointParkingLotHelper::RouterCount () const
{ // Number of routers
  return m_routers.GetN ();
}

uint32_t  PointToPointParkingLotHelper::CrossSourceCount (uint32_t routerIndex) const
{ // Number of cross source nodes
  return m_crossSources[routerIndex].GetN ();
}

uint32_t  PointToPointParkingLotHelper::CrossSinkCount (uint32_t routerIndex) const
{ // Number of cross sink nodes
  return m_crossSinks[routerIndex].GetN ();
}

Ptr<NetDevice> PointToPointParkingLotHelper::GetRouterCrossSourceNetDevice (uint32_t routerIndex, uint32_t crossSourceIndex) const
{
  return m_routerToCrossSourceDevices[routerIndex].Get (crossSourceIndex);
}

Ptr<NetDevice> PointToPointParkingLotHelper::GetRouterCrossSinkNetDevice(uint32_t routerIndex, uint32_t crossSinkIndex) const
{
  return m_routerToCrossSinkDevices[routerIndex].Get (crossSinkIndex);
}

Ptr<NetDevice> PointToPointParkingLotHelper::GetRouterToRouterNetDevice (uint32_t fromRouterIndex, uint32_t toRouterIndex) const
{
  if (fromRouterIndex < toRouterIndex)
    {
      // m_routerDevices contains the information of intermediate node twice because of different interfaces
      return m_routerDevices.Get (2 * fromRouterIndex);
    }
  return m_routerDevices.Get ((2 * fromRouterIndex) - 1);
}
}
