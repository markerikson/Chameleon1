# Microsoft Developer Studio Project File - Name="networktesting" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=networktesting - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "networktesting.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "networktesting.mak" CFG="networktesting - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "networktesting - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "networktesting - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "networktesting - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\wxWindows-2.5.1\include" /I "c:\wxWindows-2.5.1\lib\vc_lib\msw" /I "H:\chameleon\gears_chameleon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "c:\wxWindows-2.5.1\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxregex.lib wxbase25.lib wxmsw25_core.lib /nologo /subsystem:windows /machine:I386 /libpath:"c:\wxWindows-2.5.1\lib\vc_lib"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\wxWindows-2.5.1\include" /I "c:\wxWindows-2.5.1\lib\vc_lib\mswd" /I "H:\chameleon\gears_chameleon" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\wxWindows-2.5.1\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxregexd.lib wxbase25d.lib wxmsw25d_core.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"c:\wxWindows-2.5.1\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "networktesting - Win32 Release"
# Name "networktesting - Win32 Debug"
# Begin Group "Source/Header Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\gears_chameleon\common\chameleonprocessevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\chameleonprocessevent.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\datastructures.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\debug.h
# End Source File
# Begin Source File

SOURCE=".\main-network.cpp"
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\networking.cpp

!IF  "$(CFG)" == "networktesting - Win32 Release"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\networking.h

!IF  "$(CFG)" == "networktesting - Win32 Release"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\plinkconnect.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\plinkconnect.h
# End Source File
# End Group
# Begin Group "reference"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\wxWindows-2.5.1\src\common\event.cpp"

!IF  "$(CFG)" == "networktesting - Win32 Release"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="C:\wxWindows-2.5.1\include\wx\event.h"

!IF  "$(CFG)" == "networktesting - Win32 Release"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="C:\wxWindows-2.5.1\contrib\include\wx\stc\stc.h"

!IF  "$(CFG)" == "networktesting - Win32 Release"

!ELSEIF  "$(CFG)" == "networktesting - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
