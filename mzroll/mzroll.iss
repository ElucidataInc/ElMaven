[Setup]

AppName=Maven
AppVerName=Maven_663
OutputBaseFilename=Maven_663
DefaultGroupName=Maven
LicenseFile=COPYING

AppId=Maven
AppPublisher=maven.princeton.edu
AppCopyright=maven.princeton.edu
AppPublisherURL=http://maven.princeton.edu
AppMutex=Maven
OutputDir=setup
DefaultDirName={pf}\Maven
UninstallDisplayIcon={app}\Maven.ico
Compression=lzma/max
PrivilegesRequired=admin

;WizardImageFile=install\win32\setup\SetupModern.bmp
;WizardSmallImageFile=install\win32\setup\SetupModernSmall.bmp

[Languages]

[Files]
Source: ..\bin\Maven.exe; DestDir: {app}; DestName: Maven.exe
Source: Maven.ico; DestDir: {app}; DestName: Maven.ico
Source: COPYING; DestDir: {app}; DestName: COPYING
Source: README; DestDir: {app}; DestName: README
Source: ..\bin\dlls\*; DestDir: {app}
Source: ..\bin\*.csv; DestDir: {app}
Source: ..\bin\*.model; DestDir: {app}
Source: ..\bin\ligand.db; DestDir: {app}
Source: ..\bin\plugins\*; DestDir: {app}\plugins\

[Icons]
Name: {group}\Maven; Filename: {app}\Maven.exe; WorkingDir: {app}
Name: {group}\Web pages; Filename: {app}\Maven.url
Name: {group}\COPYING; Filename: {app}\COPYING
Name: {group}\Uninstall; Filename: {uninstallexe}
Name: {commondesktop}\Maven; Filename: {app}\Maven.exe; WorkingDir: {app}; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: Create icon on desktop

[Run]
;

[Registry]
; a2d
Root: HKCR; SubKey: .mzXML; ValueType: string; ValueData: Maven.Data; Flags: uninsdeletekey
Root: HKCR; SubKey: .mzroll; ValueType: string; ValueData: Maven.Data; Flags: uninsdeletekey
Root: HKCR; SubKey: .mzPeaks; ValueType: string; ValueData: Maven.Data; Flags: uninsdeletekey
Root: HKCR; SubKey: Maven.Data; ValueType: string; ValueData: Maven data file; Flags: uninsdeletekey
Root: HKCR; SubKey: Maven.Data\Shell\Open\Command; ValueType: string; ValueData: """{app}\Maven.exe"" ""%1"""; Flags: uninsdeletevalue
Root: HKCR; Subkey: Maven.Data\DefaultIcon; ValueType: string; ValueData: {app}\Maven.ico; Flags: uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Maven\Application Settings; ValueType: string; ValueName:dataDir; ValueData: {app};  Flags: uninsdeletevalue

[INI]
Filename: {app}\Maven.url; Section: InternetShortcut; Key: URL; String: http://maven.princeton.edu

[UninstallDelete]
Type: files; Name: {app}\Maven.url

[Code]
