/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_REMOTE_HOST_HPP
#define OPC_REMOTE_HOST_HPP

#include <string>
#include <comcat.h>

#include "OPC/OpcEnum.h"

#include "opc_host.hpp"
#include "opc_memory.hpp"

namespace opc 
{	
	struct remote_host : public host_base< IOPCServerList >//, memory_manager
	{
		remote_host( /* IMalloc * memManager, */ LPCOLESTR host_name );		

	protected :
		virtual void get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& server_id );

		virtual da_server * connect_to( const CLSID& clsid );

	private :		
		std::basic_string< OLECHAR > host_name_;
	};
}

#endif //OPC_REMOTE_HOST_HPP
