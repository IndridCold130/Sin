/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

#include "SQLiteResultor.h"

#define LOCTEXT_NAMESPACE "SQLiteResultor"
DEFINE_LOG_CATEGORY(Log_SQLiteResultor);

USQLiteResultor::USQLiteResultor(const FObjectInitializer& ObjectInitializer)
{
	
}

void USQLiteResultor::Init(TSharedPtr<FSQLiteResultSet> InSQLiteResultSet)
{
	SQLiteResultSet = InSQLiteResultSet;
}
void USQLiteResultor::Clean()
{
	if (SQLiteResultSet.IsValid())
	{
		SQLiteResultSet.Reset();
	}
}
void USQLiteResultor::BeginDestroy()
{
	Super::BeginDestroy();
}


void USQLiteResultor::GetRecordCount(int32& RecordCount)
{
	RecordCount = SQLiteResultSet->GetRecordCount();
}

void USQLiteResultor::GetString(int32 rowId, const FString& Column, FString& Value)
{
	int id = 0;
	for (FSQLiteResultSet::TIterator i(SQLiteResultSet.Get()); i; ++i)
	{
		if (rowId == id)
		{
			for (FDatabaseColumnInfo column : i->GetColumnNames())
			{
				if (column.ColumnName == Column)
				{
					Value = SQLiteResultSet->GetString(*Column);
					return;
				}
			}
		}
		id++;
	}
}

void USQLiteResultor::GetInt(int32 rowId, const FString& Column, int32& Value)
{
	int id = 0;
	for (FSQLiteResultSet::TIterator i(SQLiteResultSet.Get()); i; ++i)
	{
		if (rowId == id)
		{
			for (FDatabaseColumnInfo column : i->GetColumnNames())
			{
				if (column.ColumnName == Column)
				{
					Value = SQLiteResultSet->GetInt(*Column);
					return;
				}
			}
		}
		id++;
	}
}

void USQLiteResultor::GetFloat(int32 rowId, const FString& Column, float& Value)
{
	int id = 0;
	for (FSQLiteResultSet::TIterator i(SQLiteResultSet.Get()); i; ++i)
	{
		if (rowId == id)
		{
			for (FDatabaseColumnInfo column : i->GetColumnNames())
			{
				if (column.ColumnName == Column)
				{
					Value = SQLiteResultSet->GetFloat(*Column);
					return;
				}
			}
		}
		id++;
	}
}

void USQLiteResultor::GetBigInt(int32 rowId, const FString& Column, int32& Value)
{
	int id = 0;
	for (FSQLiteResultSet::TIterator i(SQLiteResultSet.Get()); i; ++i)
	{
		if (rowId == id)
		{
			for (FDatabaseColumnInfo column : i->GetColumnNames())
			{
				if (column.ColumnName == Column)
				{
					Value = SQLiteResultSet->GetBigInt(*Column);
					return;
				}
			}
		}
		id++;
	}
}

void USQLiteResultor::GetColumnInfos(TArray<FSQLiteColumnInfo>& ColumnInfos)
{
	TArray<FDatabaseColumnInfo> ColumnInfos_Temp = SQLiteResultSet->GetColumnNames();
	for (int32 i = 0; i < ColumnInfos_Temp.Num(); ++i)
	{
		FSQLiteColumnInfo SQLiteColumnInfo;
		SQLiteColumnInfo.ColumnName = ColumnInfos_Temp[i].ColumnName;
		SQLiteColumnInfo.DataType = (ESQLiteDataType)ColumnInfos_Temp[i].DataType;
		ColumnInfos.Add(SQLiteColumnInfo);
	}
}

#undef LOCTEXT_NAMESPACE
