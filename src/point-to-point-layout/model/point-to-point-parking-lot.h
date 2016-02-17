/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK, Surathkal
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

// Define an object to create a parking-lot topology.

#ifndef POINT_TO_POINT_PARKING_LOT_HELPER_H
#define POINT_TO_POINT_PARKING_LOT_HELPER_H

#include <string>

#include "point-to-point-helper.h"
#include "ipv4-address-helper.h"
#include "internet-stack-helper.h"
#include "ipv4-interface-container.h"

namespace ns3 {

/**
 * \ingroup point-to-point-layout
 *
 * \brief A helper to make it easier to create a parking-lot topology
 * with p2p links
 */
class PointToPointParkingLotHelper
{
public:
  /**
   * Create a PointToPointParkingLotHelper in order to easily create
   * parking-lot topologies using p2p links
   *
   * \param nLeftLeaf number of left side leaf nodes in the parking-lot
   *
   * \param leftHelper PointToPointHelper used to install the links
   *                   between the left leaf nodes and the left-most
   *                   router
   *
   * \param nRightLeaf number of right side leaf nodes in the parking-lot
   *
   * \param rightHelper PointToPointHelper used to install the links
   *                    between the right leaf nodes and the right-most
   *                    router
   *
   * \param nBottleneck number of bottleneck links in the parking-lot
   *                    between the left leaf nodes and right leaf nodes
   *
   * \param nCrossFlowsAtRouter number of cross flows passing through a router
   *
   * \param crossHelper PointToPointHelper used to install the link
   *                    between the router and cross sources
   *
   * \param bottleneckHelper PointToPointHelper used to install the link
   *                         between the inner-routers, usually known as
   *                         the bottleneck link
   */
  PointToPointParkingLotHelper (uint32_t nLeftLeaf,
                                PointToPointHelper leftHelper,
                                uint32_t nRightLeaf,
                                PointToPointHelper rightHelper,
                                uint32_t nBottleneck,
                                uint32_t nCrossFlowsAtRouter,
                                PointToPointHelper crossHelper,
                                PointToPointHelper bottleneckHelper);

  ~PointToPointParkingLotHelper ();

public:
  /**
   * \returns pointer to the i'th left side leaf node
   * \param i node number
   */
  Ptr<Node> GetLeft (uint32_t i) const;

  /**
   * \returns pointer to the i'th left side leaf node
   * \param i node number
   */
  Ptr<Node> GetRight (uint32_t i) const;

  /**
   * \returns pointer to the i'th router
   * \param i router number
   */
  Ptr<Node> GetRouter (uint32_t i) const;

  /**
   * \returns pointer to the j'th cross-source node attached to the i'th router
   * \param routerIndex router number
   * \param crossSourceIndex cross-source node number
   */
  Ptr<Node> GetCrossSource (uint32_t routerIndex, uint32_t crossSourceIndex) const;

  /**
  * \returns pointer to the j'th cross-sink node attached to the i'th router
  * \param routerIndex router number
  * \param crossSinkIndex cross-sink node number
  */
  Ptr<Node> GetCrossSink (uint32_t routerIndex, uint32_t crossSinkIndex) const;

  /**
   * \returns an Ipv4Address of the i'th left leaf
   * \param i node number
   */
  Ipv4Address GetLeftIpv4Address (uint32_t i ) const; // Get left leaf address

  /**
   * \returns an Ipv4Address of the i'th right leaf
   * \param i node number
   */
  Ipv4Address GetRightIpv4Address (uint32_t i) const;

  /**
   * \returns an Ipv6Address of the i'th left leaf
   * \param i node number
   */
  Ipv6Address GetLeftIpv6Address (uint32_t i ) const; // Get left leaf address

  /**
   * \returns an Ipv6Address of the i'th right leaf
   * \param i node number
   */
  Ipv6Address GetRightIpv6Address (uint32_t i) const; // Get right leaf address

  /**
   * \returns an Ipv4Address of the j'th cross-source node attached to the i'th router
   * \param routerIndex router number
   * \param crossSourceIndex cross-source node number
   */
  Ipv4Address GetCrossSourceIpv4Address (uint32_t routerIndex, uint32_t crossSourceIndex) const;

  /**
   * \returns an Ipv4Address of the j'th cross-sink node attached to the i'th router
   * \param routerIndex router number
   * \param crossSinkIndex cross-sink node number
   */
  Ipv4Address GetCrossSinkIpv4Address (uint32_t routerIndex, uint32_t crossSinkIndex) const;

  /**
   * \returns an Ipv4Address of an interface of i'th router that connects to j'th cross-source node
   * \param routerIndex router number
   * \param crossSourceIndex cross-source node number
   */
  Ipv4Address GetRouterCrossSourceIpv4Address (uint32_t routerIndex, uint32_t crossSourceIndex) const;

  /**
   * \returns an Ipv4Address of an interface of i'th router that connects to j'th cross-sink node
   * \param routerIndex router number
   * \param crossSinkIndex cross-sink node number
   */
  Ipv4Address GetRouterCrossSinkIpv4Address (uint32_t routerIndex, uint32_t crossSinkIndex) const;

  /**
   * \returns an Ipv4Address of an interface that connects i'th router to j'th router
   * \param fromRouterIndex i'th router number
   * \param toRouterIndex j'th router number
   */
  Ipv4Address GetRouterToRouterIpv4Address (uint32_t fromRouterIndex, uint32_t toRouterIndex) const;

  /**
   * \returns total number of left side leaf nodes
   */
  uint32_t  LeftCount () const;

  /**
   * \returns total number of right side leaf nodes
   */
  uint32_t  RightCount () const;

  /**
   * \returns total number of routers
   */
  uint32_t RouterCount () const;

  /**
   * \returns total number of cross-source nodes attached to i'th router
   * \param routerIndex router number
   */
  uint32_t  CrossSourceCount (uint32_t routerIndex) const;

  /**
   * \returns total number of cross-sink nodes attached to i'th router
   * \param routerIndex router number
   */
  uint32_t  CrossSinkCount (uint32_t routerIndex) const;

  /**
   * \param stack an InternetStackHelper which is used to install
   *              on every node in the parking-lot
   */
  void      InstallStack (InternetStackHelper stack);

  /**
   * \param leftIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *               interfaces on the left side of the parking-lot
   *
   * \param rightIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *                interfaces on the right side of the parking-lot
   *
   * \param routerIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *                 interfaces on the bottleneck link
   *
   * \param crossSourceIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *                      interfaces between router and cross-source nodes
   *
   * \param crossSinkIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *                    interfaces between router and cross-sink nodes
   */
  void      AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                                 Ipv4AddressHelper rightIp,
                                 Ipv4AddressHelper routerIp,
                                 Ipv4AddressHelper crossSourceIp,
                                 Ipv4AddressHelper crossSinkIp);

  /**
   * \param network an IPv6 address representing the network portion
   *                of the IPv6 Address
   * \param prefix the prefix length
   */
  void      AssignIpv6Addresses (Ipv6Address network, Ipv6Prefix prefix);

private:
  NodeContainer          m_leftLeaf;                                    //!< Left Leaf nodes
  NetDeviceContainer     m_leftLeafDevices;                             //!< Left Leaf NetDevices
  NodeContainer          m_rightLeaf;                                   //!< Right Leaf nodes
  NetDeviceContainer     m_rightLeafDevices;                            //!< Right Leaf NetDevices
  NodeContainer          m_routers;                                     //!< Routers
  NetDeviceContainer     m_routerDevices;                               //!< Routers NetDevices
  NetDeviceContainer     m_leftRouterDevices;                           //!< Left router NetDevices
  NetDeviceContainer     m_rightRouterDevices;                          //!< Right router NetDevices
  Ipv4InterfaceContainer m_leftLeafInterfaces;                          //!< Left Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_leftRouterInterfaces;                        //!< Left router interfaces (IPv4)
  Ipv4InterfaceContainer m_rightLeafInterfaces;                         //!< Right Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_rightRouterInterfaces;                       //!< Right router interfaces (IPv4)
  Ipv4InterfaceContainer m_routerInterfaces;                            //!< Router interfaces (IPv4)
  Ipv6InterfaceContainer m_leftLeafInterfaces6;                         //!< Left Leaf interfaces (IPv6)
  Ipv6InterfaceContainer m_leftRouterInterfaces6;                       //!< Left router interfaces (IPv6)
  Ipv6InterfaceContainer m_rightLeafInterfaces6;                        //!< Right Leaf interfaces (IPv6)
  Ipv6InterfaceContainer m_rightRouterInterfaces6;                      //!< Right router interfaces (IPv6)
  Ipv6InterfaceContainer m_routerInterfaces6;                           //!< Router interfaces (IPv6)
  std::vector<NodeContainer> m_crossSources;                            //!< Cross-source nodes
  std::vector<NetDeviceContainer>     m_crossSourceDevices;             //!< Cross-source NetDevices
  std::vector<NodeContainer>          m_crossSinks;                     //!< Cross-sink nodes
  std::vector<NetDeviceContainer>     m_crossSinkDevices;               //!< Cross-sink NetDevices
  std::vector<NetDeviceContainer>     m_routerToCrossSourceDevices;     //!< Router cross-source NetDevices
  std::vector<NetDeviceContainer>     m_routerToCrossSinkDevices;       //!< Router cross-sink NetDevices
  std::vector<Ipv4InterfaceContainer> m_routerToCrossSourceInterfaces;  //!< Router cross-source interfaces (IPv4)
  std::vector<Ipv4InterfaceContainer> m_routerToCrossSinkInterfaces;    //!< Router cross-sink interfaces (IPv4)
  std::vector<Ipv4InterfaceContainer> m_crossSourceInterfaces;          //!< Cross-source interfaces (IPv4)
  std::vector<Ipv4InterfaceContainer> m_crossSinkInterfaces;            //!< Cross-sink interfaces (IPv4)
  std::vector<Ipv6InterfaceContainer> m_routerToCrossSourceInterfaces6; //!< Router cross-source interfaces (IPv6)
  std::vector<Ipv6InterfaceContainer> m_routerToCrossSinkInterfaces6;   //!< Router cross-sink interfaces (IPv6)
  std::vector<Ipv6InterfaceContainer> m_crossSourceInterfaces6;         //!< Cross-source interfaces (IPv6)
  std::vector<Ipv6InterfaceContainer> m_crossSinkInterfaces6;           //!< Cross-sink interfaces (IPv6)
};
}
#endif /* POINT_TO_POINT_PARKING_LOT_HELPER_H */
