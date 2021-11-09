@echo off

copy 3rdParty\libcurl\bin\*.dll 贴吧管理器
copy 3rdParty\libcurl\bin\curl.exe 贴吧管理器
copy 3rdParty\tinyxml2\bin\Release\*.dll 贴吧管理器
copy 3rdParty\opencv\bin\Release\*.dll 贴吧管理器
copy 3rdParty\7z\7za.exe 贴吧管理器

md Debug
copy 3rdParty\libcurl\bin\*.dll Debug
copy 3rdParty\libcurl\bin\curl.exe Debug
copy 3rdParty\tinyxml2\bin\Debug\*.dll Debug
copy 3rdParty\opencv\bin\Debug\*.dll Debug
copy 3rdParty\7z\7za.exe Debug

md Release
copy 3rdParty\libcurl\bin\*.dll Release
copy 3rdParty\libcurl\bin\curl.exe Release
copy 3rdParty\tinyxml2\bin\Release\*.dll Release
copy 3rdParty\opencv\bin\Release\*.dll Release
copy 3rdParty\7z\7za.exe Release

pause
