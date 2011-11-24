using System;
using System.Net.Sockets;
using System.Net;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Linq;
using System.Text;

namespace Chatter.ChatterClient
{
    class Program
    {
        static IPAddress ipaddress;
        static int portNumber;

        static void Main(string[] args)
        {
            if (!IsInputValid(args))
            {
                return;
            }
            Logger.Logger.InitializeLogger(null);
            TcpClient tcp = new TcpClient();
            Console.Write("Enter username: ");
            string userName = Console.ReadLine();
            Console.Write("Enter password: ");
            string password = Console.ReadLine();
            Console.WriteLine("You are being authenticate with the server. Please wait...");
            try
            {
                tcp.Connect(new IPEndPoint(ipaddress, portNumber));
            }
            catch (Exception e)
            {
                Logger.Logger.WriteException(e);
                Console.WriteLine("Are you sure server address specified is right?");
                Console.WriteLine();
                Console.WriteLine("It cannot be reached. Check the connectivity to the server.");
                return;
            }
            try
            {
                ClientTalker client = new ClientTalker(tcp);
                client.Authenticate(userName, password);

                if (!client.IsServerActive)
                {
                    Console.WriteLine("Problem while trying to communicate with the server");
                    Console.WriteLine("Check the internet connection and if the server is up or not");

                    return;
                }
                while (!client.IsAuthenticated && client.IsServerActive)
                {
                    Console.WriteLine("Either user name or password are wrong. Please try again");
                    Console.Write("Enter username: ");
                    userName = Console.ReadLine();
                    Console.Write("Enter password: ");
                    password = Console.ReadLine();
                    Console.WriteLine("You are being authenticate with the server. Please wait...");
                    try
                    {
                        client.Authenticate(userName, password);
                    }
                    catch (Exception)
                    {
                        if (!client.IsAuthenticated)
                        {
                            Console.WriteLine("Unable to authenticate the provided user with the server");
                            Console.WriteLine("Check your credentials and try again");
                            Console.WriteLine("Press return to exit......");
                            Console.ReadLine();
                            return;
                        }
                    }
                }
                if (!client.IsAuthenticated)
                {
                    Console.WriteLine("Unable to authenticate the provided user with the server");
                    Console.WriteLine("Check your credentials and try again");
                }
                bool hasChatted = false;
                while (client.IsAuthenticated)
                {
                    ChatterConsole chatConsole = new ChatterConsole(client);
                    client.SetConsole(chatConsole);
                    hasChatted = true;
                    Application.Run(chatConsole);
                    //string chatMessage = Console.ReadLine();
                    //client.SendMessage(chatMessage);
                }
                if (!client.IsServerActive && !hasChatted)
                {
                    Logger.Logger.WriteError("Problem occured when trying to communicate with the server");
                    Logger.Logger.WriteError("Server closed the session");
                    Console.WriteLine("Press return to exit......");
                    Console.ReadLine();
                }
                //Console.WriteLine("Press any key to exit........");
                //Console.ReadLine();
            }
            catch (Exception e)
            {
                Console.WriteLine("Some error occured");
                Console.WriteLine("Error message:" + e.Message);
                Console.WriteLine("Stack trace:" + e.StackTrace);
                Console.WriteLine("Try again after checking the Internet connectivity");
                Console.WriteLine("Make sure that you have sufficent permission to open a port on the system");
                Console.WriteLine("Support contact: Machiry Aravind Kumar");
                Console.WriteLine("Press return to exit......");
                Console.ReadLine();
            }

        }

        static bool IsInputValid(string[] args)
        {
            if (args.Length != 2)
            {
                Usage();
                return false;
            }
            IPAddress ip;
            if (!IPAddress.TryParse(args[0], out ip))
            {
                Console.WriteLine("The IP address provided is not a valid IP address");
                Usage();
                return false;
            }
            int portNo;
            if (!int.TryParse(args[1], out portNo) || portNo < 0 || portNo > 65535)
            {
                Console.WriteLine("Port number must be an interget in the range 0-65535");
                return false;
            }
            ipaddress = ip;
            portNumber = portNo;
            return true;
        }

        static void Usage()
        {
            Console.WriteLine("Usage: ChatterClient.exe <server IP> <port number>");
            Console.WriteLine("Where:");
            Console.WriteLine("   <server IP>   IP on which the server is hosted");
            Console.WriteLine("   <port number> where ChatterServer is listening");
            Console.WriteLine();
            Console.WriteLine("Happy \"Chatter\"ing!!!!");
        }
    }
}
