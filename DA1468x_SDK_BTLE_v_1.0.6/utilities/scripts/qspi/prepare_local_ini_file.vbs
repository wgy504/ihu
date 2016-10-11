Const ForReading = 1, ForWriting = 2, ForAppending = 8

Set objArgs = WScript.Arguments
Set objShell = CreateObject("WScript.Shell")
Set fso = CreateObject("Scripting.FileSystemObject")
cfg="cli_programmer.ini"
jlinklog="jlink.log"

port=2331
Dim jlinks()

' Read jlink path from registry
gdb_path = objShell.RegRead("HKCU\Software\SEGGER\J-Link\InstallPath") + "JLinkGDBServerCL.exe"
jlink_path = objShell.RegRead("HKCU\Software\SEGGER\J-Link\InstallPath") + "JLink.exe"

sdk_root = fso.GetAbsolutePathName(fso.GetParentFolderName(WScript.ScriptFullName) + "\..\..\..") + "\"

' Add quotes in case of spaces in path
if InStr(jlink_path, " ") then
  jlink_path = """" + jlink_path + """"
  gdb_path = """" + gdb_path + """"
end if

' Default reset command
trc = jlink_path + " -CommandFile " + sdk_root + "utilities\scripts\debugging\jlink\jlink_cmd.jlink"

For i = 0 To objArgs.Count - 2 Step 2
  Select Case objArgs(i)
    Case "--cfg"
      cfg=objArgs(i + 1)
    Case "--id"
      id=objArgs(i + 1)
    Case "--port"
      port=objArgs(i + 1)
    Case "--log"
      jlinklog=objArgs(i + 1)
    Case "--trc"
      trc=objArgs(i + 1)      
    Case Else
      Exit For
  End Select
Next

' Id not passed with --id try default argument
If IsEmpty(id) And i < objArgs.Count Then id = objArgs(0) End If

' Id not passed find any connected JLink
if IsEmpty(id) then
  ' Ask JLink about connected debuggers
  'J-Link[0]: Connection: USB, Serial number: 480065842, ProductName: J-Link-OB-SAM3U128
  'J-Link[1]: Connection: USB, Serial number: 483000272, ProductName: J-Link-OB-SAM3U128
  set resn = new regexp
  set resn2 = new regexp
  ' pattern to search for is Serial number
  resn.Pattern = "Serial number: *\d*"
  resn2.Pattern = "Serial number: *"

  'use existing script to list all debuggers
  cmd = jlink_path + " -CommandFile " + sdk_root + "utilities\scripts\qspi\jlink_showemulist.script"

  num = 0
  set result = objShell.exec(cmd)
  Do While result.stdOut.AtEndOfStream <> True
    line = result.StdOut.ReadLine()
    'if line contains Serial number pattern, get it
    If resn.execute(line).count > 0 Then
      ReDim preserve jlinks(num + 1)
      id = resn2.Replace(resn.execute(line).Item(0), "")
      jlinks(num) = id
      num = num + 1
    end if
  Loop
End If
     
' If cli_programmer.ini is not in current folder, request cli_programmer to create one
If Not fso.FileExists(cfg) Then
  objShell.run sdk_root + "binaries\cli_programmer.exe --save " + cfg, 1, 1
End If

Set InFile = fso.OpenTextFile(cfg, ForReading)
Set OutFile = fso.CreateTextFile(cfg + ".new", 1)
Do While InFile.AtEndOfStream <> True
    TextLine = InFile.ReadLine
    If 0 < InStr(TextLine, "gdb_server_path") Then
      ' Regular expresion for deleting old device id from ini file
      Set re = new regexp
      re.Global = true

      ' Remove any existing device selection
      re.Pattern = " *-select usb=\d*"
      TextLine = re.Replace(TextLine, "")

      ' Remove any existing log file configuration
      re.Pattern = " *-log [^ ]*"
      TextLine = re.Replace(TextLine, "")

      ' Remove any existing port configuration
      re.Pattern = " *-((swo)|(telnet))?port \d*"
      TextLine = re.Replace(TextLine, "")

      ' If line is to short to be valid gdb server command line create default one
      If Len(TextLine) < 20 Then
        TextLine = "gdb_server_path = " + gdb_path + " -if SWD -device Cortex-M0 -singlerun -silent -speed 4000"
      End If

      ' Append selected device id
      If Not IsEmpty(id) Then
        TextLine = TextLine + " -select usb=" + id
      End If
      TextLine = TextLine + " -port " + CStr(port)
      TextLine = TextLine + " -swoport " + CStr(port + 1)
      TextLine = TextLine + " -telnetport " + CStr(port + 2)
      TextLine = TextLine + " -log " + jlinklog
    End If
    
    If 0 < InStr(TextLine, "target_reset_cmd") Then
      ' Regular expresion for deleting old device id from ini file
      Set re1 = new regexp
      re1.Global = true

      ' Remove any existing device selection
      re1.Pattern = " *-selectemubysn \d*"
      TextLine = re1.Replace(TextLine, "")

      ' If line is to short to be valid gdb server command line create default one
      If Len(TextLine) < 20 Then
        TextLine = "target_reset_cmd = " + trc
      End If

      ' Append selected device id
      If Not IsEmpty(id) Then
        TextLine = TextLine + " -selectemubysn " + id
      End If
    End If
        
    OutFile.WriteLine TextLine
Loop

InFile.close
OutFile.close

fso.DeleteFile cfg
fso.MoveFile cfg + ".new", cfg
