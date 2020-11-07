using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Windows;

namespace VirtualBoxIntegration
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        [DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool IsWow64Process([In] IntPtr hProcess, [Out] out bool lpSystemInfo);


        public App()
        {
            this.DispatcherUnhandledException += App_DispatcherUnhandledException;
        }

        void App_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.ToString(), "VirtualBoxIntegration", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        public static bool Is64Bit()
        {
            try
            {
                if (IntPtr.Size == 8)
                    return true;
                bool retVal;
                IsWow64Process(Process.GetCurrentProcess().Handle, out retVal);
                return retVal;
            }
            catch (System.Exception)
            {
                return false;
            }
        }

        internal static bool IsVirtualBoxRunning()
        {
            return Process.GetProcesses().Where(p => StringComparer.InvariantCultureIgnoreCase.Compare(Path.GetFileNameWithoutExtension(p.ProcessName), "VirtualBox") == 0).Count() != 0;
        }

        static bool IsSystemOwned(string fn)
        {
            return (File.GetAccessControl(fn).GetOwner(typeof(SecurityIdentifier)) as SecurityIdentifier) == new SecurityIdentifier(WellKnownSidType.LocalSystemSid, null);
        }

        static void ChownToSystem(string fn)
        {
            if (!IsSystemOwned(fn))
                RunAsAdmin("icacls", "\"" + fn + "\" /setowner system", true);
        }


        internal static void CheckKDClientPermissions(string myDir)
        {
            if (!IsSystemOwned(Path.Combine(myDir, "kdclient64.dll")) || !IsSystemOwned(Path.Combine(myDir, "kdclient32.dll")))
            {
                if (MessageBox.Show("VirtualBox won't load KDCLIENT32.DLL/KDCLIENT64.DLL unless it is owned by the System acoount. Do you want to fix it now?", "VirtualBoxIntegration", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                {
                    ChownToSystem(Path.Combine(myDir, "kdclient64.dll"));
                    ChownToSystem(Path.Combine(myDir, "kdclient32.dll"));
                }
            }
        }

        public static void RunAsAdmin(string cmd, string args, bool wait = false)
        {
            try
            {
                var p = new Process();
                p.StartInfo.FileName = cmd;
                p.StartInfo.Arguments = args;
                if (System.Environment.OSVersion.Version.Major >= 6)
                    p.StartInfo.Verb = "runas";
                p.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                p.Start();
                if (wait)
                    p.WaitForExit();
            }
            catch
            {

            }
        }
    }
}
