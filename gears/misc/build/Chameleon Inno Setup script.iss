[Setup]
AppCopyright=Copyright Digital Gearheads 2004
AppName=Chameleon
AppVerName=Chameleon 1.0
DefaultDirName={pf}\Digital Gearheads\Chameleon
EnableDirDoesntExistWarning=true
AllowRootDirectory=true
AppPublisher=Digital Gearheads
AppVersion=1.0
AppID={28B3D46A-A094-44A1-AEDC-A790DB2D20DE}
UninstallDisplayName=Chameleon
DefaultGroupName=Digital Gearheads
OutputBaseFilename=ChameleonSetup10
VersionInfoVersion=1.0
VersionInfoCompany=Digital Gearheads
[Files]
Source: plink.exe; DestDir: {app}
Source: pscp.exe; DestDir: {app}
Source: ..\misc\help\Chameleon.chm; DestDir: {app}
Source: ..\misc\betas\Chameleon.exe; DestDir: {app}
[Icons]
Name: {group}\Digital Gearheads\Chameleon; Filename: {app}\Chameleon.exe; IconIndex: 0
Name: {userdesktop}\Chameleon; Filename: {app}\Chameleon.exe; IconIndex: 0
