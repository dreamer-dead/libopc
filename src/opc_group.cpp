#include "opc_group.hpp"
#include "opc_item.hpp"
#include "opc_exception.hpp"
#include "opc_data_callback.hpp"

namespace opc 
{
	group::group( LPCWSTR name, bool active, DWORD updateMs, FLOAT deadBand, IOPCServer * serverPtr /*, IMalloc * memManager */ )
		: name_(name)
		, server_ptr_(serverPtr)
		//, memory_manager(memManager)
	{
		DWORD rev_update_ms = 0;
		HRESULT result = server_ptr_->AddGroup( name, (active ? TRUE : FALSE), updateMs, 0, 0, &deadBand,
			0, &handle_, &rev_update_ms, IID_IOPCGroupStateMgt, (LPUNKNOWN*)&state_mgt_);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to Add group") );
		}

		result = state_mgt_->QueryInterface(IID_IOPCSyncIO, (void**)&sync_io_);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get IID_IOPCSyncIO") );
		}

		/*
		result = iStateManagement->QueryInterface(IID_IOPCAsyncIO2, (void**)&iAsych2IO);
		if (FAILED(result)){
			throw opc_exception( result, OLESTR("Failed to get IID_IOPCAsyncIO2") );
		}
		//*/

		//*
		result = state_mgt_->QueryInterface(IID_IOPCItemMgt, (void**)&item_mgt_);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Failed to get IID_IOPCItemMgt") );
		}
		//*/
	}

	group::~group()
	{
		this->clear();

		try 
		{
			this->remove( true );
			this->disable_callback();
		}		
		catch( ... ) {}
	}

	void group::enable_callback_unk(void * unk_ptr)
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"group(%s) enable_callback()\n", name_.c_str() );
#endif

		IUnknown * ptr = (IUnknown*)unk_ptr;
		IOPCDataCallback *callback_ptr = NULL;

		if ( NULL != ptr )
		{
			HRESULT res = ptr->QueryInterface( IID_IOPCDataCallback, (VOID**)&callback_ptr );
			if ( FAILED(res) )
			{
				throw opc::opc_exception( res, OLESTR("ptr->QueryInterface( IID_IOPCDataCallback ) failed!") );
			}

			data_callback_ = callback_ptr;

			m_dwAdvise = 0;
			res = ATL::AtlAdvise(item_mgt_, callback_ptr, IID_IOPCDataCallback, &m_dwAdvise);

			if ( FAILED(res) )
				throw opc_exception( res, OLESTR("AtlAdvise failed!") );
		}
		else 
		{
			throw opc::opc_exception( E_INVALIDARG, OLESTR("Invalid ptr!") );
		}
	}

	//*
	void group::enable_callback(item_data_changed * ptr)
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"group(%s) enable_callback()\n", name_.c_str() );
#endif

		m_dwAdvise = 0;
		data_callback * dc = new data_callback( *this );
		dc->advise();

		data_callback_ = dc;

		data_changed_ptr_ = ptr;
	}
	//*/

	void group::item_changed( 
		DWORD transaction, OPCHANDLE client, 
		const VARIANT& v, const FILETIME& ft, const WORD qual
		)
	{
		if ( data_changed_ptr_ != NULL )
		{
			item * it = reinterpret_cast<item *>(client);

			if ( it != NULL )
			{
				data_changed_ptr_->data_changed( it, item::data( ft, qual, v, S_OK ) );
			}
		}		
	}

	void group::disable_callback()
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"group(%s) disable_callback()\n", name_.c_str() );
#endif

		if ( m_dwAdvise != 0 )
		{
			HRESULT res = ATL::AtlUnadvise(item_mgt_, IID_IOPCDataCallback, m_dwAdvise);

			//*
			if ( FAILED(res) )
				throw opc_exception( res, OLESTR("AtlUnadvise failed!") );
			//*/
			m_dwAdvise = 0;
		}

		data_callback_.Release();
		data_changed_ptr_ = NULL;
	}

	void group::remove( bool force )
	{
		server_ptr_->RemoveGroup( handle_, force ? TRUE : FALSE );
	}

	item * group::add_item( LPCWSTR name, bool active )
	{
#if defined _DEBUG && defined _CONSOLE
		wprintf( L"group::add_item( %s, %s )\n", name, (active ? L"active" : L"inactive" ) );
#endif

		item * it = new item( name, active, *this );

		items_.push_back( it );
		return it;
	}

	void group::clear()
	{
		std::list< item * >::iterator it = items_.begin();
		for( ; it != items_.end(); ++it )
		{
			delete (*it);
		}

		items_.clear();
	}
}
