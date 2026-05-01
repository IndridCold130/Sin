/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

#pragma once

#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"
#include "SQLiteResultSet.h"
#include "SQLiteCommonTypes.h"
#include "SQLiteResultor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_SQLiteResultor, Log, All);

/**
 *	ResultSet Manager for SQLite database queries
 */
UCLASS(ClassGroup = "SimpleSQLite|SQLiteResultor", hideCategories = (Input, Rendering), BlueprintType, Blueprintable)
class USQLiteResultor : public UObject, public TSharedFromThis<USQLiteResultor>
{
	GENERATED_BODY()

public:
	USQLiteResultor(const FObjectInitializer& ObjectInitializer);
	void Init(TSharedPtr<FSQLiteResultSet> InSQLiteResultSet);
	void Clean();
public:
	virtual void BeginDestroy() override;
public:

	/**
	 *   Returns a count of the number of records in the record set
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetRecordCount", Keywords = "SQLiteResultor GetRecordCount"), Category = "SimpleSQLite|SQLiteResultor")
	void GetRecordCount(int32& RecordCount);

	/**
	 *	Returns a string associated with the passed in column name for the assigned row.
	 *	@param Column Name of column to retrieve data in current row
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetString", Keywords = "SQLiteResultor GetString"), Category = "SimpleSQLite|SQLiteResultor")
	void GetString(int32 rowId, const FString& Column, FString& Value);

	/**
	 *	Returns an integer associated with the passed in column name for the assigned row.
	 *	@param Column Name of column to retrieve data in current row
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetInt", Keywords = "SQLiteResultor GetInt"), Category = "SimpleSQLite|SQLiteResultor")
	void GetInt(int32 rowId, const FString& Column, int32& Value);

	/**
	 *	Returns a float associated with the passed in column name for the assigned row.
	 *	@param Column Name of column to retrieve data in current row
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetFloat", Keywords = "SQLiteResultor GetFloat"), Category = "SimpleSQLite|SQLiteResultor")
	void GetFloat(int32 rowId, const FString& Column, float& Value);

	/**
	 *	Returns a int64 associated with the passed in column name for the assigned row.
	 *	@param Column Name of column to retrieve data in current row
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetBigInt", Keywords = "SQLiteResultor GetBigInt"), Category = "SimpleSQLite|SQLiteResultor")
	void GetBigInt(int32 rowId, const FString& Column, int32& Value);

	/**
	 *	Returns the set of column infos(Containing: column name, column data type) for this Recordset.	This is useful for determining
	 *	what you can actually ask the record set for without having to hard code those ahead of time.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetColumnInfos", Keywords = "SQLiteResultor GetColumnInfos"), Category = "SimpleSQLite|SQLiteResultor")
	void GetColumnInfos(TArray<FSQLiteColumnInfo>& ColumnInfos);


private:
	TSharedPtr<FSQLiteResultSet> SQLiteResultSet;
};
