@echo off

copy 3rdParty\libcurl\bin\*.dll ���ɹ�����
copy 3rdParty\libcurl\bin\curl.exe ���ɹ�����
copy 3rdParty\tinyxml2\bin\Release\*.dll ���ɹ�����
copy 3rdParty\opencv\bin\Release\*.dll ���ɹ�����
copy 3rdParty\7z\7za.exe ���ɹ�����

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
