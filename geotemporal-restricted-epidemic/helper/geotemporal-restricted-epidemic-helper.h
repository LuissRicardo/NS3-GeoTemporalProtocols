/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>, 
 *                    Networks and Data Science Laboratory (NDS-Lab) at the 
 *                    Computing Research Center (CIC-IPN) <www.prime.cic.ipn.mx>
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
 * Author: Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>
 */

#ifndef GEOTEMPORAL_RESTRICTED_EPIDEMIC_HELPER_H
#define GEOTEMPORAL_RESTRICTED_EPIDEMIC_HELPER_H

#include <map>
#include <set>

#include <ns3/ipv4-routing-helper.h>
#include <ns3/node.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

namespace ns3
{

/**
 * \ingroup geotemporal-restricted-epidemic
 * 
 * \brief Helper class that adds Geo-Temporal Restricted Epidemic routing to
 * nodes.
 */
class GeoTemporalRestrictedEpidemicHelper : public Ipv4RoutingHelper
{
private:

  ObjectFactory m_object_factory;


public:

  /**
   * Create an GeoTemporalRestrictedEpidemicHelper that makes life easier for 
   * people who want to install Epidemic routing protocol to nodes.
   */
  GeoTemporalRestrictedEpidemicHelper ();

  /**
   * Constructs a GeoTemporalRestrictedEpidemicHelper instance from another 
   * previously initialized instance (AKA copy constructor).
   * @param copy Instance to copy.
   */
  GeoTemporalRestrictedEpidemicHelper (const GeoTemporalRestrictedEpidemicHelper & copy);

  /**
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method.
   * 
   * @return pointer to clone of this GeoTemporalRestrictedEpidemicHelper
   */
  GeoTemporalRestrictedEpidemicHelper* Copy (void) const override;

  /**
   * This method will be called by ns3::InternetStackHellper::Install
   * @param node The node on which the routing protocol will run.
   * @return A newly-created routing protocol instance.
   */
  Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const override;

  /**
   * This method controls the attributes of ns3::geotemporal_restricted_epidemic::RoutingProtocol.
   * @param name The name of the attribute to set.
   * @param value The value of the attribute to set.
   */
  void Set (const std::string & name, const AttributeValue & value);

  /**
   * Assign a fixed random variable stream number to the random variables used 
   * by this model. Return the number of streams (possibly zero) that have been
   * assigned. The Install() method of the InsternetStackHelper should have 
   * previously called by the user.
   * 
   * @param nodes NodeContainer of the set of nodes for which the Restricted 
   * Epidemic routing protocol should be modified to use a fixed stream.
   * @param stream First stream index to use.
   * @return 
   */
  int64_t AssignStreams (NodeContainer nodes, int64_t stream);

private:

  /**
   * Assignment operator declared private and not implemented to disallow
   * assignment and prevent the compiler from happily inserting its own.
   * @param rhs
   * @return nothing
   */
  GeoTemporalRestrictedEpidemicHelper& operator= (const GeoTemporalRestrictedEpidemicHelper& rhs);
};

} // namespace ns3

#endif /* GEOTEMPORAL_RESTRICTED_EPIDEMIC_HELPER_H */

