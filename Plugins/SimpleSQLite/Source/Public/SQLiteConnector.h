/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

#pragma once

#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"
#include "SQLiteDatabaseConnection.h"
#include "SQLiteCommonTypes.h"
#include "SQLiteConnector.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_SQLiteConnector, Log, All);

/**
 * 
 */
UCLASS(ClassGroup = "SimpleSQLite|SQLiteConnector", hideCategories = (Input, Rendering), BlueprintType, Blueprintable)
class USQLiteConnector : public UObject//, public TSharedFromThis<USQLiteResultor>
{
	GENERATED_BODY()

public:

	USQLiteConnector(const FObjectInitializer& ObjectInitializer);

	virtual void BeginDestroy() override;

public:
	USQLiteResultor* Resultor;

public:
	/**
	 *	Open a SQLite file(The root path is under 'Content' of your project)
	 *	@param	ConnectionString	Path to the file that should be opened
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open Database", Keywords = "SQLite Open"), Category = "SimpleSQLite|SQLiteConnector")
	void Open(const FString& DBFilePath, FSQLiteReturnInfo& ReturnInfo);

	/** Closes the database handle and unlocks the file */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Database", Keywords = "SQLite Close"), Category = "SimpleSQLite|SQLiteConnector")
	void Close();

	/** Execute a command on the database without storing the result set (like insert, update..) */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute(Query Only)", Keywords = "SQLite Execute No RecordSet"), Category = "SimpleSQLite|SQLiteConnector")
	void ExecuteNoRecordSet(const FString& CommandString, FSQLiteReturnInfo& ReturnInfo);

	/** Executes the command string on the currently opened database, returning a SQLiteResultor.*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute(With RecordSet)", Keywords = "SQLite Execute With RecordSet"), Category = "SimpleSQLite|SQLiteConnector")
	void ExecuteWithRecordSet(const FString& CommandString, USQLiteResultor*& NewSQLiteResultor, FSQLiteReturnInfo& ReturnInfo);

	/** SQLiteConnector gets last error info.*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetLastErrorInfo", Keywords = "SQLite GetLastErrorInfo"), Category = "SimpleSQLite|SQLiteConnector")
	void GetLastErrorInfo(FSQLiteReturnInfo& ReturnInfo);

private:
	void UpdateReturnInfo(FSQLiteReturnInfo& ReturnInfo);

private:
	TSharedPtr<FSQLiteDatabaseConnection> SQLiteDatabaseConnection;
	
};
