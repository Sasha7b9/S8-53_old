@echo off

@echo Rebuild S8-53

c:\Keil_v5\UV4\UV4 -c sources\Device53\Device53.uvprojx -j0
c:\Keil_v5\UV4\UV4 -c sources\Loader53\Loader53.uvprojx -j0

call build53.bat
