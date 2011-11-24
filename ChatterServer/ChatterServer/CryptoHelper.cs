using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Linq;
using System.Text;

namespace Chatter.ChatterServer
{
    public class CryptoHelper
    {
        static SHA256Managed shaCreator = new SHA256Managed();

        public static byte[] GetSHA256Hash(string text)
        {
            byte[] computedHash = null;
            List<byte> byteList = new List<byte>();
            if (text != null)
            {
                List<char> charArray = new List<char>(text.ToCharArray());
                charArray.ForEach(delegate(char c)
                {
                    byteList.Add((byte)c);
                });
               
            }
            lock (shaCreator)
            {
                computedHash = shaCreator.ComputeHash(byteList.ToArray());
                return computedHash;
            }
        }
    }
}
