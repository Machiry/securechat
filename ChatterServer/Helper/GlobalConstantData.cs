using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;

namespace Chatter.Helper
{
    public class GlobalConstants
    {
        public static readonly string AuthSuccessMessage = "AUTH_SUCESS";
        public static readonly string AuthFailureMessage = "AUTH_FAILED";
        public static readonly string ProbeMessage = "R-U-ALIVE";
        public static readonly string AuthFailureNoRetryMessage = "AUTH_FAILED_NO_RETRY";
        public static readonly string MaxConnectionsReached = "MAX_CONNECTIONS_REACHED";

        //This is teh interval during which client will be probed for liveness
        //if the client is not live then the connection will be torn down
        static int probeInterval = 0;

        static int timeOutForAuthentication = 0;

        public static int ProbeInterval
        {
            get
            {
                if (probeInterval == 0)
                {
                    if ((new List<string>(ConfigurationManager.AppSettings.AllKeys)).Contains("ProbeInterval"))
                    {
                        int interval = 0;
                        if (!int.TryParse(ConfigurationManager.AppSettings["ProbeInterval"], out interval) || interval < 2)
                        {
                            probeInterval = 5;
                        }
                        else
                        {
                            probeInterval = interval;
                        }
                    }
                    else
                    {
                        probeInterval = 5;
                    }
                }
                return probeInterval;
            }
        }

        public static int TimeOutForAuthentication
        {
            get
            {
                if (timeOutForAuthentication == 0)
                {
                    if ((new List<string>(ConfigurationManager.AppSettings.AllKeys)).Contains("TimeOutForAuthentication"))
                    {
                        int interval = 0;
                        if (!int.TryParse(ConfigurationManager.AppSettings["TimeOutForAuthentication"], out interval) || interval < 30)
                        {
                            timeOutForAuthentication = 60;
                        }
                        else
                        {
                            timeOutForAuthentication = interval;
                        }
                    }
                    else
                    {
                        timeOutForAuthentication = 60;
                    }
                }
                return timeOutForAuthentication;
            }
        }

    }
}
