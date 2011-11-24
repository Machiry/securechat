using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Numerics;
using System.Linq;
using System.Text;
using System.IO;
using Security.Cryptography;
using System.Security.Cryptography;
//Application Specific references
using Chatter.Logger;

namespace Chatter.DHHelper
{
    public class DHHelper
    {
        //AesManaged AesCrypto;
        RijndaelManaged AesCrypto;

        BigInteger publicBase;

        public BigInteger PublicBase
        {
            get { return publicBase; }
        }

        BigInteger publicPrime;

        public BigInteger PublicPrime
        {
            get { return publicPrime; }
            set { publicPrime = value; }
        }

        BigInteger privateExponent;

        BigInteger privatePower;

        public BigInteger PrivatePower
        {
            get { return privatePower; }
        }

        BigInteger secretKey;

        public DHHelper(bool isClient)
        {
            if (isClient)
            {
                //Compute the public prime number i.e P
                this.publicPrime = PrimeNumberHelper.GetPrimeNumber();

                Random rand = new Random((int)DateTime.Now.Ticks);

                //Select Random Value, i.e g
                this.publicBase = PrimeNumberHelper.GetRandInRange(this.publicPrime);//new BigInteger(rand.Next(1, int.MaxValue));

                //This is the exponent i.e selected uniquely 
                this.privateExponent = PrimeNumberHelper.GetRandInRange(this.publicPrime);//new BigInteger(rand.Next(1, int.MaxValue));

                this.privatePower = BigInteger.ModPow(this.publicBase, this.privateExponent, this.publicPrime);

                Logger.Logger.WriteInfo("Diffie-Hellman client instance initialized");
            }

            this.AesCrypto = new System.Security.Cryptography.RijndaelManaged();
            this.AesCrypto.Padding = PaddingMode.None;

        }

        public void SetDHValues(BigInteger publicPrime, BigInteger publicBase)
        {
            this.publicBase = publicBase;
            this.publicPrime = publicPrime;

            Random rand = new Random((int)DateTime.Now.Ticks);

            this.privateExponent = new BigInteger(rand.Next(1, int.MaxValue));

            this.privatePower = BigInteger.ModPow(this.publicBase, this.privateExponent, this.publicPrime);

            Logger.Logger.WriteInfo("Diffie-Hellman server instance initialized");
            
        }

        public bool TryComputeSecretKey(BigInteger senderExponent)
        {
            bool isSuccess = true;
            try
            {
                this.secretKey = BigInteger.ModPow(senderExponent, this.privateExponent, this.publicPrime);
                PasswordDeriveBytes pwdDerivedBytes = new System.Security.Cryptography.PasswordDeriveBytes(this.secretKey.ToByteArray(), this.secretKey.ToByteArray());
                this.AesCrypto.Key = pwdDerivedBytes.GetBytes(16);
                this.AesCrypto.IV = pwdDerivedBytes.GetBytes(16);
                this.AesCrypto.Padding = System.Security.Cryptography.PaddingMode.PKCS7;
                Logger.Logger.WriteInfo("Secret key computed");
                
            }
            catch (Exception e)
            {
                isSuccess = false;
                Logger.Logger.WriteException(e);
            }

            return isSuccess;
        }

        public string GetCipherText(string plainText)
        {
            MemoryStream mem = new MemoryStream();
            byte[] plainTextB = Encoding.ASCII.GetBytes(plainText);
            CryptoStream cipherTextStream = new System.Security.Cryptography.CryptoStream(mem, this.AesCrypto.CreateEncryptor(), CryptoStreamMode.Write);
            StreamWriter cwrit = new StreamWriter(cipherTextStream);
            cwrit.WriteLine(Convert.ToBase64String(plainTextB));
            cwrit.Flush();
            cwrit.Close();
            cipherTextStream.Close();
            string cipherText = Convert.ToBase64String(mem.ToArray());
            mem.Close();
            return cipherText;
        }

        public string GetPlainText(string cipherText)
        {
            MemoryStream mem = new MemoryStream(Convert.FromBase64String(cipherText));
            CryptoStream cipherTextStream1 = new System.Security.Cryptography.CryptoStream(mem, AesCrypto.CreateDecryptor(), CryptoStreamMode.Read);
            StreamReader sreader = new StreamReader(cipherTextStream1);
            string b64CipherText = sreader.ReadLine();
            sreader.Close();
            cipherTextStream1.Close();
            byte[] plainTextB = Convert.FromBase64String(b64CipherText);
            mem.Close();
            string plainText = Encoding.ASCII.GetString(plainTextB, 0, plainTextB.Length);
            return plainText;
        }

    }
}
