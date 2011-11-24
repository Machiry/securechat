using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using Chatter.Logger;
using System.Net;
using System.IO;
using System.Net.Sockets;
using System.Text;

namespace Chatter.ChatterServer
{
    public class Program
    {
        static UInt16 listningPortnumber;
        static int maxNumberOfClients = 4;
        static List<ClientHandler> activeClients = new List<ClientHandler>();

        static void Main(string[] args)
        {

            if (!IsInputValid(args))
            {
                Console.WriteLine("Keep going! :-)");
                return;
            }

            try
            {

                Logger.Logger.InitializeLogger(null);
                Logger.Logger.WriteInfo("Logging initialized");
                Logger.Logger.WriteInfo("Trying to listen at port: " + listningPortnumber);
                TcpListener listner = new TcpListener(new IPEndPoint(IPAddress.Any, (int)listningPortnumber));
                try
                {
                    listner.Start();

                }
                catch (Exception e)
                {
                    Console.WriteLine("Unable to listen via the provided port\n" + e.Message + "\n" + e.StackTrace);
                }

                while (true)
                {
                    if (listner.Pending())
                    {

                        TcpClient connection = listner.AcceptTcpClient();
                        Logger.Logger.WriteInfo(string.Format("Connection request from: {0}", connection.Client.RemoteEndPoint.ToString()));
                        CleanActiveClients();
                        lock (activeClients)
                        {
                            if (activeClients.Count < maxNumberOfClients)
                            {
                                Logger.Logger.WriteInfo(string.Format("Connection accepted"));
                                ClientHandler handler = new ClientHandler(connection);
                                activeClients.Add(handler);
                                handler.StartCommunication();
                            }
                            else
                            {
                                Logger.Logger.WriteInfo(string.Format("Connection will be rejected because the maximum number of connections has been reached"));
                                connection.Close();
                            }
                        }
                    }
                    listner.Start();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception occured in the ChatterServer application");
                Console.WriteLine("Exception details:");
                Console.WriteLine(e.Message);
                Console.WriteLine("Stack trace:");
                Console.WriteLine(e.StackTrace.ToString());
                Console.WriteLine("Try again after checking the internet connectivity");
                Console.WriteLine("Make sure that you have sufficent permission to open a port on the system");
                Console.WriteLine("Support contact: Machiry Aravind Kumar");
            }
        }   

        static void CleanActiveClients()
        {
            List<ClientHandler> clientHandlers = new List<ClientHandler>();
            lock (activeClients)
            {
                activeClients.ForEach(delegate(ClientHandler cli)
                {
                    if (!cli.CanBeDisposed())
                    {
                        clientHandlers.Add(cli);
                    }
                    else
                    {
                        try
                        {
                            cli.TearConnection();
                        }
                        catch (Exception) { }
                    }
                });
                activeClients.RemoveAll(p => true);
                activeClients.AddRange(clientHandlers);
            }
        }
        static bool IsInputValid(string[] args)
        {

            if (args.Length > 2 || args.Length < 1)
            {
                Console.WriteLine("Invalid number of arguments");
                Usage();
                return false;
            }
            else
            {
                UInt16 portNumber = 0;
                if (!UInt16.TryParse(args[0], out portNumber) || portNumber < 1 || portNumber > 65534)
                {
                    Console.WriteLine("Port number provided: {0} is invalid", args[0]);
                    Usage();
                    return false;
                }
                listningPortnumber = portNumber;
                if (args.Length > 1)
                {
                    int maxNumber = 0;
                    if (!int.TryParse(args[1], out maxNumber) || maxNumber <= 0 || maxNumber > 10000)
                    {
                        Console.WriteLine("Arguments provided for number of clients: {0} is invalid", args[1]);
                        Usage();
                        return false;
                    }

                    maxNumberOfClients = maxNumber;
                }
            }
            return true;
        }

        static void Usage()
        {
            Console.WriteLine("Usage:");
            Console.WriteLine("ChatterServer.exe <port number> [<no. of clients>]");
            Console.WriteLine("Where:");
            Console.WriteLine("   <port number>    this is the port number on");
            Console.WriteLine("                    which the server listens for");
            Console.WriteLine("                    outbound connections");
            Console.WriteLine("                    (in the range 1-65534)");
            Console.WriteLine("   <no. of clients> limits the number of clients");
            Console.WriteLine("                    to the given number");
            Console.WriteLine("                    (default: 4, max: 10000)");
            Console.WriteLine();
            Console.WriteLine("NOTE: Please make sure that you have sufficent permissions");
            Console.WriteLine("      to open a listening port. The application might fail");
            Console.WriteLine("      if the above privileges are missing");
            Console.WriteLine();
            Console.WriteLine("The file containing hashes of vaid usernames and passwords");
            Console.WriteLine("need to be specified in the configuration file \"ChatterServer.exe.config\"");
            Console.WriteLine();
            Console.WriteLine("Happy \"Chatter\"ing!!!!");
        }


        public static void MessageHandler(object obj)
        {
            //Send message to all the clients currently registered
            if (obj != null)
            {
                string message = obj.ToString();
                lock (activeClients)
                {
                    activeClients.ForEach(delegate(ClientHandler client)
                    {
                        client.SendMessage(message);
                    });
                }
            }
        }

        public static void ConnectionTearDown(ClientHandler client)
        {
            //Some client has lost connection.
            //Drop its connections at server side
            lock (activeClients)
            {
                activeClients.Remove(client);
            }
        }
    }
}
