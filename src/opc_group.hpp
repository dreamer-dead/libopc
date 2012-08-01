/*
*
*/

#if defined _MSC_VER && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef OPC_GROUP_HPP
#define OPC_GROUP_HPP

#include <string>
#include <list>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atlbase.h>

#include "OPC/opcda.h"
#include "opc_memory.hpp"

namespace opc 
{
	struct item;

	struct item_data_changed;	

	struct group //: protected memory_manager
	{
		group( LPCWSTR name, bool active, DWORD updateMs, FLOAT deadBand, IOPCServer * serverPtr/*, IMalloc * memManager */ );
		void remove( bool force );

		item * add_item( LPCWSTR name, bool active );
		void clear();

		void enable_callback(item_data_changed * ptr);
		void enable_callback_unk(void * ptr);
		void disable_callback();

		void item_changed( 
			DWORD transaction, OPCHANDLE client, 
			const VARIANT& v, const FILETIME& ft, const WORD qual
			);

		ATL::CComPtr<IOPCItemMgt>& item_mgt() { return item_mgt_; }
		ATL::CComPtr<IOPCGroupStateMgt>& group_mgt() { return state_mgt_; }

		~group();

	protected :
		
		friend struct item;

		//IOPCItemMgt * item_mgt() { return item_mgt_; }
		IOPCSyncIO * sync_io() { return sync_io_; }

		std::wstring name_;
		IOPCServer * server_ptr_;
		std::list< item * > items_;
		item_data_changed * data_changed_ptr_;

		// OPC
		OPCHANDLE handle_;
		ATL::CComPtr<IOPCGroupStateMgt>	state_mgt_;
		ATL::CComPtr<IOPCSyncIO>		sync_io_;
		ATL::CComPtr<IOPCItemMgt>		item_mgt_;
		ATL::CComPtr<IOPCDataCallback>	data_callback_;
		DWORD m_dwAdvise;
	};
}

#endif //OPC_GROUP_HPP
