call xcopy /Q /E /Y /I Syndra\vendor\assimp\build\assimp-vc143-mtd.dll bin\Debug-windows-x86_64\Syndra-Editor\
call xcopy /Q /E /Y /I Kaesar\vendor\VulkanSDK\Bin bin\Debug-windows-x86_64\Kaesar-Editor\
call vendor\bin\premake\premake5.exe vs2022
PAUSE