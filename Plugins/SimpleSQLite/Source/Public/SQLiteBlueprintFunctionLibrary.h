/*
**	Copyright (c) Code By PatrolCat.
**	All rights reserved.
**	@ Date : 2021/10/20
**  Demo Video:https://www.bilibili.com/video/BV1qr4y117L4
*/

/** References:
 *  [1]: Packaged Game Paths, Obtain Directories Based on Executable Location
 *  (https://wiki.unrealengine.com/Packaged_Game_Paths,_Obtain_Directories_Based_on_Executable_Location )
 *  [2]: Using SQLite with Unreal (on iOS)
 *  (http://www.casualdistractiongames.com/single-post/2016/06/03/Using-SQLite-with-Unreal-on-iOS )
 *
 */

#pragma once
#include "Engine/Engine.h"
#include "SQLiteConnector.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SQLiteBlueprintFunctionLibrary.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(Log_SQLiteBlueprintFunctionLibrary, Log, All);


UCLASS()
class USQLiteBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CreateSQLiteConnector"), Category = "SimpleSQLite|SQLiteBlueprintFunctionLibrary")
		static void CreateSQLiteConnector(USQLiteConnector*& NewSQLiteConnector);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "TestSQLiteConnect"), Category = "SimpleSQLite|SQLiteBlueprintFunctionLibrary")
		static bool TestSQLiteConnect(FString path, TArray<FShopItem>& Items);

	static FString GetDBFullPath(FString path);

};
