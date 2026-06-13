// 명령어				옵션			원본파일이 있는 위치			사본파일을 저장할 위치
xcopy					/y				.\Engine\public\*.*				.\Library\public
xcopy					/y/s				.\Engine\3rdParty\*.*				.\Library\public
xcopy					/y				.\Engine\Bin\*.dll				.\Client\Bin
xcopy					/y/s				.\Engine\Bin\*.lib				.\Library\Lib
xcopy					/y/s				.\Engine\Lib\*.lib				.\Library\Lib
xcopy					/y				.\Engine\3rdParty\json\*.hpp			.\Library\public
xcopy				/y			.\Engine\Bin\ShaderFiles\*.*	.\Client\Bin\ShaderFiles\