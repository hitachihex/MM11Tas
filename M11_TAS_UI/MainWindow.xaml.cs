﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Globalization;

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
        private string filepathToPointerRef = string.Empty;

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
                        foundProcess = true;
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
                lblmyinfo.Content = "No Game Found Yet";
                btnInitialize.Visibility = Visibility.Visible;
            }


            if (foundProcess && foundInjectedPointers)
            {
                Test(processHandle);
            }

        }

        public MainWindow()
        {
            InitializeComponent();
            CompositionTarget.Rendering += DoUpdates;
        }
        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }

        private void Test(IntPtr processHandle)
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
            //bool isLoading = BitConverter.ToBoolean(ReadValue(ptrIsLoading, 1), 0);

            bool isLoading = BitConverter.ToBoolean(ReadValue(ReadAddress(myBase + ptrIsLoading, new UInt32[] { 0 }), 1), 0);

            string myString = String.Empty;
            if (isPlayingBack)
            {
                myString = System.Text.Encoding.UTF8.GetString(ReadValue(ptrPlaybackMangerStringState, 120));
                for (int i = 0; i < myString.Length; i++)
                {
                    if (myString[i] == 0)
                    {
                        //Console.WriteLine("i: " + i);
                        myString = myString.Remove(i, 120 - i);
                        break;
                    }
                }
            }

            lblmyinfo.Content = "Pos: " + myXPos + ", " + myYPos + " \nXVelocity: " + myXVel + " | YVelocity: " + myYVel + " \nHP: " + myHP + " | BossHP: " + bossHP + " \nRobo Stage: " + roboStage + " | Wily Stage: " + wilyStage + " | RoomID: " + roomID + " \nisPlayingBack: " + isPlayingBack + " | isLoading: " + isLoading +  " \n" + myString;
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
                MessageBox.Show("Unable to find Process and/or Injected Pointers File!");
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
            imgMoveMe.Opacity = 0.02;
        }
    }
}