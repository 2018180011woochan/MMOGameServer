#include "pch.h"
#include "DBManager.h"

bool DBManager::LoginAccount(const std::string& accountName, const std::string& password, OUT AccountDBData& outAccountData)
{
	DBConnection* dbcon = GDBConnectionPool->Pop();
	if (dbcon == nullptr)
		return false;

	WCHAR query[256];
	::swprintf_s(query, 256, L"SELECT AccountId FROM account WHERE AccountName = '%S' AND Password = '%S'", accountName.c_str(), password.c_str());

	DBBind<0, 1> dbBind(*dbcon, query);

	int32 fetchedAccountId = 0;
	dbBind.BindCol(0, OUT fetchedAccountId); 

	if (dbBind.Execute() == false)
	{
		GDBConnectionPool->Push(dbcon);
		return false;
	}

	bool isSuccess = false;

	if (dbcon->Fetch())
	{
		outAccountData.accountId = fetchedAccountId;
		isSuccess = true;
	}

	GDBConnectionPool->Push(dbcon);

	return isSuccess;
}