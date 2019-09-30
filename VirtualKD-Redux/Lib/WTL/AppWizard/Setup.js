// Windows Template Library - WTL version 10.0
// Copyright (C) Microsoft Corporation, WTL Team. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file MS-PL.txt at the root folder.

// WTL App Wizard universal setup program for Visual Studio

"use strict";

main();

function main()
{
	// Decode command line arguments
	var bDebug = false;
	var bElevated = false;
	var strVersion = "";
	var bCopyFiles = false;

	var Args = WScript.Arguments;
	for(var i = 0; i < Args.length; i++)
	{
		if(Args(i) == "/debug")
			bDebug = true;
		else if(Args(i) == "/elevated")
			bElevated = true;
		else if(Args(i).substr(0, 5) == "/ver:")
			strVersion = Args(i).substr(5);
		else if(Args(i) == "/copyfiles")
			bCopyFiles = true;
	}

	// See if UAC is enabled
	var Shell = WScript.CreateObject("Shell.Application");
	if(!bElevated && Shell.IsRestricted("System", "EnableLUA"))
	{
		// Check that the script is being run interactively.
		if(!WScript.Interactive)
		{
			WScript.Echo("ERROR: Elevation required.");
			return;
		}
		
		// Now relaunch the script, using the "RunAs" verb to elevate
		var strParams = "\"" + WScript.ScriptFullName + "\"";
		if(bDebug)
			strParams += " /debug";
		if(strVersion)
			strParams += " /ver:" + strVersion;
		if(bCopyFiles)
			strParams += " /copyfiles";
		strParams += " /elevated";
		Shell.ShellExecute(WScript.FullName, strParams, null, "RunAs");
		return;
	}

	// Create shell object
	var WSShell = WScript.CreateObject("WScript.Shell");
	// Create file system object
	var FileSys = WScript.CreateObject("Scripting.FileSystemObject");

	// Get the folder containing the script file
	var strValue = FileSys.GetParentFolderName(WScript.ScriptFullName);
	if(strValue == null || strValue == "")
		strValue = ".";

	var strSourceFolder = FileSys.BuildPath(strValue, "Files");
	if(bDebug)
		WScript.Echo("Source: " + strSourceFolder);

	if(!FileSys.FolderExists(strSourceFolder))
	{
		WScript.Echo("ERROR: Cannot find Wizard folder (should be: " + strSourceFolder + ")");
		return;
	}

	if(!strVersion)
		MessageBox(WSShell, "Setup will search for installed versions of Visual Studio,\nand ask to add the WTL App Wizard for each of them.");

	var strRegKey_32 = "HKLM\\Software\\";
	var strRegKey_64 = "HKLM\\Software\\Wow6432Node\\";

// Search for Visual Studio 2005-2015
	var nVersions = 6;

	var astrRegKeyVer = new Array();
	astrRegKeyVer[0] = "Microsoft\\VisualStudio\\8.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[1] = "Microsoft\\VisualStudio\\9.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[2] = "Microsoft\\VisualStudio\\10.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[3] = "Microsoft\\VisualStudio\\11.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[4] = "Microsoft\\VisualStudio\\12.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[5] = "Microsoft\\VisualStudio\\14.0\\Setup\\VC\\ProductDir";

	var strWizardFolder = "vcprojects";

	var astrVersions = new Array();
	astrVersions[0] = "Visual Studio 2005 (8.0)";
	astrVersions[1] = "Visual Studio 2008 (9.0)";
	astrVersions[2] = "Visual Studio 2010 (10.0)";
	astrVersions[3] = "Visual Studio 2012 (11.0)";
	astrVersions[4] = "Visual Studio 2013 (12.0)";
	astrVersions[5] = "Visual Studio 2015 (14.0)";

	var astrWizVer = new Array();
	astrWizVer[0] = "8.0";
	astrWizVer[1] = "9.0";
	astrWizVer[2] = "10.0";
	astrWizVer[3] = "11.0";
	astrWizVer[4] = "12.0";
	astrWizVer[5] = "14.0";

	var astrParamVer = new Array();
	astrParamVer[2] = "10";
	astrParamVer[3] = "11";
	astrParamVer[4] = "12";
	astrParamVer[5] = "14";

	var nSpecial = 2;

	var bFound = false;
	for(var i = 0; i < nVersions; i++)
	{
		if(strVersion && (strVersion != astrParamVer[i]))
			continue;

		if(bDebug)
			WScript.Echo("Looking for: " + astrVersions[i]);

		try
		{
			var strVCKey = strRegKey_32 + astrRegKeyVer[i];
			strValue = WSShell.RegRead(strVCKey);
		}
		catch(e)
		{
			try
			{
				var strVCKey_x64 = strRegKey_64 + astrRegKeyVer[i];
				strValue = WSShell.RegRead(strVCKey_x64);
			}
			catch(e)
			{
				continue;
			}
		}

		var strDestFolder = FileSys.BuildPath(strValue, strWizardFolder);
		if(bDebug)
			WScript.Echo("Destination: " + strDestFolder);
		if(!FileSys.FolderExists(strDestFolder))
			continue;

		if(i == nSpecial)   // special case for VS2010
		{
			var strCheckFile = FileSys.BuildPath(strDestFolder, "vc.vsdir");
			if(!FileSys.FileExists(strCheckFile))
				continue;
		}

		var strDataDestFolder = "";
		if(bCopyFiles)
		{
			strDataDestFolder = FileSys.BuildPath(strValue, "VCWizards");
			if(bDebug)
				WScript.Echo("Data Destination: " + strDataDestFolder);
			if(!FileSys.FolderExists(strDataDestFolder))
				continue;

			strDataDestFolder = FileSys.BuildPath(strDataDestFolder, "AppWiz\\WTL");
		}

		bFound = true;
		var bRet = true;
		if(!strVersion)
		{
			var strMsg = "Found: " + astrVersions[i] + "\n\nInstall WTL App Wizard?";
			bRet = MessageBox(WSShell, strMsg, true);
		}
		if(bRet)
		{
			SetupWizard(WSShell, FileSys, strSourceFolder, strDestFolder, strDataDestFolder, astrWizVer[i], bDebug);
		}
	}

// Search for Visual Studio 2017-2019
	var nVersionsNew = 2;

	var strWizardFolderNew = "Common7\\IDE\\VC\\vcprojects";

	var astrVersionsNew = new Array();
	astrVersionsNew[0] = "Visual Studio 2017 (15.0)";
	astrVersionsNew[1] = "Visual Studio 2019 (16.0)";

	var astrWizVerNew = new Array();
	astrWizVerNew[0] = "15.0";
	astrWizVerNew[1] = "16.0";

	var astrParamVerNew = new Array();
	astrParamVerNew[0] = "15";
	astrParamVerNew[1] = "16";

	var astrFilterVer = new Array();
	astrFilterVer[0] = "[15.0,16.0]";
	astrFilterVer[1] = "[16.0,17.0]";

	// Ensure that we can run vswhere.exe
	var strProgFileRegKey_x86 = "HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir (x86)";
	var strProgFileRegKey = "HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir";

	var strVSWFolder = "";
	try
	{
		strVSWFolder = WSShell.RegRead(strProgFileRegKey_x86);
	}
	catch(e)
	{
		try
		{
			strVSWFolder = WSShell.RegRead(strProgFileRegKey);
		}
		catch(e)
		{
			strVSWFolder = "";
		}
	}

	var strVSWFile = "";
	if(strVSWFolder)
	{
		strVSWFile = FileSys.BuildPath(strVSWFolder, "Microsoft Visual Studio\\Installer\\vswhere.exe");
		if(!FileSys.FileExists(strVSWFile))
		{
			if(bDebug)
				WScript.Echo("ERROR: Cannot find vswhere.exe");
			strVSWFile = "";
		}
	}

	for(var j = 0; j < nVersionsNew; j++)
	{
		if(!strVSWFile)
			continue;

		if(strVersion && (strVersion != astrParamVerNew[j]))
			continue;

		if(bDebug)
			WScript.Echo("Looking for: " + astrVersionsNew[j]);

		var TemporaryFolder = 2;
		var strTmpFolder = FileSys.GetSpecialFolder(TemporaryFolder);

		var strBatFile = FileSys.BuildPath(strTmpFolder, FileSys.GetTempName() + ".bat");
		if(bDebug)
			WScript.Echo("Temp batch file: " + strBatFile);

		var strLogFile = FileSys.BuildPath(strTmpFolder, FileSys.GetTempName());
		if(bDebug)
			WScript.Echo("Temp log file: " + strLogFile);

		try
		{
			var fileBat = FileSys.CreateTextFile(strBatFile);
			var strLine = "@\"" + strVSWFile + "\" -nologo -version " + astrFilterVer[j] + " -prerelease -property installationPath -format value > " + strLogFile;
			fileBat.WriteLine(strLine);
			fileBat.Close();

			WSShell.Run(strBatFile, 0, true);
		}
		catch(e)
		{
			var strError = "no info";
			if(e.description.length != 0)
				strError = e.description;
			WScript.Echo("ERROR: Cannot create/use tmp batch file (" + strError + ")");
			continue;
		}

		try
		{
			var ForReading = 1;
			var fileLog = FileSys.OpenTextFile(strLogFile, ForReading);

			while(!fileLog.AtEndOfStream)
			{
				var strLine = fileLog.ReadLine();

				var strDestFolder = FileSys.BuildPath(strLine, strWizardFolderNew);
				if(bDebug)
					WScript.Echo("Destination: " + strDestFolder);
				if(!FileSys.FolderExists(strDestFolder))
				{
					try
					{
						FileSys.CreateFolder(strDestFolder);
					}
					catch(e)
					{
					}

					if(!FileSys.FolderExists(strDestFolder))
						continue;
				}

				var strDataDestFolder = "";
				if(bCopyFiles)
				{
					strDataDestFolder = FileSys.BuildPath(strLine, "VCWizards");
					if(bDebug)
						WScript.Echo("Data Destination: " + strDataDestFolder);
					if(!FileSys.FolderExists(strDataDestFolder))
						continue;

					strDataDestFolder = FileSys.BuildPath(strDataDestFolder, "AppWiz\\WTL");
				}

				bFound = true;
				var bRet = true;
				if(!strVersion)
				{
					var strMsg = "Found: " + astrVersionsNew[j] + "\nat: " + strLine + "\n\nInstall WTL App Wizard?";
					bRet = MessageBox(WSShell, strMsg, true);
				}
				if(bRet)
				{
					SetupWizard(WSShell, FileSys, strSourceFolder, strDestFolder, strDataDestFolder, astrWizVerNew[j], bDebug);
				}
			}

			fileLog.Close();
		}
		catch(e)
		{
			var strError = "no info";
			if(e.description.length != 0)
				strError = e.description;
			WScript.Echo("ERROR: Cannot read tmp file (" + strError + ")");
		}

		try
		{
			FileSys.DeleteFile(strBatFile);
			FileSys.DeleteFile(strLogFile);
		}
		catch(e)
		{
			var strError = "no info";
			if(e.description.length != 0)
				strError = e.description;
			WScript.Echo("ERROR: Cannot delete tmp file (" + strError + ")");
		}
	}

	if(!strVersion)
	{
		if(bFound)
			MessageBox(WSShell, "Done!");
		else
			MessageBox(WSShell, "Setup could not find Visual Studio installed");
	}
}

function MessageBox(WSShell, strText, bYesNo)
{
	var nType = bYesNo ? (4 + 32) : 0;   // 4 = Yes/No buttons, 32 = Questionmark icon, 0 = OK button
	var nRetBtn = WSShell.Popup(strText, 0, "WTL App Wizard Setup", nType);
	return (nRetBtn == 6);   // 6 = Yes;
}

function SetupWizard(WSShell, FileSys, strSourceFolder, strDestFolder, strDataDestFolder, strWizVer, bDebug)
{
	// Copy files
	try
	{
		var strSrc = FileSys.BuildPath(strSourceFolder, "WTL10AppWiz.ico");
		var strDest = FileSys.BuildPath(strDestFolder, "WTL10AppWiz.ico");
		FileSys.CopyFile(strSrc, strDest);

		strSrc = FileSys.BuildPath(strSourceFolder, "WTL10AppWiz.vsdir");
		strDest = FileSys.BuildPath(strDestFolder, "WTL10AppWiz.vsdir");
		FileSys.CopyFile(strSrc, strDest);

		if(strDataDestFolder != "")
			FileSys.CopyFolder(strSourceFolder, strDataDestFolder, true);
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot copy file (" + strError + ")");
		return;
	}

	// Read and write WTL10AppWiz.vsz, add engine version and replace path when found
	try
	{
		var strSrc = FileSys.BuildPath(strSourceFolder, "WTL10AppWiz.vsz");
		var strDest = FileSys.BuildPath(strDestFolder, "WTL10AppWiz.vsz");

		var ForReading = 1;
		var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
		if(fileSrc == null)
		{
			WScript.Echo("ERROR: Cannot open source file " + strSrc);
			return;
		}

		var ForWriting = 2;
		var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
		if(fileDest == null)
		{
			WScript.Echo("ERROR: Cannot open destination file" + strDest);
			return;
		}

		while(!fileSrc.AtEndOfStream)
		{
			var strLine = fileSrc.ReadLine();
			if((strLine.indexOf("Wizard=VsWizard.VsWizardEngine") != -1))
			{
				strLine += "." + strWizVer;
			}
			else if(strLine.indexOf("WIZARD_VERSION") != -1)
			{
				strLine = "Param=\"WIZARD_VERSION = " + strWizVer + "\"";
			}
			else if(strLine.indexOf("ABSOLUTE_PATH") != -1)
			{
				if(strDataDestFolder == "")
					strLine = "Param=\"ABSOLUTE_PATH = " + strSourceFolder + "\"";
				else
					strLine = "Param=\"ABSOLUTE_PATH = " + strDataDestFolder + "\"";
			}

			fileDest.WriteLine(strLine);
		}

		fileSrc.Close();
		fileDest.Close();
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot read and write WTL10AppWiz.vsz (" + strError + ")");
		return;
	}

	// Create WTL folder
	var strDestWTLFolder = "";
	try
	{
		strDestWTLFolder = FileSys.BuildPath(strDestFolder, "WTL");
		if(!FileSys.FolderExists(strDestWTLFolder))
			FileSys.CreateFolder(strDestWTLFolder);
		if(bDebug)
			WScript.Echo("WTL Folder: " + strDestWTLFolder);
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot create WTL folder (" + strError + ")");
		return;
	}

	// Read and write additional WTL10AppWiz.vsdir, add path to the wizard location
	try
	{
		var strSrc = FileSys.BuildPath(strSourceFolder, "WTL10AppWiz.vsdir");
		var strDest = FileSys.BuildPath(strDestWTLFolder, "WTL10AppWiz.vsdir");

		var ForReading = 1;
		var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
		if(fileSrc == null)
		{
			WScript.Echo("ERROR: Cannot open source file " + strSrc);
			return;
		}

		var ForWriting = 2;
		var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
		if(fileDest == null)
		{
			WScript.Echo("ERROR: Cannot open destination file" + strDest);
			return;
		}

		while(!fileSrc.AtEndOfStream)
		{
			var strLine = fileSrc.ReadLine();
			if(strLine.indexOf("WTL10AppWiz.vsz|") != -1)
				strLine = "..\\" + strLine;
			fileDest.WriteLine(strLine);
		}

		fileSrc.Close();
		fileDest.Close();
	}
	catch(e)
	{
		var strError = "no info";
		if(e.description.length != 0)
			strError = e.description;
		WScript.Echo("ERROR: Cannot read and write WTL\\WTL10AppWiz.vsdir (" + strError + ")");
		return;
	}

	WScript.Echo("App Wizard successfully installed!");
}
