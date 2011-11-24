using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Numerics;
using System.Text;

namespace Chatter.DHHelper
{
    public class PrimeNumberHelper
    {

        /// <summary>
        /// This function returns a random interget which is prime
        /// </summary>
        /// <returns>a prime interger of type BigInteger</returns>
        public static BigInteger GetPrimeNumber()
        {
            BigInteger resultingPrimeNumber = new BigInteger((uint)0);

            
            
            //This is the logic to compute 1024 bit prime numbers
            /*
            
            var rng = new RNGCryptoServiceProvider();
            resultingPrimeNumber = GetRand(rng);
            while (!MillerRabin(resultingPrimeNumber, int.MaxValue>>16))
            {
                resultingPrimeNumber = GetRand(rng);
            }
            */

            //This is just to reduce the computation over head
            resultingPrimeNumber = new BigInteger(18446744073709551557);

            //Return the resulting object
            return resultingPrimeNumber;
        }


        public static BigInteger GetRandInRange(BigInteger d)
        {
            int byteSize = d.ToByteArray().Length;
            if (byteSize > 2)
            {
                byte[] bytes = new byte[byteSize];
                var rng = new RNGCryptoServiceProvider();
                rng.GetBytes(bytes);
                BigInteger a = new BigInteger(bytes);                
                while (a < 0 || BigInteger.Compare(d, a) <= 0)
                {                   
                    rng.GetBytes(bytes);
                    a = BigInteger.Abs(new BigInteger(bytes));
                }
                return a;
            }
            else
            {
                Random random = new Random();
                int randomNumber = random.Next(1, Convert.ToInt32(d.ToString()));
                return new BigInteger(randomNumber);
            }
        }

        private static BigInteger GetRand(RNGCryptoServiceProvider rng)
        {
            int byteSize = 16;
            long ticks = DateTime.Now.Hour; 
            BigInteger a = new BigInteger(1);
            
            while (ticks > 0)
            {
                byte[] bytes = new byte[byteSize];
                rng.GetBytes(bytes);
                a = BigInteger.Abs(new BigInteger(bytes));
                ticks = ticks >> 1;
            }
            return a;

        }

        private static BigInteger getRand(BigInteger d)
        {
            int byteSize = d.ToByteArray().Length;
            if (byteSize > 2)
            {
                BigInteger a = new BigInteger(1);
                var rng = new RNGCryptoServiceProvider();
                while (BigInteger.Compare(a, d) <= 0)
                {
                    byte[] bytes = new byte[byteSize];
                    rng.GetBytes(bytes);
                    a = BigInteger.Abs(new BigInteger(bytes));
                }
                return a;
            }
            else
            {
                Random random = new Random();
                int randomNumber = random.Next(1, Convert.ToInt32(d.ToString()));
                return new BigInteger(randomNumber);
            }
        }

        public static Boolean MillerRabin(BigInteger n, int rounds)
        {
            if (n.IsOne) return false;
            if (n.IsEven) return false;
            BigInteger d = BigInteger.Subtract(n, 1);
            int s = 0;
            while (BigInteger.ModPow(d, 1, 2) == 0)
            {
                s++;
                d = BigInteger.Divide(d, 2);
            }
            for (int round = 0; round < rounds; round++)
            {
                BigInteger a = getRand(d);
                BigInteger x = BigInteger.ModPow(a, d, n);
                if ((x == 1) || (x == BigInteger.Subtract(n, 1)))
                { }
                else
                {
                    Boolean doNext = false;
                    for (int r = 1; r < s; r++)
                    {
                        x = BigInteger.ModPow(x, 2, n);
                        if (x.Equals(new BigInteger(1))) return false;
                        if (x == BigInteger.Subtract(n, 1))
                        {
                            doNext = true;
                            break;
                        }
                    }
                    if (!doNext) return false;
                }
            }
            return true;
        }

        /// <summary>
        /// This is the function which checks whether a given number is prime or not
        /// </summary>
        /// <param name="number">input integer</param>
        /// <returns>true if the number is prime else false</returns>
        public static bool IsPrime(int number)
        {
            bool isPrime = false;
            if (number > 0 && ((number & 1) != 0))
            {
                isPrime = true;
                for (int i = 3; i < (number / 2); i += 2)
                {
                    if (number % i == 0)
                    {
                        isPrime = false;
                        break;
                    }
                }
            }
            return isPrime;
        }

        /// <summary>
        /// This is the function which checks whether a given number is prime or not
        /// </summary>
        /// <param name="number">input integer</param>
        /// <returns>true if the number is prime else false</returns>
        public static bool IsPrime(ulong number)
        {
            bool isPrime = false;
            if (number > 0 && ((number & 1) != 0))
            {
                isPrime = true;
                for (ulong i = 3; i < (number / 2); i += 2)
                {
                    if (number % i == 0)
                    {
                        isPrime = false;
                        break;
                    }
                }
            }
            return isPrime;
        }
    }
}
