# Microsoft Developer Studio Project File - Name="crypto_test_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=crypto_test_win32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "crypto_test_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "crypto_test_win32.mak" CFG="crypto_test_win32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crypto_test_win32 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "crypto_test_win32 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "crypto_test_win32 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /map /machine:I386

!ELSEIF  "$(CFG)" == "crypto_test_win32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "crypto_test_win32 - Win32 Release"
# Name "crypto_test_win32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "hash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\hash\hmac_md5.c
# End Source File
# Begin Source File

SOURCE=..\hash\hmac_sha1.c
# End Source File
# Begin Source File

SOURCE=..\hash\md5.c
# End Source File
# Begin Source File

SOURCE=..\hash\sha1.c
# End Source File
# End Group
# Begin Group "block"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\block\aes.c
# End Source File
# Begin Source File

SOURCE=..\block\blowfish.c
# End Source File
# Begin Source File

SOURCE=..\block\des.c
# End Source File
# End Group
# Begin Group "rsa"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\utils\bignum.c
# End Source File
# Begin Source File

SOURCE=..\vigik\iso9796.c
# End Source File
# Begin Source File

SOURCE=..\rsa\keygen.c
# End Source File
# Begin Source File

SOURCE=..\rsa\primes.c
# End Source File
# Begin Source File

SOURCE=..\rsa\rabin.c
# End Source File
# Begin Source File

SOURCE=..\rsa\rsa.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=..\utils\random.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\block\aes.h
# End Source File
# Begin Source File

SOURCE=..\utils\bignum.h
# End Source File
# Begin Source File

SOURCE=..\block\blowfish.h
# End Source File
# Begin Source File

SOURCE=..\block\des.h
# End Source File
# Begin Source File

SOURCE=..\hash\hmac_md5.h
# End Source File
# Begin Source File

SOURCE=..\hash\hmac_sha1.h
# End Source File
# Begin Source File

SOURCE=..\vigik\iso9796.h
# End Source File
# Begin Source File

SOURCE=..\hash\md5.h
# End Source File
# Begin Source File

SOURCE=..\rsa\rabin.h
# End Source File
# Begin Source File

SOURCE=..\utils\random.h
# End Source File
# Begin Source File

SOURCE=..\rsa\rsa.h
# End Source File
# Begin Source File

SOURCE=..\hash\sha1.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
