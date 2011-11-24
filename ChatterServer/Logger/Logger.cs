using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Chatter.Logger
{
    public class Logger
    {
        static string infoPrefix = "INFO: ";
        static string errorPrefix = "ERROR: ";
        static string exceptionPrefix = "FATAL ERROR: ";
        static StreamWriter logWriter = null;
        static bool disableLogging = false;

        static void WriteText(string outPut)
        {
            lock (logWriter)
            {
                if (!disableLogging)
                {
                    if (logWriter == null)
                    {
                        throw new InvalidOperationException("Logger object not initialized");
                    }
                    try
                    {
                        logWriter.WriteLine("{0}", outPut);
                        logWriter.Flush();
                    }
                    catch (Exception e)
                    {
                        //This universal Exception filter is to avoid deadlock on the streamwriter
                        Console.WriteLine("Unable to write: {0}, with Logger, exception eccured: {1}", outPut, e.Message);
                    }
                }
            }
        }

        public static void DisableLogging()
        {
            lock (logWriter)
            {
                if (logWriter == null)
                {
                    disableLogging = true;
                }
            }
        }

        public static bool InitializeLogger(string fileName)
        {
            bool isSucess = true;
            if (disableLogging)
            {
                isSucess = false;
            }
            else if (logWriter == null)
            {
                try
                {
                    logWriter = new StreamWriter(fileName);
                }
                catch (Exception e)
                {
                    isSucess = String.IsNullOrEmpty(fileName);
                    logWriter = new StreamWriter(Console.OpenStandardOutput());
                }
            }

            return isSucess;
        }

        public static void WriteInfo(string message)
        {
            WriteText(infoPrefix + message);
        }

        public static void WriteError(string message)
        {
            WriteText(errorPrefix + message);
        }

        public static void WriteException(Exception e)
        {
            WriteText(exceptionPrefix + e.Message);
        }
    }
}
