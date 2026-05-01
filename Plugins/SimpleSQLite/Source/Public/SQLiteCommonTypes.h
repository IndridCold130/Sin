/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

#pragma once

#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "SQLiteCommonTypes.generated.h"


/**
 *
 */
UENUM(BlueprintType)
enum class ESQLiteReturnInfoType :uint8
{
	/**	Successful result */
	SQLITE_OK = 0 UMETA(DisplayName = "SQLITE_OK"),
	/**	SQL error or missing database */
	SQLITE_ERROR = 1 UMETA(DisplayName = "SQLITE_ERROR"),
	/**	Internal logic error in SQLite */
	SQLITE_INTERNAL = 2 UMETA(DisplayName = "SQLITE_INTERNAL"),
	/**	Access permission denied */
	SQLITE_PERM = 3 UMETA(DisplayName = "SQLITE_PERM"),
	/**	Callback routine requested an abort */
	SQLITE_ABORT = 4 UMETA(DisplayName = "SQLITE_ABORT"),
	/**	The database file is locked */
	SQLITE_BUSY = 5 UMETA(DisplayName = "SQLITE_BUSY"),
	/**	A table in the database is locked */
	SQLITE_LOCKED = 6 UMETA(DisplayName = "SQLITE_LOCKED"),
	/**	A malloc() failed */
	SQLITE_NOMEM = 7 UMETA(DisplayName = "SQLITE_NOMEM"),
	/**	Attempt to write a readonly database */
	SQLITE_READONLY = 8 UMETA(DisplayName = "SQLITE_READONLY"),
	/**	Operation terminated by sqlite3_interrupt() */
	SQLITE_INTERRUPT = 9 UMETA(DisplayName = "SQLITE_INTERRUPT"),
	/**	Some kind of disk I/O error occurred */
	SQLITE_IOERR = 10 UMETA(DisplayName = "SQLITE_IOERR"),
	/**	The database disk image is malformed */
	SQLITE_CORRUPT = 11 UMETA(DisplayName = "SQLITE_CORRUPT"),
	/**	Unknown opcode in sqlite3_file_control() */
	SQLITE_NOTFOUND = 12 UMETA(DisplayName = "SQLITE_NOTFOUND"),
	/**	Insertion failed because database is full */
	SQLITE_FULL = 13 UMETA(DisplayName = "SQLITE_FULL"),
	/**	Unable to open the database file */
	SQLITE_CANTOPEN = 14 UMETA(DisplayName = "SQLITE_CANTOPEN"),
	/**	Database lock protocol error */
	SQLITE_PROTOCOL = 15 UMETA(DisplayName = "SQLITE_PROTOCOL"),
	/**	Database is empty */
	SQLITE_EMPTY = 16 UMETA(DisplayName = "SQLITE_EMPTY"),
	/**	The database schema changed */
	SQLITE_SCHEMA = 17 UMETA(DisplayName = "SQLITE_SCHEMA"),
	/**	String or BLOB exceeds size limit */
	SQLITE_TOOBIG = 18 UMETA(DisplayName = "SQLITE_TOOBIG"),
	/**	Abort due to constraint violation */
	SQLITE_CONSTRAINT = 19 UMETA(DisplayName = "SQLITE_CONSTRAINT"),
	/**	Data type mismatch */
	SQLITE_MISMATCH = 20 UMETA(DisplayName = "SQLITE_MISMATCH"),
	/**	Library used incorrectly */
	SQLITE_MISUSE = 21 UMETA(DisplayName = "SQLITE_MISUSE"),
	/**	Uses OS features not supported on host */
	SQLITE_NOLFS = 22 UMETA(DisplayName = "SQLITE_NOLFS"),
	/**	Authorization denied */
	SQLITE_AUTH = 23 UMETA(DisplayName = "SQLITE_AUTH"),
	/**	Auxiliary database format error */
	SQLITE_FORMAT = 24 UMETA(DisplayName = "SQLITE_FORMAT"),
	/**	2nd parameter to sqlite3_bind out of range */
	SQLITE_RANGE = 25 UMETA(DisplayName = "SQLITE_RANGE"),
	/**	File opened that is not a database file */
	SQLITE_NOTADB = 26 UMETA(DisplayName = "SQLITE_NOTADB"),
	/**	Notifications from sqlite3_log() */
	SQLITE_NOTICE = 27 UMETA(DisplayName = "SQLITE_NOTICE"),
	/**	Warnings from sqlite3_log() */
	SQLITE_WARNING = 28 UMETA(DisplayName = "SQLITE_WARNING"),
	/**	sqlite3_step() has another row ready */
	SQLITE_ROW = 100 UMETA(DisplayName = "SQLITE_ROW"),
	/**	sqlite3_step() has finished executing */
	SQLITE_DONE = 101 UMETA(DisplayName = "SQLITE_DONE")

};

/**
 *	Type of SQLite Database data.
 */
UENUM(BlueprintType)
enum class ESQLiteDataType :uint8
{
	/**	SQLite Data Type:Unkown */
	SQLITE_DT_UNKOWN UMETA(DisplayName = "UNKOWN"),
	/**	SQLite Data Type:float */
	SQLITE_DT_FLOAT UMETA(DisplayName = "FLOAT"),
	/**	SQLite Data Type:int */
	SQLITE_DT_INT UMETA(DisplayName = "INT"),
	/**	SQLite Data Type:string */
	SQLITE_DT_STRING UMETA(DisplayName = "STRING")
};


/**
* This struct holds info relating to a column.  Specifically, we need to get back
* certain meta info from a RecordSet so we can "Get" data from it.
*/
USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FSQLiteColumnInfo
{
public:
	GENERATED_USTRUCT_BODY()

public:
	/** The name of the column **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleSQLite|SQLiteColumnInfo")
	FString ColumnName;

	/** This is the type of data in this column.  (e.g. so you can do GetFloat or GetInt on the column **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleSQLite|SQLiteColumnInfo")
	ESQLiteDataType DataType;
public:
	/** Default constructor **/
	FSQLiteColumnInfo() : DataType(ESQLiteDataType::SQLITE_DT_UNKOWN) 
	{

	}
};

/**
 * 
 */
USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FSQLiteReturnInfo
{
public:
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleSQLite|SQLiteReturnInfo")
	ESQLiteReturnInfoType ReturnInfoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleSQLite|SQLiteReturnInfo")
	FString Description;

public:
	FSQLiteReturnInfo():ReturnInfoType(ESQLiteReturnInfoType::SQLITE_OK)
	{

	}
};


USTRUCT(BlueprintType)
struct FShopItem :public FTableRowBase
{
	GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SimpleSQLite|SQLiteReturnInfo")
		FText Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SimpleSQLite|SQLiteReturnInfo")
		FText Icon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SimpleSQLite|SQLiteReturnInfo")
		float Price;
};
