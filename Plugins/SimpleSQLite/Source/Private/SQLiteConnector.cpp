/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

#include "SQLiteConnector.h"
#include "SQLiteResultor.h"
#include "SQLiteBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "SQLiteConnector"
DEFINE_LOG_CATEGORY(Log_SQLiteConnector);

USQLiteConnector::USQLiteConnector(const FObjectInitializer& ObjectInitializer)
{
	SQLiteDatabaseConnection = MakeShareable(new FSQLiteDatabaseConnection);
}


void USQLiteConnector::BeginDestroy()
{
	Super::BeginDestroy();
	this->Close();
}

void USQLiteConnector::Open(const FString& DBFilePath, FSQLiteReturnInfo& ReturnInfo)
{
	FString fullpath = USQLiteBlueprintFunctionLibrary::GetDBFullPath(DBFilePath);
	bool SQLiteStatus = SQLiteDatabaseConnection->Open(*fullpath, nullptr, nullptr);
	UpdateReturnInfo(ReturnInfo);
}


void USQLiteConnector::Close()
{
	if (Resultor != nullptr)
	{
		Resultor->Clean();
		Resultor = nullptr;
	}
	SQLiteDatabaseConnection->Close();
}

void USQLiteConnector::ExecuteNoRecordSet(const FString& CommandString, FSQLiteReturnInfo& ReturnInfo)
{
	SQLiteDatabaseConnection->Execute(*CommandString);
	UpdateReturnInfo(ReturnInfo);
}

void USQLiteConnector::ExecuteWithRecordSet(const FString& CommandString, USQLiteResultor*& NewSQLiteResultor, FSQLiteReturnInfo& ReturnInfo)
{
	FSQLiteResultSet* RecordSet = nullptr;
	if (SQLiteDatabaseConnection->Execute(*CommandString, RecordSet) && RecordSet != nullptr)
	{
		NewSQLiteResultor = NewObject<USQLiteResultor>();
		NewSQLiteResultor->Init(MakeShareable(RecordSet));
		Resultor = NewSQLiteResultor;
	}
	UpdateReturnInfo(ReturnInfo);

}

void USQLiteConnector::GetLastErrorInfo(FSQLiteReturnInfo& ReturnInfo)
{
	UpdateReturnInfo(ReturnInfo);
}

void USQLiteConnector::UpdateReturnInfo(FSQLiteReturnInfo& ReturnInfo)
{
	if (!SQLiteDatabaseConnection.IsValid())
	{
		ReturnInfo.ReturnInfoType = ESQLiteReturnInfoType::SQLITE_ERROR;
		ReturnInfo.Description = "SQLiteDatabaseConnection is invalid";
	}
	//if (SQLiteDatabaseConnection->DbHandleIsValid()) //��Ҫһ��������ʹ��FSQLiteDatabaseConnection��ı�����ԱDatabase
	{
		//ReturnInfo.ReturnInfoType = (ESQLiteReturnInfoType)SQLiteDatabaseConnection->GetLastErrorCode();
		ReturnInfo.Description = SQLiteDatabaseConnection->GetLastError();
	}
	
}

#undef LOCTEXT_NAMESPACE
