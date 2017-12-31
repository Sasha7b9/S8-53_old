@echo off

@echo Build S8-53-Open

echo Compile Loader53-Open ... %TIME%

c:\Keil_v5\UV4\UV4 -b sources\Loader53\Loader53-Open7XX.uvprojx -j0 -z -o Loader53-Open.out
if ERRORLEVEL 1 goto BadLoader53

echo Compile Device53-Open ... %TIME%

:FineLoader53
c:\Keil_v5\UV4\UV4 -b sources\Device53\Device53-Open7XX.uvprojx -j0 -z -o Device53-Open.out
if ERRORLEVEL 1 goto BadDevice53

:FineDevice53

goto leave

:BadLoader53
echo ERROR!!! Loader53 failed!!!
"c:\Program Files\Notepad++\notepad++.exe" -multiInst -alwaysOnTop sources\Loader53\Loader53-Open.out -n1000 -x0 -y0
goto FineLoader53

:BadDevice53
echo ERROR!!! Device53 failed!!!
"c:\Program Files\Notepad++\notepad++.exe" -multiInst -alwaysOnTop sources\Device53\Device53-Open.out -n1000 -x0 -y0
goto FineDevice53

:leave
