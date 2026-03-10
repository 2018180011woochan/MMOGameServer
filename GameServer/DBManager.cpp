#include "pch.h"
#include "DBManager.h"

bool DBManager::LoginAccount(const std::wstring& accountName, const std::wstring& password, OUT AccountDBData& outAccountData)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr) return false;
	WCHAR query[256] = L"{CALL sp_LoginAccount(?, ?)}";
	DBBind<2, 1> dbBind(*dbcon, query);
	dbBind.BindParam(0, accountName.c_str());
	dbBind.BindParam(1, password.c_str());
	int32 fetchedAccountId = 0;

	dbBind.BindCol(0, OUT fetchedAccountId);
	bool isSuccess = false;
	if (dbBind.Execute() == true) {
		if (dbcon->Fetch()) {
			outAccountData.accountId = fetchedAccountId;
			isSuccess = true;
		}
	}

	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::InsertInventoryItem(int32 accountId, int32 itemId, int32 slotIndex)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256] = L"{CALL sp_InsertInventoryItem(?, ?, ?)}";
	DBBind<3, 0> dbBind(*dbcon, query);

	dbBind.BindParam(0, accountId);
	dbBind.BindParam(1, itemId);
	dbBind.BindParam(2, slotIndex);

	bool isSuccess = dbBind.Execute();

	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::DeleteInventoryItem(int32 accountId, int32 slotIndex)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256] = L"{CALL sp_DeleteInventoryItem(?, ?)}";
	DBBind<2, 0> dbBind(*dbcon, query);

	dbBind.BindParam(0, accountId);
	dbBind.BindParam(1, slotIndex);

	bool isSuccess = dbBind.Execute();

	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::LoadInventory(int32 accountId, OUT std::vector<ItemDBData>& outInventory)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256] = L"{CALL sp_LoadInventory(?)}";
	DBBind<1, 2> dbBind(*dbcon, query);

	dbBind.BindParam(0, accountId);

	int32 fetchedItemId = 0;
	int32 fetchedSlotIndex = 0;

	dbBind.BindCol(0, OUT fetchedItemId);
	dbBind.BindCol(1, OUT fetchedSlotIndex);

	if (dbBind.Execute() == false)
	{
		GDBConnectionPool->Push(dbcon);
		return false;
	}

	while (dbcon->Fetch())
	{
		ItemDBData item;
		item.itemId = fetchedItemId;
		item.slotIndex = fetchedSlotIndex;

		outInventory.push_back(item);
	}

	GDBConnectionPool->Push(dbcon);
	return true;
}
