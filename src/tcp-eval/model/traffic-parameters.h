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

// Define an object to set traffic parameters in tcp-eval.

#ifndef TRAFFIC_PARAMETERS_H
#define TRAFFIC_PARAMETERS_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/core-module.h"

namespace ns3 {

/**
 * \brief Initializes the traffic parameters.
 *
 * This class consists of setters and getters for traffic parameters
 * The object of this class is passed to the methods where traffic
 * parameters are used
 */
class TrafficParameters : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   */
  TrafficParameters (void);

  /**
   * \brief Destructor
   */
  ~TrafficParameters (void);

  /**
   * \brief Set the number of forward FTP flows
   *
   * \param nFwdFtpFlows the number of forward FTP flows
   */
  void SetNumOfFwdFtpFlows (uint32_t nFwdFtpFlows);

  /**
   * \brief Get the number of forward FTP flows
   *
   * \return the number of forward FTP flows
   */
  uint32_t GetNumOfFwdFtpFlows (void) const;

  /**
   * \brief Set the number of reverse FTP flows
   *
   * \param nRevFtpFlows the number of reverse FTP flows
   */
  void SetNumOfRevFtpFlows (uint32_t nRevFtpFlows);

  /**
   * \brief Get the number of reverse FTP flows
   *
   * \return the number of reverse FTP flows
   */
  uint32_t GetNumOfRevFtpFlows (void) const;

  /**
   * \brief Set the number of cross FTP flows
   *
   * \param nCrossFtpFlows the number of cross FTP flows
   */
  void SetNumOfCrossFtpFlows (uint32_t nCrossFtpFlows);

  /**
    * \brief Get the number of cross FTP flows
    *
    * \return the number of cross FTP flows
    */
  uint32_t GetNumOfCrossFtpFlows (void) const;

  /**
    * \brief Set the number of voice flows
    *
    * \param nVoiceFlows the number of voice flows
    */
  void SetNumOfVoiceFlows (uint32_t nVoiceFlows);

  /**
    * \brief Get the number of voice flows
    *
    * \return the number of voice flows
    */
  uint32_t GetNumOfVoiceFlows (void) const;

  /**
   * \brief Set the number of forward streaming flows
   *
   * \param nFwdStreamingFlows the number of forward streaming flows
   */
  void SetNumOfFwdStreamingFlows (uint32_t nFwdStreamingFlows);

  /**
    * \brief Get the number of forward streaming flows
    *
    * \return the number of forward streaming flows
    */
  uint32_t GetNumOfFwdStreamingFlows (void) const;

  /**
   * \brief Set the number of reverse streaming flows
   *
   * \param nRevStreamingFlows the number of reverse streaming flows
   */
  void SetNumOfRevStreamingFlows (uint32_t nRevStreamingFlows);

  /**
    * \brief Get the number of reverse streaming flows
    *
    * \return the number of reverse streaming flows
    */
  uint32_t GetNumOfRevStreamingFlows (void) const;

  /**
   * \brief Set the bit rate of streaming flows
   *
   * \param streamingRate the bit rate of streaming flows
   */
  void SetStreamingRate (double streamingRate);

  /**
    * \brief Get the bit rate of streaming flows
    *
    * \return the bit rate of streaming flows in Kbps
    */
  double GetStreamingRate (void) const;

  /**
   * \brief Set the packet size of streaming flows
   *
   * \param streamingPacketSize the packet size of streaming flows
   */
  void SetStreamingPacketSize (uint32_t streamingPacketSize);

  /**
    * \brief Get the packet size of streaming flows
    *
    * \return the packet size of streaming flows in bytes
    */
  uint32_t GetStreamingPacketSize (void) const;

  /**
   * \brief Set whether AQM is to be used
   *
   * \param useAqm True if AQM is enabled
   */
  void SetAqmUsed (bool useAqm);

  /**
    * \brief Check whether AQM is to be used
    *
    * \return True if AQM is enabled
    */
  bool IsAqmUsed (void) const;

  /**
    * \brief Set the simulation time in seconds
    *
    * \param simulationTime Time when the simulation should stop
    */
  void SetSimulationTime (Time simulationTime);

  /**
    * \brief Get the simulation time in seconds
    *
    * \return the simulation time in seconds
    */
  Time GetSimulationTime (void) const;


private:
  uint32_t    m_nFwdFtpFlows;           //!< Number of FTP flows on forward path (left -> right)
  uint32_t    m_nRevFtpFlows;           //!< Number of FTP flows on reverse path (right -> left)
  uint32_t    m_nCrossFtpFlows;         //!< Number of FTP flows on cross paths (top -> down in parking-lot)
  uint32_t    m_nVoiceFlows;            //!< Number of two-way voice flows
  uint32_t    m_nFwdStreamingFlows;     //!< Number of streaming flows on forward path
  uint32_t    m_nRevStreamingFlows;     //!< Number of streaming flows on reverse path
  double      m_streamingRate;          //!< Bit rate of streaming flows in Kbps
  uint32_t    m_streamingPacketSize;    //!< Packet size of streaming flows in bytes
  bool        m_useAqm;                 //!< Enable or disable AQM in routers
  Time        m_simulationTime;         //!< Total simulation time in seconds
};

}

#endif /* TRAFFIC_PARAMETERS_H */

