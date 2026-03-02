#include "DBConnectionPool.h"
#include "DBBind.h"
#include <string>

struct AccountDBData
{
	int32 accountId = 0;
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
	bool LoginAccount(const std::string& accountName, const std::string& password, OUT AccountDBData& outAccountData);

private:
	DBManager() {}
	DBManager(const DBManager&) = delete;
	DBManager& operator=(const DBManager&) = delete;
};

#define GDBManager DBManager::Instance()