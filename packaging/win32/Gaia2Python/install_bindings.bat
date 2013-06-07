echo Installing gaia python bindings to %1

set DESTDIR=%1
REM set DESTDIR=C:\Python26\Lib\site-packages


REM Copy the necessary bindings files
copy release\Gaia2Python.dll %DESTDIR%\_gaia2.pyd
copy gaia2.py %DESTDIR%\gaia2.py
copy ..\..\src\bindings\gaia2utils.py %DESTDIR%\gaia2utils.py
copy ..\..\src\bindings\fastyaml.py %DESTDIR%\fastyaml.py

REM Copy the required DLLs that the bindings depend on
copy ..\..\..\gaia2-thirdparty\bin\* %DESTDIR%
