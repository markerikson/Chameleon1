# Microsoft Developer Studio Project File - Name="chameleon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=chameleon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chameleon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chameleon.mak" CFG="chameleon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chameleon - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "chameleon - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chameleon - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\wxWindows-2.5.1\include" /I "C:\wxWindows-2.5.1\lib\vc_dll\msw" /I "c:\wxWindows-2.5.1\contrib\include" /D "MSVC6" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D WXDEBUG=0 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "c:\wxWindows-2.5.1\include" /i "c:\wxWindows-2.5.1\lib\msw" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase25.lib wxmsw25_core.lib wxmsw25_stc.lib wxregex.lib wxmsw25_adv.lib wxjpeg.lib wxpng.lib wxtiff.lib wxzlib.lib /nologo /subsystem:windows /machine:I386 /libpath:"c:\wxwindows-2.5.1\lib\vc_lib"

!ELSEIF  "$(CFG)" == "chameleon - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\wxWindows-2.5.1\include" /I "c:\wxWindows-2.5.1\lib\vc_lib\mswd" /I "c:\wxWindows-2.5.1\contrib\include" /D "MSVC6" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\wxWindows-2.5.1\include" /i "c:\wxWindows-2.5.1\lib\mswd" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase25d.lib wxmsw25d_core.lib wxregexd.lib wxjpegd.lib wxpngd.lib wxtiffd.lib wxzlibd.lib wxmsw25d_stc.lib wxmsw25d_adv.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Chameleon-D.exe" /pdbtype:sept /libpath:"c:\wxwindows-2.5.1\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "chameleon - Win32 Release"
# Name "chameleon - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\Chameleon.rc
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\ChameleonPrintout.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\ChameleonPrintout.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\chameleonprocessevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\chameleonprocessevent.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\CommonHeaders.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\Crc16.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\Crc16.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\datastructures.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\debug.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\DebugEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\DebugEvent.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\fixvsbug.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\options.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\Options.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\ProjectInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\common\ProjectInfo.h
# End Source File
# End Group
# Begin Group "compiler"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\compiler\compiler.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\compiler\compiler.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\compiler\compilerevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\compiler\compilerevent.h
# End Source File
# End Group
# Begin Group "debugger"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\debugger\cham_db.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\debugger\cham_db.h
# End Source File
# End Group
# Begin Group "editor"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\editor\editor.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\editor\editor.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\editor\stc.h

!IF  "$(CFG)" == "chameleon - Win32 Release"

!ELSEIF  "$(CFG)" == "chameleon - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Group "dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\AboutChameleonDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\AboutChameleonDialog.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\AddVariableWatch.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\AddVariableWatch.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\CompilerOutputPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\CompilerOutputPanel.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\RemoteFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\RemoteFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\VariableWatchPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\VariableWatchPanel.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\wxTermContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\dialogs\wxTermContainer.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\gears_chameleon\gui\Chameleon.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\ChameleonNotebook.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\ChameleonNotebook.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\ChameleonWindow.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\wxssh.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\wxssh.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\wxterm.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\gui\wxterm.h
# End Source File
# End Group
# Begin Group "network"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\network\actions.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\gtelnet.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\gterm.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\keytrans.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\networking.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\networking.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\plinkconnect.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\plinkconnect.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\states.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\TextManager.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\TextManager.h
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\vt52_states.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\network\winmesgbeep.cpp
# End Source File
# End Group
# Begin Group "perms"

# PROP Default_Filter "cpp;c;cxx;rc;h;hpp"
# Begin Source File

SOURCE=..\gears_chameleon\perms\p.cpp
# End Source File
# Begin Source File

SOURCE=..\gears_chameleon\perms\p.h
# End Source File
# End Group
# End Target
# End Project
