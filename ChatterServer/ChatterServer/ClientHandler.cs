using System;
using System.Collections.Generic;
using System.Threading;
using System.Linq;
using System.Text;
using Chatter.DHHelper;
using System.Security.Cryptography;
using System.Numerics;
using System.IO;
using System.Timers;
using System.Net.Sockets;

namespace Chatter.ChatterServer
{
    public class ClientHandler
    {

        #region PrivateMembers

        TcpClient clientConnection = null;
        DHHelper.DHHelper dhKeyHelper = null;
        string userName;
        bool isAuthenticated = false;
        AuthenticationHelper authHelper = null;
        Thread workerThread = null;
        StreamReader socketReader = null;
        StreamWriter socketWriter = null;
        System.Timers.Timer probeAliveTimer;
        TimeSpan probeTimeSpan;
        volatile bool isClientAlive = true;
        Thread chatThread = null;
        System.Timers.Timer AuthTimeOut;
        #endregion


        public string UserName
        {
            get
            {
                if (!isAuthenticated)
                {
                    throw new InvalidOperationException("No user authenticated");
                }
                return userName;
            }
        }

        #region Constructor

        public ClientHandler(TcpClient connection)
        {
            this.dhKeyHelper = new DHHelper.DHHelper(false);
            this.clientConnection = connection;
            this.authHelper = new AuthenticationHelper();
            this.userName = String.Empty;
            this.isAuthenticated = false;
            this.socketReader = new StreamReader(this.clientConnection.GetStream());
            this.socketWriter = new StreamWriter(this.clientConnection.GetStream());
            this.probeTimeSpan = new TimeSpan(0, 0, Helper.GlobalConstants.ProbeInterval);
        }

        #endregion


        #region PrivateMembers

        void StartChat()
        {
            while (this.isAuthenticated && this.isClientAlive)
            {
                try
                {
                    //Wait for messages
                    string message = GetDecryptedLine();

                    //Create a thread
                    Thread messagePropogator = new Thread(Program.MessageHandler);

                    //Let the thread handle the messages
                    messagePropogator.Start(message);

                    //Go back and listen for more messages
                }
                catch (Exception e)
                {
                    Logger.Logger.WriteInfo(String.Format("Stream of client: {0} closed, so tearing down the connection", this.UserName));
                    this.isAuthenticated = false;
                    this.isClientAlive = false;
                    this.probeAliveTimer.Stop();
                }
            }
        }

        #endregion

        #region PublicMembers

        public void TearConnection()
        {
            if (CanBeDisposed())
            {
                this.clientConnection.Close();
            }
        }
        public bool CanBeDisposed()
        {
            return !this.isClientAlive && !this.isAuthenticated;
        }

        public void SendMessage(string message)
        {
            //send this message to the client
            this.probeAliveTimer.Enabled = false;
            if (isAuthenticated)
            {
                try
                {
                    SendLineEncrypted(message);
                    this.probeAliveTimer.Interval = Helper.GlobalConstants.ProbeInterval * 1000;
                    this.probeAliveTimer.Enabled = true;
                }
                catch (Exception e)
                {
                    //This probably means something bad happned
                    //Close the socket connetion and notify the server
                    Logger.Logger.WriteInfo(String.Format("Unable to send messages to client: {0}, so tearing down the connection", this.UserName));
                    this.isClientAlive = false;
                    this.isAuthenticated = false;
                    this.probeAliveTimer.Stop();
                }

            }
        }


        public bool StartCommunication()
        {
            bool isSuccess = false;
            //See if there is already ongoing communication..if yes then throw exception.
            if (workerThread != null)
            {
                throw new InvalidOperationException("Communication thread has already started");
            }

            try
            {

                //Create a thread for the communicating with the client
                workerThread = new Thread(this.Talk);

                //Start the thread
                workerThread.Start();

                isSuccess = true;
            }
            catch (Exception e)
            {
                workerThread = null;
                Logger.Logger.WriteException(e);
            }
            return isSuccess;
        }

        private void Talk()
        {
            //Set up a shared secret key using DH algorithm
            //Get the public exponent and base prime
             AuthTimeOut = new System.Timers.Timer(Helper.GlobalConstants.TimeOutForAuthentication * 1000);
             AuthTimeOut.Elapsed += new ElapsedEventHandler(AuthTimeOut_Elapsed);
            try
            {
                if (SetupDHSharedKey())
                {
                    AuthTimeOut.Enabled = false;
                    AuthTimeOut.Interval = Helper.GlobalConstants.TimeOutForAuthentication * 1000;
                    AuthTimeOut.Enabled = true;
                    string currentLine;
                    int noOfTries = 3;
                    while (noOfTries > 0 && !this.isAuthenticated && this.isClientAlive)
                    {
                        currentLine = GetDecryptedLine();
                        string[] credentials = currentLine.Split(null);
                        if (credentials.Length == 2)
                        {
                            AuthenticationHelper authHelper = new AuthenticationHelper();
                            //If the user name and password are valid for the user
                            if (authHelper.IsValid(credentials[0], credentials[1]))
                            {
                                this.isAuthenticated = true;
                                //Tell client that Authentication is sucessfull
                                AuthTimeOut.Enabled = false;
                                AuthTimeOut.Dispose();
                                this.SendLineEncrypted(Helper.GlobalConstants.AuthSuccessMessage);
                                this.userName = credentials[0];
                                Logger.Logger.WriteInfo(String.Format("User \"{0}\" successfully authenticated", credentials[0]));
                                break;
                            }
                        }
                        if (noOfTries == 1)
                        {
                            this.SendLineEncrypted(Helper.GlobalConstants.AuthFailureNoRetryMessage);
                        }
                        else
                        {
                            this.SendLineEncrypted(Helper.GlobalConstants.AuthFailureMessage);
                        }
                        Logger.Logger.WriteInfo(String.Format("User \"{0}\" failed to authenticate. Trials remained: {1}", credentials[0], noOfTries-1));
                        noOfTries--;
                    }
                    if (this.isAuthenticated)
                    {
                        this.probeAliveTimer = new System.Timers.Timer(Helper.GlobalConstants.ProbeInterval * 1000);//this.SendAliveProbe, Helper.GlobalConstants.ProbeMessage, this.probeTimeSpan, this.probeTimeSpan);
                        this.probeAliveTimer.Elapsed += new ElapsedEventHandler(probeAliveTimer_Elapsed);
                        this.probeAliveTimer.AutoReset = true;
                        this.chatThread = new Thread(this.StartChat);
                        this.chatThread.Start();
                        this.probeAliveTimer.Start();
                    }
                    else
                    {
                        AuthTimeOut.Enabled = false;
                        AuthTimeOut.Dispose();
                        this.clientConnection.Close();
                        Logger.Logger.WriteError("Authentication failed");
                    }
                }

            }
            catch (Exception e)
            {
                this.isClientAlive = false;
                this.isAuthenticated = false;
                this.clientConnection.Close();
                Logger.Logger.WriteError("Problem occured while trying to establish a secure connection with the client");
                Logger.Logger.WriteException(e);
            }

            if (!this.isAuthenticated)
            {
                this.isClientAlive = false;
                this.isAuthenticated = false;
            }
            //Validate the provided user name and password

            //Start Listining for any text from client
        }

        void AuthTimeOut_Elapsed(object sender, ElapsedEventArgs e)
        {
            this.isAuthenticated = false;
            this.isClientAlive = false;
            try
            {
                Logger.Logger.WriteError("Timeout happened during authentication of the user");
                Logger.Logger.WriteInfo("Tearing down the connection");
                this.AuthTimeOut.Enabled = false;
                this.clientConnection.Close();
            }
            catch (Exception) { }
        }

        bool SetupDHSharedKey()
        {
            bool isSucucess = false;
            try
            {
                string currentLine = this.socketReader.ReadLine();
                string[] splitStrings = currentLine.Split(null);
                if (splitStrings.Length == 2)
                {
                    BigInteger publicBase = BigInteger.One;
                    BigInteger publicPrime = BigInteger.One;
                    if (BigInteger.TryParse(splitStrings[0], out publicBase) && BigInteger.TryParse(splitStrings[1], out publicPrime) && publicBase > 0 && publicPrime > 0 && publicBase < publicPrime && PrimeNumberHelper.MillerRabin(publicPrime,int.MaxValue>>16))
                    {
                        this.dhKeyHelper.SetDHValues(publicPrime,publicBase);
                        //Send g^Xa
                        this.socketWriter.WriteLine(this.dhKeyHelper.PrivatePower.ToString());
                        this.socketWriter.Flush();

                        //Get g^Xb
                        currentLine = this.socketReader.ReadLine();
                        BigInteger clientExponent = new BigInteger();
                        //compuete the secret shared key
                        
                        if (BigInteger.TryParse(currentLine, out clientExponent) && clientExponent > 0 && this.dhKeyHelper.TryComputeSecretKey(clientExponent))
                        {
                            //Now the shared key has been established
                            //We start communicating with the client

                            Logger.Logger.WriteInfo("Secure channel established through Diffie-Hellman algorithm");
                            this.socketWriter.AutoFlush = true;
                            isSucucess = true;
                        }
                    }
                }
            }
            catch (Exception e)
            {
                this.isClientAlive = false;
                this.isAuthenticated = false;
                Logger.Logger.WriteError(String.Format("Problem occured trying to establish shared key using DH with {0}, so tearing down the connection", this.clientConnection.Client.RemoteEndPoint.ToString()));
                Logger.Logger.WriteException(e);
            }
            if (!isSucucess)
            {
                this.isClientAlive = false;
                this.isAuthenticated = false;
            }
            return isSucucess;

        }

        void probeAliveTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            this.SendMessage(Helper.GlobalConstants.ProbeMessage);
        }


        private string GetDecryptedLine()
        {
            string plainText = null;

            lock (this.socketReader)
            {
                plainText = this.dhKeyHelper.GetPlainText(this.socketReader.ReadLine());
            }
            return plainText;

        }

        private void SendLineEncrypted(string message)
        {
            lock (this.socketWriter)
            {
                this.socketWriter.WriteLine(this.dhKeyHelper.GetCipherText(message));
                this.socketWriter.Flush();
            }

        }

        #endregion

    }
}
