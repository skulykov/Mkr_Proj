using System;
using System.Collections.Generic;
using System.Text;

namespace MkrSoft.Net.Dns.Client
{
	/// <summary>
	/// 
	/// </summary>
	internal enum OPCODE
	{
		/// <summary>
		/// A standard query.
		/// </summary>
		QUERY = 0,       

		/// <summary>
		/// An inverse query.
		/// </summary>
		IQUERY = 1,

		/// <summary>
		/// A server status request.
		/// </summary>
		STATUS = 2, 		
	}
}
