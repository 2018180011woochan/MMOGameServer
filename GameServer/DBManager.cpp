#include "pch.h"
#include "DBManager.h"

bool DBManager::LoginAccount(const std::string& accountName, const std::string& password, OUT AccountDBData& outAccountData)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr) return false;
	WCHAR query[256];
	::swprintf_s(query, 256, L"CALL sp_LoginAccount('%S', '%S')",
		accountName.c_str(), password.c_str());
	DBBind<0, 1> dbBind(*dbcon, query);
	int32 fetchedAccountId = 0;
	dbBind.BindCol(0, OUT fetchedAccountId); 
	if (dbBind.Execute() == false) {
		GDBConnectionPool->Push(dbcon);
		return false;
	}
	bool isSuccess = false;
	if (dbcon->Fetch()) {
		outAccountData.accountId = fetchedAccountId;
		isSuccess = true;
	}
	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::InsertInventoryItem(int32 accountId, int32 itemId, int32 slotIndex)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256];
	::swprintf_s(query, 256, L"CALL sp_InsertInventoryItem(%d, %d, %d)", accountId, itemId, slotIndex);

	DBBind<0, 0> dbBind(*dbcon, query);
	bool isSuccess = dbBind.Execute();

	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::DeleteInventoryItem(int32 accountId, int32 slotIndex)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256];
	::swprintf_s(query, 256, L"CALL sp_DeleteInventoryItem(%d, %d)", accountId, slotIndex);

	DBBind<0, 0> dbBind(*dbcon, query);
	bool isSuccess = dbBind.Execute();

	GDBConnectionPool->Push(dbcon);
	return isSuccess;
}

bool DBManager::LoadInventory(int32 accountId, OUT std::vector<ItemDBData>& outInventory)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256];
	::swprintf_s(query, 256, L"CALL sp_LoadInventory(%d)", accountId);

	DBBind<0, 2> dbBind(*dbcon, query);

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
