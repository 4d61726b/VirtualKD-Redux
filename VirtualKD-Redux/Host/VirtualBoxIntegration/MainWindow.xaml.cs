using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using VirtualBox;

namespace VirtualBoxIntegration
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public List<MachineWrapper> Machines {get;set;}

        static string MyDir = System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

        public MainWindow()
        {
            RegistryKey key;
            string locationerr = "Unable to locate VirtualBox. Please install the latest version of VirtualBox.";
            string versionerr = "VirtualBox older than 6.1 detected. Please install VirtualBox 6.1 or later to use this version of VirtualKD-Redux.";

            InitializeComponent();

            this.DataContext = this;

            key = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Oracle\VirtualBox");
            if (key == null)
            {
                MessageBox.Show(locationerr, "VirtualBoxIntegration", MessageBoxButton.OK, MessageBoxImage.Error);
                Environment.Exit(1);
            }

            try
            {
                _VirtualBox = new VirtualBox.VirtualBox();
                
            }
            catch (System.InvalidCastException e)
            {
                string version = key.GetValue("Version") as string;

                if (version == null)
                {
                    MessageBox.Show(locationerr, "VirtualBoxIntegration version null", MessageBoxButton.OK, MessageBoxImage.Error);
                    Environment.Exit(1);
                }

                if (!IsMinimumVirtualBoxVersion(version))
                {
                    MessageBox.Show(versionerr, "VirtualBoxIntegration not minimal virtualbox", MessageBoxButton.OK, MessageBoxImage.Error);
                    Environment.Exit(1);
                }

                throw e;
            }

            lblVersion.Content = _VirtualBox.Version;
            if (!IsMinimumVirtualBoxVersion(_VirtualBox.Version))
            {
                MessageBox.Show(versionerr, "VirtualBoxIntegration", MessageBoxButton.OK, MessageBoxImage.Error);
                Environment.Exit(1);
            }

            var is64Bit = App.Is64Bit();

            string dir;
            if (key != null)
                dir = key.GetValue("InstallDir") as string;
            else
                dir = Environment.GetEnvironmentVariable(is64Bit ? "ProgramW6432" : "ProgramFiles") + "\\Oracle\\VirtualBox";
            if (dir == null || !Directory.Exists(dir))
                throw new Exception("VirtualBox directory not found");
            VirtualBoxPath = dir.TrimEnd('\\');

            Refresh();
        }
        const string VirtualKDConfigEntry = "VBoxInternal/Devices/VirtualKD/0/Config/Path";

        private bool IsMinimumVirtualBoxVersion(string version)
        {
            try
            {
                string[] sp = version.Split('.');
                int major = int.Parse(sp[0]);

                if (major < 6)
                {
                    return false;
                }

                if (major == 6)
                {
                    int minor = int.Parse(sp[1]);
                    if (minor < 1)
                    {
                        return false;
                    }
                }

                return true;
            }
            catch
            {
            }

            return false;
        }

        public class MachineWrapper : INotifyPropertyChanged
        {
            private VirtualBox.IMachine _Machine;
            private MainWindow _Window;

            public MachineWrapper(MainWindow w, VirtualBox.IMachine m)
            {
                _Machine = m;
                _Window = w;
                EnableCommand = new EnableDisableCommand(this, true);
                DisableCommand = new EnableDisableCommand(this, false);
            }

            public string Name
            {
                get
                {
                    return _Machine.Name;
                }
            }


            public class EnableDisableCommand : ICommand
            {
                private MachineWrapper _Wrapper;
                public EnableDisableCommand(MachineWrapper wrp, bool enable)
                {
                    _Enable = enable;
                    _Wrapper = wrp;
                }

                public bool CanExecute(object parameter)
                {
                    return _Enable != _Wrapper.Integrated;
                }

                public event EventHandler CanExecuteChanged;
                private bool _Enable;

                public void Execute(object parameter)
                {
                    if (App.IsVirtualBoxRunning())
                    {
                        MessageBox.Show("Please close ALL VirtualBox instances before changing the settings.", "VirtualBoxIntegration", MessageBoxButton.OK, MessageBoxImage.Error);
                        return;
                    }

                    ProcessStartInfo info = new ProcessStartInfo(System.IO.Path.Combine(_Wrapper._Window.VirtualBoxPath, "VBoxManage.exe"), "setextradata \"" + _Wrapper._Machine.Name + "\" " + VirtualKDConfigEntry) { CreateNoWindow = true, UseShellExecute = false };
                    if (_Enable)
                    {
                        info.Arguments += " \"" + MyDir + "\"";
                        App.CheckKDClientPermissions(MyDir);
                    }

                    var proc = Process.Start(info);
                    proc.WaitForExit();
                    if (proc.ExitCode != 0)
                    {
                        MessageBox.Show("Failed to update machine properties", "VirtualBoxIntegration", MessageBoxButton.OK, MessageBoxImage.Error);
                    }

                    _Wrapper.InvalidateStatus();
               }

                public void InvalidateStatus()
                {
                    if (CanExecuteChanged != null)
                        CanExecuteChanged(this, EventArgs.Empty);
                }
            }

            public EnableDisableCommand EnableCommand { get; set; }
            public EnableDisableCommand DisableCommand { get; set; }

            public bool Integrated
            {
                get
                {
                    return _Machine.GetExtraData(VirtualKDConfigEntry) == MyDir;
                }
            }

            public string Status
            {
                get
                {
                    return Integrated ? "Enabled" : "Disabled";
                }
            }


            internal void InvalidateStatus()
            {
                EnableCommand.InvalidateStatus();
                DisableCommand.InvalidateStatus();
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Status"));
                    PropertyChanged(this, new PropertyChangedEventArgs("Integrated"));
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Refresh();
        }

        private void Refresh()
        {
            Machines = new List<MachineWrapper>();

            foreach (var machine in _VirtualBox.Machines.Cast<VirtualBox.IMachine>())
            {
                if (machine.Accessible != 0)
                {
                    Machines.Add(new MachineWrapper(this, machine));
                }
            }

            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("Machines"));

            App.CheckKDClientPermissions(MyDir);
        }
        

        public event PropertyChangedEventHandler PropertyChanged;
        private VirtualBox.VirtualBox _VirtualBox;
        private string VirtualBoxPath;

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Process.Start(System.IO.Path.Combine(VirtualBoxPath, "VirtualBox.exe"));
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }
    }
}
