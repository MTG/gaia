# name of program
Name 'Gaia'

!define GAIA_ROOT '..\..'
!ifndef THIRDPARTY
    !define THIRDPARTY 'C:\Documents and Settings\buildbot\build-space\gaia2-thirdparty'
!endif
!ifndef FFMPEG_DIR
    !define FFMPEG_DIR 'C:\Documents and Settings\buildbot\build-space\ffmpeg0.5_install'
!endif

# installer filename
OutFile '${GAIA_ROOT}\packaging\win32\install_gaia2.exe'

#SetCompress off ; Useful to disable compression under development

# default installation directory (user will be able to change this)
InstallDir '$PROGRAMFILES\Gaia2\'

# enable xp styles
XPStyle on

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

!include 'uninstall_ex_begin.nsh'
Section 'Prerequisites'
  SetOutPath $INSTDIR\Prerequisites
  MessageBox MB_YESNO "Install Microsoft Visual Studio 2005 SP 1 C-Runtime?" /SD IDYES IDNO endRedist
    File "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"
    ExecWait "$INSTDIR\Prerequisites\vcredist_x86.exe"
    Goto endRedist
  endRedist:
SectionEnd

# everything that comes with the base
Section 'Gaia2 Library'
    ${SetOutPath} '$INSTDIR' # just need to set it once, is saved for the rest

    # create the uninstaller
    ${WriteUninstaller} 'uninstall.exe'

    # install the lib file
    ${SetOutPath} '$INSTDIR\lib'
    ${File} '${GAIA_ROOT}\packaging\win32\Gaia2lib\release\' 'libGaia2lib.a'

    # install the header files
    ${SetOutPath} '$INSTDIR\include\gaia2'
    ${File} '${GAIA_ROOT}\src\' '*.h'
    ${File} '${GAIA_ROOT}\src\algorithms\' 'analyzer.h'
    ${File} '${GAIA_ROOT}\src\algorithms\' 'applier.h'
    ${File} '${GAIA_ROOT}\src\metrics\' 'distancefunction.h'
    ${File} '${GAIA_ROOT}\src\metrics\' 'layoutawarefactory.h'

    !macro include_local3rdparty_header_folder dirname
        ${SetOutPath} '$INSTDIR\include\gaia2\${dirname}'
        ${File} '${GAIA_ROOT}\src\3rdparty\${dirname}\' '*.h'
    !macroend

    # install Eigen headers
    ${SetOutPath} '$INSTDIR\include\gaia2\Eigen'
    ${File} '${GAIA_ROOT}\src\3rdparty\Eigen\' '*'
    #!insertmacro include_local3rdparty_header_folder 'Eigen\src\Array'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Cholesky'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Core'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Core\arch\AltiVec'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Core\arch\SSE'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Core\products'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Core\util'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Geometry'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Geometry\arch'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Householder'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Jacobi'
    #!insertmacro include_local3rdparty_header_folder 'Eigen\src\LeastSquares'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\LU'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\QR'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\Sparse'
    !insertmacro include_local3rdparty_header_folder 'Eigen\src\SVD'

    # install thirdparty libraries
    ${SetOutPath} '$INSTDIR\thirdparty'

    # install thirdparty binaries (DLLs)
    ${SetOutPath} '$INSTDIR\thirdparty\bin'
    ${File} '${THIRDPARTY}\bin\' '*.dll'

    # install thirdparty includes
    ${SetOutPath} '$INSTDIR\thirdparty\include'
    ${File} '${THIRDPARTY}\include\' '*.h'

    !macro include_sub_folder dirname
        ${SetOutPath} '$INSTDIR\thirdparty\include\${dirname}'
        ${File} '${THIRDPARTY}\include\${dirname}\' '*.h'
    !macroend

    !insertmacro include_sub_folder 'tbb'
    !insertmacro include_sub_folder 'tbb\machine'

    # install thirdparty libs
    ${SetOutPath} '$INSTDIR\thirdparty\lib'
    ${File} '${THIRDPARTY}\lib\' '*.lib'

SectionEnd


Section 'Gaia2 Tools'
    ${SetOutPath} '$INSTDIR\bin'
    ${File} '${GAIA_ROOT}\packaging\win32\gaiamerge\release\' 'gaiamerge.exe'
    ${File} '${GAIA_ROOT}\packaging\win32\gaiainfo\release\' 'gaiainfo.exe'
    #${File} '${GAIA_ROOT}\build\examples\' 'streaming_schizo_extractor.exe'
SectionEnd


Section 'Doxygen Documentation'
    ${SetOutPath} '$INSTDIR\doc'
    # we have to list each extension explicitly because of a limitation in the
    # auto-uninstall part of this script
    ${File} '${GAIA_ROOT}\build\doc\html\' '*.css'
    ${File} '${GAIA_ROOT}\build\doc\html\' '*.gif'
    ${File} '${GAIA_ROOT}\build\doc\html\' '*.png'
    ${File} '${GAIA_ROOT}\build\doc\html\' '*.html'
SectionEnd



#!include 'envvarupdate.nsh'

## Do not update the env variables anymore, as it might be dangerous to fuck up the DLL path
## indeed QtCore4.dll is very likely to be distributed by other applications

#Section 'Update PATH Environment Variable'
#    # create new custom path key GAIA_THIRDPARTY
#    WriteRegStr HKLM 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment' \
#                     'GAIA_THIRDPARTY' '$INSTDIR\thirdparty\bin'
#
#    #   append ESSENTIA_THIRDPARTY to path
#    ${EnvVarUpdate} $0 'PATH' 'A' 'HKLM' '%GAIA_THIRDPARTY%'
#SectionEnd


# just files for the python bindings
Section 'Python Bindings'
    # Read latest version of python runtime
    # $0 - index
    # $1 - key-name
    StrCpy $0 0
    StrCpy $2 ''
    loop:
        EnumRegKey $1 HKLM 'Software\Python\PythonCore' $0
        StrCmp $1 '' done # if no more keys, goto done
        StrCpy $2 $1
        IntOp $0 $0 + 1
        goto loop
    done:


    # $2 should contain latest version, or empty string (if python not installed)
    #StrCmp $2 '2.5' valid_version
    StrCmp $2 '2.6' valid_version

    # not valid version
    MessageBox MB_OK 'Valid Python installation not detected.$\n\
                      Please install Python version 2.6 or later and restart \
                      installation: http://python.org/download/'
    Abort 'Valid Python installation not detected. Please install Python \
           version 2.6 or later: http://python.org/download/'

    valid_version:

    # read installation directory into $0 (path will usually have trailing slash)
    ReadRegStr $0 HKLM 'Software\Python\PythonCore\$2\InstallPath' ''

    # gather python-binding related files and install
    ${SetOutPath} '$0Lib\site-packages\gaia2'
    ${File} '${GAIA_ROOT}\packaging\win32\Gaia2Python\release\' 'Gaia2Python.dll'
    ${Rename} 'Gaia2Python.dll' '_gaia2.pyd'
    # also copy the DLLs directly in there, to avoid problems with the PATH variable
    ${File} '${THIRDPARTY}\bin\' '*.dll'

    ${File} '${GAIA_ROOT}\packaging\win32\Gaia2Python\' 'gaia2.py'
    ${Rename} 'gaia2.py' '__init__.py'
    ${File} '${GAIA_ROOT}\src\bindings\pygaia\' '*.py'
 
    ${SetOutPath} '$0Lib\site-packages\gaia2\classification\'
    ${File} '${GAIA_ROOT}\src\bindings\pygaia\classification\' '*.py'
    ${File} '${GAIA_ROOT}\src\bindings\pygaia\classification\' '*.css'

    ${SetOutPath} '$0Lib\site-packages\gaia2\linalg\'
    ${File} '${GAIA_ROOT}\src\bindings\pygaia\linalg\' '*.py'
    # install the python extractor
    #${SetOutPath} '$INSTDIR'
    #${CreateDirectory} 'bin'
    #${SetOutPath} '$INSTDIR\bin'
    #${File} '${GAIA_ROOT}\src\python\' 'essentia_extractor'

    # save Gaia install location
    WriteRegStr HKLM 'Software\MTG\Gaia' 'PythonInstallDir' '$0Lib\site-packages\gaia2'
SectionEnd


Section un.ExtraCleanup
    # delete any pyc files
    ReadRegStr $0 HKLM 'Software\MTG\Gaia' 'PythonInstallDir'
    Delete '$0\*.pyc'

    # clean up environment variables
    #DeleteRegValue HKLM 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment' \
    #                    'GAIA_THIRDPARTY'
    #${un.EnvVarUpdate} $0 'PATH' 'R' 'HKLM' '%GAIA_THIRDPARTY%'

    # clean up other keys
    DeleteRegKey HKLM 'Software\MTG\Gaia'

    # if MTG now empty, clean up MTG key
    EnumRegKey $0 HKLM 'Software\MTG' 0
    StrCmp $0 "" +1 +2
    DeleteRegKey HKLM 'Software\MTG'

SectionEnd

!include 'uninstall_ex_end.nsh'
