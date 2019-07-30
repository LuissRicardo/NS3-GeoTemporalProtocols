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
 * This is an example script for the Geo-Temporal routing protocol.
 * 
 * Author: Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>
 */

#include "geotemporal-installer.h"

#include <ns3/log.h>

using namespace ns3;

int
main (int argc, char **argv)
{
  //  LogComponentEnable ("GeoTemporalInstaller", LOG_LEVEL_ALL);
  //  LogComponentEnable ("GeoTemporalInstaller", LOG_PREFIX_FUNC);
  //  LogComponentEnable ("GeoTemporalInstaller", LOG_PREFIX_NODE);
  //  LogComponentEnable ("GeoTemporalInstaller", LOG_PREFIX_TIME);

  //  LogComponentEnable ("GeoTemporalApplication", LOG_LEVEL_ALL);
  //  LogComponentEnable ("GeoTemporalApplication", LOG_PREFIX_FUNC);
  //  LogComponentEnable ("GeoTemporalApplication", LOG_PREFIX_NODE);
  //  LogComponentEnable ("GeoTemporalApplication", LOG_PREFIX_TIME);

  //  LogComponentEnable ("GeoTemporalRoutingProtocol", LOG_LEVEL_ALL);
  //  LogComponentEnable ("GeoTemporalRoutingProtocol", LOG_PREFIX_FUNC);
  //  LogComponentEnable ("GeoTemporalRoutingProtocol", LOG_PREFIX_NODE);
  //  LogComponentEnable ("GeoTemporalRoutingProtocol", LOG_PREFIX_TIME);

  //  LogComponentEnable ("GeoTemporalPacketsQueue", LOG_LEVEL_ALL);
  //  LogComponentEnable ("GeoTemporalPacketsQueue", LOG_PREFIX_FUNC);
  //  LogComponentEnable ("GeoTemporalPacketsQueue", LOG_PREFIX_NODE);
  //  LogComponentEnable ("GeoTemporalPacketsQueue", LOG_PREFIX_TIME);

  GeoTemporalInstaller example;

  if (!example.Configure (argc, argv))
    NS_FATAL_ERROR ("Configuration failed. Aborted!");

  example.Run ();
}

