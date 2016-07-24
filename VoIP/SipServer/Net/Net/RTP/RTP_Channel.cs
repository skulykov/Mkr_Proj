using System;
using System.Collections.Generic;
using System.Text;

namespace MkrSoft.Net.RTP
{
    public class RTP_Channel
    {
        public RTP_Channel()
        {
        }

        // Send RTP packet.
        public void Send()
        {
        }


        public event EventHandler ReceivedRtpPacket = null;

    }
}
