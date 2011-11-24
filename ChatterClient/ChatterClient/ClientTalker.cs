using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Linq;
using System.Numerics;
using System.IO;
using Chatter.DHHelper;
using Chatter.Helper;
using System.Threading;
using Chatter.Logger;
using System.Security.Cryptography;
using Chatter.ChatterClient;
using System.Text;

namespace Chatter.ChatterClient
{
    public class ClientTalker
    {
        TcpClient serverConnection;
        DHHelper.DHHelper dhKeyHelper = null;
        string userName;
        volatile ChatterConsole console = null;
        bool isAuthenticated = false;
        Thread chatListnerThread;
        bool isSharedKeySetup = false;
        volatile bool isServerActive = true;

        public bool IsServerActive
        {
            get { return isServerActive; }
        }

        public bool IsAuthenticated
        {
            get { return isAuthenticated; }
        }

        public string UserName
        {
            get { return userName; }
        }
        StreamReader socketReader = null;
        StreamWriter socketWriter = null;

        public ClientTalker(TcpClient connection)
        {
            this.serverConnection = connection;
            dhKeyHelper = new DHHelper.DHHelper(true);
            this.socketReader = new StreamReader(this.serverConnection.GetStream());
            this.socketWriter = new StreamWriter(this.serverConnection.GetStream());
        }

        public void SetConsole(ChatterConsole conso)
        {
            if (this.console == null)
            {
                this.console = conso;
            }
        }

        public void TearDownConnection()
        {
            this.isServerActive = false;
            this.isAuthenticated = false;
        }

        public void SendMessage(object obj)
        {
            if (obj != null)
            {
                string message = obj.ToString();
                if (this.IsAuthenticated && this.isServerActive)
                {
                    try
                    {
                        message.Trim();
                        SendLineEncrypted(this.UserName + ":" + message);
                    }
                    catch (Exception e)
                    {
                        if (this.console != null)
                        {
                            if (this.console.chatBoard.InvokeRequired)
                            {
                                this.console.chatBoard.Invoke(new System.Windows.Forms.MethodInvoker(delegate
                                {
                                    this.console.chatBoard.AppendText("\n" + "Unable to reach server");
                                    this.console.chatBoard.AppendText("\n" + "Terminating the connection");
                                    this.console.chatBoard.SelectionStart = this.console.chatBoard.TextLength - 1;
                                    this.console.chatBoard.Refresh();
                                    this.console.chatBoard.ScrollToCaret();
                                    this.console.RaiseProblem();
                                }));
                            }
                        }
                        Logger.Logger.WriteException(e);
                        this.isServerActive = false;
                    }
                }
                else
                {
                    throw new InvalidOperationException("Client not authenticated to the server or server not active");
                }
            }
        }

        public bool Authenticate(string userName, string password)
        {
            bool isSucess = false;
            try
            {
                if (SetUpDHSharedKey())
                {
                    isSucess = AuthenticateForUser(userName, password);
                }
                else
                {
                    Logger.Logger.WriteError("Failed to set up Diffie-Hellman shared key");
                }
            }
            catch (Exception e)
            {
                this.isServerActive = false;
                Logger.Logger.WriteException(e);
            }
            return isSucess;
        }

        bool AuthenticateForUser(string userName, string password)
        {
            bool isSucess = false;
            this.socketWriter.AutoFlush = true;

            SendLineEncrypted(userName + " " + password);
            string result = GetDecryptedLine();
            if (result.Equals(GlobalConstants.AuthSuccessMessage))
            {
                isSucess = true;
                this.isAuthenticated = true;
                this.userName = userName;
                this.chatListnerThread = new Thread(this.Listen);
                this.chatListnerThread.Start();
                Logger.Logger.WriteInfo("Authentication successfull");
                Logger.Logger.WriteInfo("Enjoy chatting with Chatter!");
            }
            else if(result.Equals(GlobalConstants.AuthFailureNoRetryMessage))
            {
                this.isServerActive = false;
                Logger.Logger.WriteError("Authentication failed\nServer closed the connection!!");
            }
            else
            {
                Logger.Logger.WriteError("Authentication failed");
            }
            return isSucess;
        }
        void Listen()
        {
            try
            {
                string currentLine;
                while (true && this.isServerActive)
                {
                    currentLine = GetDecryptedLine();
                    if (!currentLine.Equals(GlobalConstants.ProbeMessage))
                    {
                        currentLine = currentLine.Trim();
                        if (currentLine.StartsWith(this.UserName + ":"))
                        {
                            currentLine = currentLine.Substring(this.UserName.Length + 1);
                            currentLine = "Me:" + currentLine.Trim();
                        }
                        if (this.console == null)
                        {
                            Console.WriteLine(currentLine);
                        }
                        else
                        {
                            if (this.console.chatBoard.InvokeRequired)
                            {
                                this.console.chatBoard.Invoke(new System.Windows.Forms.MethodInvoker(delegate
                                {
                                    this.console.chatBoard.AppendText("\n" + currentLine);
                                    this.console.chatBoard.SelectionStart = this.console.chatBoard.TextLength - 1;
                                    this.console.chatBoard.Refresh();
                                    this.console.chatBoard.ScrollToCaret();
                                }));
                            }

                        }
                    }
                }
            }
            catch (Exception e)
            {
                if (this.console.chatBoard.InvokeRequired)
                {
                    this.console.chatBoard.Invoke(new System.Windows.Forms.MethodInvoker(delegate
                    {
                        this.console.chatBoard.AppendText("\n" + "Terminating the connection");
                        this.console.chatBoard.SelectionStart = this.console.chatBoard.TextLength - 1;
                        this.console.chatBoard.Refresh();
                        this.console.chatBoard.ScrollToCaret();
                    }));
                    this.console.RaiseProblem();
                }
                this.isServerActive = false;
                Logger.Logger.WriteException(e);
            }
        }
        bool SetUpDHSharedKey()
        {
            bool isSucucess = false;
            if (this.isSharedKeySetup)
            {
                return this.isSharedKeySetup;
            }

            try
            {

                this.socketWriter.WriteLine(this.dhKeyHelper.PublicBase.ToString() + " " + this.dhKeyHelper.PublicPrime.ToString());
                this.socketWriter.Flush();
                this.socketWriter.WriteLine(this.dhKeyHelper.PrivatePower.ToString());
                this.socketWriter.Flush();
                string currentLine;
                currentLine = this.socketReader.ReadLine();
                BigInteger clientExponent = new BigInteger();
                //compuete the secret shared key
                if (BigInteger.TryParse(currentLine, out clientExponent) && clientExponent > 0 && this.dhKeyHelper.TryComputeSecretKey(clientExponent))
                {
                    //Now the shared key has been established
                    //We start communicating with the client
                    Logger.Logger.WriteInfo("Secure channel established via Diffie-Hellman algorithm");
                    isSucucess = true;
                    this.isSharedKeySetup = true;
                }
            }
            catch (Exception e)
            {
                this.isServerActive = false;
                Logger.Logger.WriteError(String.Format("Problem occured while trying to establish shared key using Diffie-Hellman with server"));
                Logger.Logger.WriteException(e);
            }

            return isSucucess;
        }

        private string GetDecryptedLine()
        {
            lock (this.socketReader)
            {
                string plainText = null;
                plainText = this.dhKeyHelper.GetPlainText(this.socketReader.ReadLine());
                return plainText;
            }
        }

        private void SendLineEncrypted(string message)
        {
            lock (this.socketWriter)
            {
                this.socketWriter.WriteLine(this.dhKeyHelper.GetCipherText(message));
                this.socketWriter.Flush();
            }
        }
    }
}
