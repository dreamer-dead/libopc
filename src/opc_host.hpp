/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_HOST_HPP
#define OPC_HOST_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <comdef.h>
#include <atlbase.h>

#include "OPC/opcda.h"

namespace opc 
{
	struct da_server;

	enum DA_VERSION 
	{
		DA_10 = 10,
		DA_20 = 20,
		DA_30 = 30
	};

	struct host 
	{
		virtual da_server * connect( LPCOLESTR server_name, DA_VERSION version ) = 0;
	};

	host * make_local_host();
	host * make_host( LPCOLESTR address );

	template < typename T >
	struct host_base : host 
	{
	protected :
		virtual da_server * connect_to( const CLSID clsid ) = 0;

		virtual void get_clsid( const CATID& cat_id, LPCOLESTR server_name, CLSID& clsid ) = 0;

		da_server * connect( LPCOLESTR server_name, DA_VERSION version )
		{
			CLSID server_id;

			switch( version )
			{
			case DA_20 :
				{
					this->get_clsid( IID_CATID_OPCDAServer20, server_name, server_id );				
				}		
				break;

			case DA_30 :
				{
					this->get_clsid( IID_CATID_OPCDAServer30, server_name, server_id );				
				}
				break;

			default :
				return NULL;
			}

			return connect_to( server_id );			
		}

		ATL::CComPtr< T > root_;
	};
}

#endif //OPC_HOST_HPP
