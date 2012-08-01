#include "opc_memory.hpp"
#include "opc_item.hpp"
#include "opc_exception.hpp"

namespace opc 
{
	item::item( LPCWSTR name, bool active, group& g )
		: owner_(g)
		, name_( lstrlenW( name ) + 1 )
	{		
		lstrcpynW( &name_[0], name, static_cast<int>( name_.size() ) );

		client_ = OPCHANDLE( intptr_t( this ) );

		OPCITEMDEF itemDef;
		itemDef.szItemID = &name_[0];
		itemDef.szAccessPath = NULL;
		itemDef.bActive = active ? TRUE : FALSE;
		itemDef.hClient = client_;
		itemDef.dwBlobSize = 0;
		itemDef.pBlob = NULL;
		itemDef.vtRequestedDataType = VT_EMPTY;

		cotask_holder< HRESULT > itemResult;
		cotask_holder< OPCITEMRESULT > itemDetails;
		HRESULT	result = owner_.item_mgt()->AddItems( 1, &itemDef, itemDetails.addr(), itemResult.addr() );
		if (FAILED(result))
		{
#if defined _DEBUG && defined _CONSOLE
			wprintf( L"item::item( %s ) failed! HR = 0x%x\n", name, result );
#endif
			throw opc_exception( result, OLESTR("Failed to add item") );
		}

#if defined _DEBUG && defined _CONSOLE
		wprintf( L"item::item( %s ) created! hClient = 0x%X\n", name, itemDef.hClient );
#endif

		cotask_holder< BYTE > blobHolder( itemDetails[0].pBlob );

		if ( FAILED(itemResult[0]) )
		{
			throw opc_exception( result, OLESTR("Failed to add item") );
		}

		handle_			= itemDetails[0].hServer;
		data_type_		= itemDetails[0].vtCanonicalDataType;
		access_rights_	= itemDetails[0].dwAccessRights;
	}

	item::data::data(const FILETIME& time, WORD qual, VARIANT val, HRESULT err)
	{
		vDataValue.vt = VT_EMPTY;
		HRESULT result = VariantCopy( &vDataValue, &val);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("VarCopy failed") );
		}

		ftTimeStamp = time;
		wQuality = qual;
		error = err;
	}

	void item::data::set(OPCITEMSTATE &itemState)
	{
		HRESULT result = VariantCopy( &vDataValue, &itemState.vDataValue);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("VarCopy failed") );
		}

		ftTimeStamp = itemState.ftTimeStamp;
		wQuality = itemState.wQuality;
	}

	void item::data::set(FILETIME time, WORD qual, VARIANT & val)
	{
		HRESULT result = VariantCopy( &vDataValue, &val);
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("VarCopy failed") );
		}

		ftTimeStamp = time;
		wQuality = qual;
	}

	void item::read( data& v, OPCDATASOURCE source )
	{
		cotask_holder<HRESULT> itemResult;
		cotask_holder<OPCITEMSTATE> itemState;

		HRESULT	result = owner_.sync_io()->Read(source, 1, &handle_, itemState.addr(), itemResult.addr());
		if (FAILED(result))
		{
			throw opc_exception( result, OLESTR("Read failed") );
		} 

		if (FAILED(itemResult[0]))
		{
			throw opc_exception( itemResult[0], OLESTR("Read failed") );
		}

		v.set(itemState[0]);

		VariantClear(&itemState[0].vDataValue);
	}
}
