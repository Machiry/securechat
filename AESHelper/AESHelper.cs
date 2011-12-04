using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Security.Cryptography;

namespace AESHelper
{
    public class AESHelper
    {
        List<Byte> keyBytes;

        long initialSeed;
        int keyLength;

        List<Byte> iV;

        public List<Byte> IV
        {
            get { return iV; }
        }

        public List<Byte> Key
        {
            get { return keyBytes; }
        }

        RijndaelManaged AesCrypto = new RijndaelManaged();

        public AESHelper(int keyLength)
        {
            if (keyLength < 0 || (keyLength != 128 && keyLength != 192 && keyLength != 256))
            {
                new InvalidOperationException("Invalid Key Length Specified For AES, Valid key lengths are: 128,192 or 256 bits");
            }
            Random randGen = new Random((int)DateTime.Now.Ticks);
            int number = randGen.Next();
            PasswordDeriveBytes pwdDerivedBytes = new System.Security.Cryptography.PasswordDeriveBytes(BitConverter.GetBytes(number), BitConverter.GetBytes(number));
            this.keyBytes = new List<byte>();
            this.keyBytes.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
            this.iV = new List<byte>();
            this.iV.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
        }

        public AESHelper(int keyLength, long initialSeed)
        {
            if (keyLength < 0 || (keyLength != 128 && keyLength != 192 && keyLength != 256))
            {
                new InvalidOperationException("Invalid Key Length Specified For AES, Valid key lengths are: 128,192 or 256 bits");
            }
            else
            {
                this.keyLength = keyLength;
                this.initialSeed = initialSeed;
                Random randGen = new Random((int)initialSeed);
                int number = randGen.Next();
                PasswordDeriveBytes pwdDerivedBytes = new System.Security.Cryptography.PasswordDeriveBytes(BitConverter.GetBytes(number), BitConverter.GetBytes(number));
                this.keyBytes = new List<byte>();
                this.keyBytes.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
                this.iV = new List<byte>();
                this.iV.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
            }
        }

        public AESHelper(List<Byte> key)
        {
            if (key != null && (key.Count == 16 || key.Count == 24 || key.Count == 32))
            {
                this.keyBytes = key;
                this.keyLength = key.Count;
                Random randGen = new Random((int)initialSeed);
                int number = randGen.Next();
                PasswordDeriveBytes pwdDerivedBytes = new System.Security.Cryptography.PasswordDeriveBytes(BitConverter.GetBytes(number), BitConverter.GetBytes(number));
                this.iV = new List<byte>();
                this.iV.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
            }
            else
            {
                new InvalidOperationException("Invalid Key Length Specified For AES, Valid key lengths are: 128,192 or 256 bits");
            }
            
        }

        public AESHelper()
        {
            //Default Key Length : 16*8 bits
            this.keyLength = 16;
            Random randGen = new Random((int)DateTime.Now.Ticks);
            int number = randGen.Next();
            PasswordDeriveBytes pwdDerivedBytes = new System.Security.Cryptography.PasswordDeriveBytes(BitConverter.GetBytes(number), BitConverter.GetBytes(number));
            this.keyBytes = new List<byte>();
            this.keyBytes.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
            this.iV = new List<byte>();
            this.iV.AddRange(pwdDerivedBytes.GetBytes(this.keyLength));
        }

        public string EncryptText(string plainText)
        {
            string cipherText = null;
            MemoryStream mem = new MemoryStream();
            byte[] plainTextB = Encoding.ASCII.GetBytes(plainText);
            this.AesCrypto.Key = this.keyBytes.ToArray();
            this.AesCrypto.IV = this.iV.ToArray();
            CryptoStream cipherTextStream = new System.Security.Cryptography.CryptoStream(mem, this.AesCrypto.CreateEncryptor(), CryptoStreamMode.Write);
            StreamWriter cwrit = new StreamWriter(cipherTextStream);
            cwrit.WriteLine(Convert.ToBase64String(plainTextB));
            cwrit.Flush();
            cwrit.Close();
            cipherTextStream.Close();
            cipherText = Convert.ToBase64String(mem.ToArray());
            mem.Close();
            return cipherText;
        }

        public string DecryptText(string cipherText)
        {
            string plainText = null;
            this.AesCrypto.Key = this.keyBytes.ToArray();
            this.AesCrypto.IV = this.iV.ToArray();
            MemoryStream mem = new MemoryStream(Convert.FromBase64String(cipherText));
            CryptoStream cipherTextStream1 = new System.Security.Cryptography.CryptoStream(mem, AesCrypto.CreateDecryptor(), CryptoStreamMode.Read);
            StreamReader sreader = new StreamReader(cipherTextStream1);
            string b64CipherText = sreader.ReadLine();
            sreader.Close();
            cipherTextStream1.Close();
            byte[] plainTextB = Convert.FromBase64String(b64CipherText);
            mem.Close();
            plainText = Encoding.ASCII.GetString(plainTextB, 0, plainTextB.Length);
            return plainText;
        }
    }
}
