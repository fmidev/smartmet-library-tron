@REM
@REM make.cmd [debug|release] [Boost_path]
@REM
@REM Requires:
@REM     SCons       installed and available ('scons -v' works)
@REM     VC++ 2008   command line tools available (run 'vcvars32.bat')
@REM     Boost 1.36  available at D:\Boost\1_36_0 (installed by BoostPro Computing installer)
@REM
@if "%1"=="" %0 debug
@if "%2"=="" %0 %1 D:\Boost\boost_1_36_0

@if not "%WindowsSdkDir%"=="" goto VC_OK
@echo.
@echo *** Run '%VC90COMNTOOLS%\vcvars32.bat'
@echo *** to set up Visual C++ 2008 command line tools.
@echo.
@goto EXIT

:VC_OK
@REM 'objdir' on local disk to speed up compilation if using a remote file system
@REM (s.a. ssh mapped to G: via SFtpDrive)
@REM
scons %1=1 windows_boost_path=%2 smartmet_tron_%1.lib objdir=%TEMP%\tron-%1

:EXIT
