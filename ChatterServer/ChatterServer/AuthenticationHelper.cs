using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Configuration;
using Chatter.Logger;
using System.Text;

namespace Chatter.ChatterServer
{
    public class AuthenticationHelper
    {
        static string shadowFile = null;
        static StreamReader fileReader = null;
        static AuthenticationHelper()
        {
            try
            {
                shadowFile = ConfigurationManager.AppSettings["ShadowFile"];
            }
            catch(Exception)
            {
                shadowFile = "shadow.txt";
            }
        }

        public AuthenticationHelper()
        { }

        public bool IsValid(string userName, string password)
        {
            if (String.IsNullOrEmpty(userName) || String.IsNullOrEmpty(password))
            {
                return false;
            }
            return Verify(userName, password);
        }

        bool Verify(string userName, string password)
        {
            bool isValid = false;
            string userLine = null;


            try
            {
                //lock (fileReader)
                //{
                    fileReader = null;
                    fileReader = new StreamReader(shadowFile);
                    userLine = fileReader.ReadLine();
                    while (userLine != null && !userLine.StartsWith(userName+":"))
                    {
                        userLine = fileReader.ReadLine();
                    }
                    fileReader.Close();
                    if (userLine == null)
                    {
                        return isValid;
                    }
                //}
                //Do the verification part

                //Compute the hash
                byte[] sha256Hash = CryptoHelper.GetSHA256Hash(password);
                //Get the hash from the userLine
                string hash = userLine.Substring(userName.Length+1);
                string[] hashByteStrings = hash.Split(null);

                //Compare them
                if (hashByteStrings != null && sha256Hash.Length == hashByteStrings.Length)
                {
                    isValid = true;
                    for (int i = 0; i < hashByteStrings.Length; i++)
                    {
                        if(!hashByteStrings[i].Equals(sha256Hash[i].ToString()))
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if (isValid)
                    {
                        Logger.Logger.WriteInfo("Password matched");
                    }
                }
            }
            catch (Exception e)
            {
                Logger.Logger.WriteException(e);
            }

            return isValid;
        }
    }
}
