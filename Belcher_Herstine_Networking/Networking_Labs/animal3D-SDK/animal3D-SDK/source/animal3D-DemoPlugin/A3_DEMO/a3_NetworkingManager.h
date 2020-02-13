/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_NetworkingManager.h
	Networking manager interface.
*/

#ifndef __ANIMAL3D_NETWORKINGMANAGER_H
#define __ANIMAL3D_NETWORKINGMANAGER_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_NetworkingManager				a3_NetworkingManager;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	typedef a3byte a3netAddressStr[16];

	// networking manager
	struct a3_NetworkingManager
	{
		a3ui16 port_inbound, port_outbound;
		a3ui16 maxConnect_inbound, maxConnect_outbound;
		void* peer;
	};


//-----------------------------------------------------------------------------

	// startup networking
	a3i32 a3netStartup(a3_NetworkingManager* net, a3ui16 port_inbound, a3ui16 port_outbound, a3ui16 maxConnect_inbound, a3ui16 maxConnect_outbound);

	// shutdown networking
	a3i32 a3netShutdown(a3_NetworkingManager* net);


	// connect
	a3i32 a3netConnect(a3_NetworkingManager* net, a3netAddressStr const ip);

	// disconnect
	a3i32 a3netDisconnect(a3_NetworkingManager* net);


	// process inbound packets
	a3i32 a3netProcessInbound(a3_NetworkingManager* net);

	// process outbound packets
	a3i32 a3netProcessOutbound(a3_NetworkingManager* net);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_NETWORKINGMANAGER_H