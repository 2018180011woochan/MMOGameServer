#include "DBConnectionPool.h"
#include "DBBind.h"
#include <string>

struct AccountDBData
{
	int32 accountId = 0;
};

struct ItemDBData
{
	int32 itemId = 0;
	int32 slotIndex = 0;
};

class DBManager
{
public:
	static DBManager* Instance()
	{
		static DBManager instance;
		return &instance;
	}

public:
	// 계정
	bool LoginAccount(const std::wstring& accountName, const std::wstring& password, OUT AccountDBData& outAccountData);
	// 인벤토리
	bool InsertInventoryItem(int32 accountId, int32 itemId, int32 slotIndex);
	bool DeleteInventoryItem(int32 accountId, int32 slotIndex);
	bool LoadInventory(int32 accountId, OUT std::vector<ItemDBData>& outInventory);

private:
	DBManager() {}
	DBManager(const DBManager&) = delete;
	DBManager& operator=(const DBManager&) = delete;
};

#define GDBManager DBManager::Instance()