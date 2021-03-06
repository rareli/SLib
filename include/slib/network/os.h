/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CHECKHEADER_SLIB_NETWORK_OS
#define CHECKHEADER_SLIB_NETWORK_OS

#include "definition.h"

#include "ip_address.h"
#include "mac_address.h"
#include "../core/string.h"

namespace slib
{

	class SLIB_EXPORT NetworkInterfaceInfo
	{
	public:
		String name;
		String displayName;
		String description;
		MacAddress macAddress;
		List<IPv4AddressInfo> addresses_IPv4;
		List<IPv6Address> addresses_IPv6;
		
	public:
		NetworkInterfaceInfo();
		
		~NetworkInterfaceInfo();
		
	};
	
	class SLIB_EXPORT Network
	{
	public:
		static sl_bool findInterface(const String& nameOrDisplayName, NetworkInterfaceInfo* pInfo);
		
		static List<NetworkInterfaceInfo> findAllInterfaces();
		
		
		// used in L2PacketInfo::iface, and 0 is returned on error (wrapper of if_nametoindex call)
		static sl_uint32 getInterfaceIndexFromName(const String& name);
		
		// wrapper of if_indextoname
		static String getInterfaceNameFromIndex(sl_uint32 index);
		
		
		static List<IPv4Address> findAllIPv4Addresses();
		
		static List<IPv4AddressInfo> findAllIPv4AddressInfos();
		
		static List<MacAddress> findAllMacAddresses();
		
		
		static List<IPAddress> getIPAddressesFromHostName(const String& hostName);
		
		static IPAddress getIPAddressFromHostName(const String& hostName);
		
		static IPv4Address getIPv4AddressFromHostName(const String& hostName);
		
		static IPv6Address getIPv6AddressFromHostName(const String& hostName);
		
	};

}

#endif
