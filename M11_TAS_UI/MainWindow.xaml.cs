using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Threading;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace M11_TAS_UI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll")]
        public static extern bool ReadProcessMemory(int hProcess, UInt64 lpBaseAddress, byte[] lpBuffer, int dwSize, ref int lpNumberOfBytesRead);

        public static bool IsAdministrator()
        {
            return (new WindowsPrincipal(WindowsIdentity.GetCurrent()))
                      .IsInRole(WindowsBuiltInRole.Administrator);
        }

        private const int PROCESS_ALL_ACCESS = (0x1F0FFF);
        private Process process;
        private IntPtr processHandle = IntPtr.Zero;
        private Byte[] myBuffer = new Byte[8];
        private bool foundProcess = false;
        private bool foundInjectedPointers = false;
        private UInt64 ptrPlaybackManager = 0;
        private UInt64 ptrIsPlayingBack = 0;
        private UInt64 ptrPlaybackMangerStringState = 0;
        private UInt64 ptrIsLoading = 0;
        private UInt64 ptrIsPaused = 0;
        private string filepathToPointerRef = string.Empty;
        private string filepathToMainRecord = string.Empty;

        private bool FindProcess()
        {
            if (Process.GetProcessesByName("game").Length > 0)
            {
                if (!foundProcess)
                {
                    try
                    {
                        process = Process.GetProcessesByName("game")[0];
                        processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, process.Id);
                        filepathToPointerRef = process.MainModule.FileName.Replace("game.exe", "pointer_ref.txt");
                        filepathToMainRecord = process.MainModule.FileName.Replace("game.exe", "megaman.rec");
                        try
                        {
                            Process.Start("MM11Injector.exe");
                            Console.WriteLine("Started MM11Injector.exe!");
                            Thread.Sleep(1500);
                            foundProcess = true;
                        }
                        catch (Exception)
                        {
                            try
                            {
                                Process.Start(@"..\..\..\x64\Release\MM11Injector.exe");
                                Console.WriteLine("Oh hi! You must be debugging. Started MM11Injector.exe!");
                                Thread.Sleep(1500);
                                foundProcess = true;
                            }
                            catch (Exception e)
                            {
                                MessageBox.Show("Unable to find MM11Injector.exe! Make sure it is in the same directory as this GUI!\n\n" + e.Message, "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                                Console.WriteLine("Injector Error: " + e.Message);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Process Initialization Error: " + e.Message);
                    }

                }
                if (foundProcess && !foundInjectedPointers)
                {
                    try
                    {
                        string text = System.IO.File.ReadAllText(filepathToPointerRef);
                        string[] pointers = text.Split(',');
                        UInt64.TryParse(pointers[0], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ptrPlaybackManager);
                        UInt64.TryParse(pointers[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ptrIsPlayingBack);
                        UInt64.TryParse(pointers[2], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ptrPlaybackMangerStringState);
                        UInt64.TryParse(pointers[3], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ptrIsLoading);
                        UInt64.TryParse(pointers[4], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ptrIsPaused);
                        foundInjectedPointers = true;
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Injected Pointer Initialization Error: " + e.Message);
                    }
                }
                return (foundProcess && foundInjectedPointers);
            }
            return false;
        }

        private void DoUpdates(object sender, EventArgs e)
        {
            if (Process.GetProcessesByName("game").Length == 0)
            {
                processHandle = IntPtr.Zero;
                foundProcess = false;
                foundInjectedPointers = false;
                lblmyinfo.Content = "MM11 Not Found";
                btnInitialize.Visibility = Visibility.Visible;
            }


            if (foundProcess && foundInjectedPointers)
            {
                DoWork(processHandle);
            }

        }

        public MainWindow()
        {
            InitializeComponent();
            if (!IsAdministrator())
            {
                MessageBox.Show("You aren't running as administrator! Exiting since this won't work :(", "Error", MessageBoxButton.OK, MessageBoxImage.Stop);
                Application.Current.Shutdown();
            }
            CompositionTarget.Rendering += DoUpdates;
        }
        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }

        private void DoWork(IntPtr processHandle)
        {
            if (processHandle == IntPtr.Zero)
            {
                return;
            }
            UInt64 myBase = 0x0000000140000000;
            Byte[] myBuffer = new Byte[8];
            Byte[] myval = new Byte[4];

            var myStuff = ReadAddress(myBase, new UInt32[] { 0xC3EF58, 0x1D0 });
            float myXPos = BitConverter.ToSingle(ReadValue(myStuff + 0x50, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xC3EF58, 0x1D0 });
            float myYPos = BitConverter.ToSingle(ReadValue(myStuff + 0x54, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xC3EF58, 0x1D0 });
            float myYVel = BitConverter.ToSingle(ReadValue(myStuff + 0x1864, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xC3EF58, 0x1D0 });
            float myXVel = BitConverter.ToSingle(ReadValue(myStuff + 0x19B0, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xB87A20, 0x468 });
            Int32 myHP = BitConverter.ToInt32(ReadValue(myStuff + 0x183C, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xB87660, 0x120 });
            Int32 bossHP = BitConverter.ToInt32(ReadValue(myStuff + 0x4628, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xB87660 });
            Int32 roboStage = BitConverter.ToInt32(ReadValue(myStuff + 0x150, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xC3F6C0 });
            Int32 wilyStage = BitConverter.ToInt32(ReadValue(myStuff + 0x3B48, 4), 0);

            myStuff = ReadAddress(myBase, new UInt32[] { 0xB87F60 });
            Int32 roomID = BitConverter.ToInt32(ReadValue(myStuff + 0x78C, 4), 0);

            bool isPlayingBack = BitConverter.ToBoolean(ReadValue(ptrIsPlayingBack, 1), 0);
            bool isPaused = BitConverter.ToBoolean(ReadValue(ptrIsPaused, 1), 0);
            //bool isLoading = BitConverter.ToBoolean(ReadValue(ptrIsLoading, 1), 0);
            
            string playbackString = String.Empty;
            if (isPlayingBack)
            {
                playbackString = System.Text.Encoding.UTF8.GetString(ReadValue(ptrPlaybackMangerStringState, 120));
                for (int i = 0; i < playbackString.Length; i++)
                {
                    if (playbackString[i] == 0)
                    {
                        //Console.WriteLine("i: " + i);
                        playbackString = playbackString.Remove(i, 120 - i);
                        break;
                    }
                }
            }

            string stage = string.Empty;
            if (roboStage != 4)
            {
                stage = " \nRobo Stage: " + roboStage;
            }
            else
            {
                stage = " \nWily Stage: " + wilyStage;
            }

            lblmyinfo.Content = "Pos: " + myXPos + ", " + myYPos + " | X-Velocity: " + myXVel + " | Y-Velocity: " + myYVel + " | HP: " + myHP + " | BossHP: " + bossHP + stage + " | RoomID: " + roomID + " \nisPlayingBack: " + isPlayingBack + " | isPaused: " + isPaused +  " \n" + playbackString;
        }

        private Byte[] ReadValue(UInt64 address, int size)
        {
            Byte[] myValBuffer = new Byte[size];
            int bytesRead = 0;

            ReadProcessMemory((int)processHandle, address, myValBuffer, myValBuffer.Length, ref bytesRead);

            return myValBuffer;
        }

        private UInt64 ReadAddress(UInt64 address, UInt32[] offsets)
        {
            int bytesRead = 0;

            if (offsets != null)
            {
                for (int i = 0; i < offsets.Length; i++)
                {
                    ReadProcessMemory((int)processHandle, address + offsets[i], myBuffer, myBuffer.Length, ref bytesRead);
                    UInt64.TryParse(ByteArrayToString(myBuffer), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out address);
                    address = (UInt64)BitConverter.ToInt64(myBuffer, 0);
                }
                return address;
            }

            ReadProcessMemory((int)processHandle, address, myBuffer, myBuffer.Length, ref bytesRead);
            UInt64.TryParse(ByteArrayToString(myBuffer), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out address);
            address = (UInt64)BitConverter.ToInt64(myBuffer, 0);

            return address;
        }

        public static string ByteArrayToString(byte[] ba)
        {
            return BitConverter.ToString(ba).Replace("-", "");
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (!FindProcess())
            {
                MessageBox.Show("Unable to find Process and/or Injected Pointers File!", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
            } else
            {
                btnInitialize.Visibility = Visibility.Collapsed;
            }
        }

        private void Image_MouseEnter(object sender, MouseEventArgs e)
        {
            imgMoveMe.Opacity = 1;
        }

        private void Image_MouseLeave(object sender, MouseEventArgs e)
        {
            imgMoveMe.Opacity = 0.05;
        }

        private void MenuItemExit_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void MenuItemOpenRecord_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start("notepad++.exe", filepathToMainRecord);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Notepad++.exe issue: " + ex.Message);
            }            
        }

        private void MenuItemOpenFileLoc_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(filepathToMainRecord.Replace("megaman.rec", ""));
            }
            catch (Exception ex)
            {
                Console.WriteLine("No filepath yet. Initialize first! Error: " + ex.Message);
            }            
        }

        private void ImgMoveMe_ContextMenuOpening(object sender, System.Windows.Controls.ContextMenuEventArgs e)
        {
            if (foundProcess)
            {
                OpenRecord.IsEnabled = true;
                OpenFileLoc.IsEnabled = true;
                ReInject.IsEnabled = true;
            } else
            {
                OpenRecord.IsEnabled = false;
                OpenFileLoc.IsEnabled = false;
                ReInject.IsEnabled = false;
            }
        }

        private void ReInject_Click(object sender, RoutedEventArgs e)
        {
            foundProcess = false;
            foundInjectedPointers = false;
            FindProcess();
        }
    }
}
