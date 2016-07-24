using System;

using System.Web;


using Twilio;
using Twilio.TwiML;
using System.Xml.Linq;

namespace TwilioWeb
{

    class MyLog
    {
        public static void Log(string m)
        {
        }
    }
    /// <summary>
    /// Summary description for rpdf
    /// </summary>
    public class Twilio : IHttpHandler
    {
       
        //--------------------------------------------
              
       
        //my_bsm_twilio_app_name
        //AP34cc430e163631e62b1491676b02a496
        public void ProcessRequest(HttpContext context)
        {
            if (context.Request.HttpMethod == "GET")
            {
                try
                {
                    string req1 = context.Request.QueryString["req"];
                    MyLog.Log("Method...Req=" + req1.ToString());
                    MyLog.Log("Req=" + context.Request.QueryString.ToString());
                    //req=call&Called=%2b12057196849&ToState=AL&CallerCountry=CA&Direction=inbound&CallerState=ON&ToZip=&CallSid=CA2767a6d7be9fe4adceec844dabb81746&To=%2b12057196849&CallerZip=M4W+3R8&ToCountry=US&ApiVersion=2010-04-01&CalledZip=&CalledCity=&CallStatus=ringing&From=%2b14164600203&AccountSid=AC4c7d33ff8cb0cf5063d2e7588c74aa9c&CalledCountry=US&CallerCity=TORONTO&Caller=%2b14164600203&FromCountry=CA&ToCity=&FromCity=TORONTO&CalledState=AL&FromZip=M4W+3R8&FromState=ON
                    if (req1=="token")
                    {
                        string clientName = "my_bsm_twilio_app_name";
                        string app_sid = "AP34cc430e163631e62b1491676b02a496";
                        // Create a TwilioCapability object passing in our credentials.
                        var capability = new TwilioCapability("AC4c7d33ff8cb0cf5063d2e7588c74aa9c", "f84a16fed82a4faff16e304c8b8fda56");

                        // Specify that this token allows receiving of incoming calls
                        capability.AllowClientIncoming(clientName);
                        capability.AllowClientOutgoing(app_sid);
                        // Return the token as text
                        context.Response.Write(capability.GenerateToken());

                    }
                    else if (req1 == "call")
                    {
                        // string response= "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>< Response > < Say > Hello Bsm Wireless </ Say ></ Response > ";
                        var response = new TwilioResponse();
                        string clientName = "my_bsm_twilio_app_name";
                        response.Dial(new Client(clientName));
                        XDocument data = null;
                        if (response != null)
                            data = response.ToXDocument();
                        ExecuteResult(data, context);

                    }
                    else if (req1 == "app_call")
                    {
                        InitiateCall(context);
                    }

                }
                catch (Exception ex)
                {
                    MyLog.Log("Twilio EXC=" + ex.Message);
                    context.Response.Write(/*ex.Message*/"??");
                }
                return;
            }
            
        }

        /*
        Method...Req=app_call
req=app_call&ApplicationSid=AP34cc430e163631e62b1491676b02a496&ApiVersion=2010-04-01&Called=&Caller=client%3amy_bsm_twilio_app_name&CallStatus=ringing&CallSid=CA86c7dee52efc0aa8b10d2221b30fd719&To=905-553-6523&From=client%3amy_bsm_twilio_app_name&Direction=inbound&AccountSid=AC4c7d33ff8cb0cf5063d2e7588c74aa9c
  ascii 0x3A :
        */

        // /Client/InitiateCall?source=5551231234&target=5554561212
        void InitiateCall(HttpContext context)
        {

            string source = context.Request.QueryString["Caller"]; ;
            string target = context.Request.QueryString["To"]; ;
            if (source.Contains(":"))
            {
                string[] s1 = source.Split(':');
                source = s1[1];
            }
            var response = new TwilioResponse();

            // Add a <Dial> tag that specifies the callerId attribute
            response.Dial(target, new { callerId = source });

            XDocument data = null;
            if (response != null)
                data = response.ToXDocument();
            ExecuteResult(data, context);
        }


        void ExecuteResult(XDocument data, HttpContext context)
		{
			
			context.Response.ContentType = "application/xml";

			if (data == null)
			{
				data = new XDocument(new XElement("Response"));
			}

			data.Save(context.Response.Output);
		}
       

        public bool IsReusable
        {
            get
            {
                return false;
            }
        }
    }
}