# Microsoft Developer Studio Project File - Name="sitemon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sitemon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sitemon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sitemon.mak" CFG="sitemon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sitemon - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sitemon - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sitemon - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "CURL_STATICLIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ssleay32.lib libeay32.lib libs\libcurl.lib ws2_32.lib libs\zlib.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "sitemon - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "CURL_STATICLIB" /FR /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib ssleay32.lib libeay32.lib libs\libcurld.lib ws2_32.lib libs\zlibd.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sitemon - Win32 Release"
# Name "sitemon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\component_downloader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\config.cpp
# End Source File
# Begin Source File

SOURCE=.\src\debug_settings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\event.cpp
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\hit_load_request_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\hit_test_engine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\html_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_engine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_form_generator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_request.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_response.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_db_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_html_formatters.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_request.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_request_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_responses.cpp
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\load_test_results_saver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\profile_load_request_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\profile_test_engine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduled_results_saver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler_db_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler_test_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\script.cpp
# End Source File
# Begin Source File

SOURCE=.\src\script_debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\src\script_result.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sitemon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite3.c
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite_db.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite_query.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\string_helper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\thread_pool.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\time.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\tinyxmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\uri_helper.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\component_downloader.h
# End Source File
# Begin Source File

SOURCE=.\src\config.h
# End Source File
# Begin Source File

SOURCE=.\src\debug_settings.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\event.h
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\hit_load_request_thread.h
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\hit_test_engine.h
# End Source File
# Begin Source File

SOURCE=.\src\html_parser.h
# End Source File
# Begin Source File

SOURCE=.\src\http_engine.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_form_generator.h
# End Source File
# Begin Source File

SOURCE=.\src\http_request.h
# End Source File
# Begin Source File

SOURCE=.\src\http_response.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_db_helpers.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_html_formatters.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_request.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_request_thread.h
# End Source File
# Begin Source File

SOURCE=.\src\http_server\http_server_responses.h
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\load_test_results_saver.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\misc.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\mutex.h
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\profile_load_request_thread.h
# End Source File
# Begin Source File

SOURCE=.\src\load_testing\profile_test_engine.h
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduled_results_saver.h
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler.h
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler_db_helpers.h
# End Source File
# Begin Source File

SOURCE=.\src\scheduler\scheduler_test_thread.h
# End Source File
# Begin Source File

SOURCE=.\src\script.h
# End Source File
# Begin Source File

SOURCE=.\src\script_debugger.h
# End Source File
# Begin Source File

SOURCE=.\src\script_result.h
# End Source File
# Begin Source File

SOURCE=.\src\sitemon.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\socket.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite3.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite_db.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\sqlite_query.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\string_helper.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\thread.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\thread_pool.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\time.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\src\uri_helper.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
