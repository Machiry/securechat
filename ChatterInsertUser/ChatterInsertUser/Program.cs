using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;

namespace ChatterInsertUser
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 3)
            {
                string userName = args[1];
                if (!IsUserNameValid(userName))
                {
                    Usage();
                    return;
                }
               
                byte[] sha256Hash = CryptoHelper.GetSHA256Hash(args[2]);
                string passwordHash = null;
                for (int i = 0; i < sha256Hash.Length; i++)
                {
                    passwordHash = passwordHash + sha256Hash[i].ToString() + " ";
                }
                StreamWriter writer = new StreamWriter(args[0], true);
                writer.WriteLine(userName + ":" + passwordHash.Trim());
                Console.WriteLine("Username and SHA of password inserted into the provided file");
                writer.Close();
            }
            else
            {
                Console.WriteLine("Invalid number of arguments");
                Usage();
            }
        }

        static void Usage()
        {
            Console.WriteLine("Usage: ChatterInsertUser.exe <shadow file> <username> <password>");
            Console.WriteLine("Where:");
            Console.WriteLine("   <shadow file> the file in which store the username and the");
            Console.WriteLine("                 hash value of the password");
            Console.WriteLine("   <username>    username: must be of 3-15 charecters and cannot");
            Console.WriteLine("                 contain spaces, tabs or hyphens");
            Console.WriteLine("   <password>    password for the user (max length 15 characters)");
            
        }

        static bool IsUserNameValid(string userName)
        {
            if (userName.Length < 3 || userName.Length > 15 || (userName.Contains('-') || userName.Contains(' ') || userName.Contains('\t')))
            {
                Console.WriteLine("Username is not valid");
                Console.WriteLine("Username must be of 3-15 charecters and cannot contain spaces, tabs or hyphens (-)");
                if (DateTime.Now.Ticks % 2 == 0)
                {
                    Console.WriteLine("Btw, nice try!");
                }
                return false;
            }
            return true;
        }

        static bool IsPasswordValid(string password)
        {
            password.Trim();
            if (String.IsNullOrEmpty(password) || password.Length > 15 || password.Contains(' ') || password.Contains('\t'))
            {
                if (DateTime.Now.Ticks % 2 == 0)
                {
                    Console.WriteLine("Are you trying to break me?!");
                }
                return false;
            }
            return true;
        }
    }
}
