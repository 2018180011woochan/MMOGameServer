#pragma once
#include"DBConnection.h"

template<int32 C>
struct FullBits{ enum{ value = ( 1 << ( C - 1 ) ) | FullBits<C - 1>::value }; };

template<>
struct FullBits<1>{ enum{ value = 1 }; };

template<>
struct FullBits<0>{enum{value = 0}; };

template<int32 ParamCount,int32 ColumnConut>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query)
		:_dbConnection(dbConnection), _query(query)
	{
		::memset(_paramIndex, 0, sizeof(_paramIndex));
		::memset(_columnIndex, 0, sizeof(_columnIndex));
		_paramFlag = 0;
		_columnFlag = 0;
		dbConnection.Unbind();
	}
	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnConut>::value;
	}
	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query);
	}
	bool Fetch()
	{
		return _dbConnection.Fetch();
	}
public:
	template<class T>
	void BindParam(int32 idx, T& value)
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag |= ( 1LL << idx );
	}

	//void BindParam(int32 idx, const WCHAR* value)
	//{
	//	_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
	//	_paramFlag |= ( 1LL << idx );
	//}
	void BindParam(int32 idx, const WCHAR* value)
	{
		_dbConnection.BindParam(idx + 1, value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	// 추가
	void BindParam(int32 idx, const std::wstring& value)
	{
		_dbConnection.BindParam(idx + 1, value.c_str(), &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, std::wstring& value) // non-const 참조
	{
		// 내부 동작은 const 버전과 동일
		_dbConnection.BindParam(idx + 1, value.c_str(), &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<class T, int32 N>
	void BindParam(int32 idx, T(&value)[N])
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= ( 1LL << idx );
	}

	template<class T>
	void BindParam(int32 idx, T* value, int32 N)
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= ( 1LL << idx );
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= ( 1LL << idx );
	}

	template<int32 N>
	void BindCol(int32 idx, WCHAR(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, N - 1, &_columnIndex[idx]);
		_columnFlag |= ( 1LL << idx );
	}

	void BindCol(int32 idx, WCHAR* value, int32 len)
	{
		_dbConnection.BindCol(idx + 1, value, len - 1, &_columnIndex[idx]);
		_columnFlag |= ( 1LL << idx );
	}

	template<typename T, int32 N>
	void BindCol(int32 idx, T(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, size32(T) * N, &_columnIndex[idx]);
		_columnFlag |= ( 1LL << idx );
	}

	template<int32 N>
	void BindCol(int32 idx, char(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, (BYTE*)value, N, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, int32& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, int64& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, float& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, uint32& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, int32& value) // 참조(&)로 받아서
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]); // 주소(&)를 넘겨줍니다.
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, int64& value) // 참조(&)로 받아서
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]); // 주소(&)를 넘겨줍니다.
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, float& value) // 참조(&)로 받아서
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]); // 주소(&)를 넘겨줍니다.
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, const int32& value)
	{
		// DB API가 non-const 포인터를 요구하므로 const_cast로 변환해줍니다.
		_dbConnection.BindParam(idx + 1, const_cast<int32*>(&value), &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, const int64& value)
	{
		_dbConnection.BindParam(idx + 1, const_cast<int64*>(&value), &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, const float& value)
	{
		_dbConnection.BindParam(idx + 1, const_cast<float*>(&value), &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN _paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN _columnIndex[ColumnConut > 0 ? ColumnConut : 1];
	uint64 _paramFlag;
	uint64 _columnFlag;

};
