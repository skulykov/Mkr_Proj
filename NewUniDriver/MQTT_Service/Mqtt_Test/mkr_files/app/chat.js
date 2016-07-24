host = '23.95.120.21';	// hostname or IP address
//host = '127.0.0.1';	// hostname or IP address
// host = '172.16.153.110';	// hostname or IP address
port = 9001;
lgn_topic = 'mkr/test/login/';		// topic to subscribe to login ( registration )
useTLS = false;
username = null;
password = null;
// username = "jjolie";
// password = "aa";
cleansession = true;
my_name="Dispatcher",my_did="admin";
chat_topic = 'mkr/test/admin';	


//=====================MQTT===================================
 var mqtt;
    var reconnectTimeout = 2000;

    function MQTTconnect() {
        mqtt = new Paho.MQTT.Client(
                        host,
                        port,
                        "web_" + parseInt(Math.random() * 100,
                        10));
        var options = {
            timeout: 3,
            useSSL: useTLS,
            cleanSession: cleansession,
            onSuccess: onConnect,
            onFailure: function (message) {
                dbg("Connection failed: " + message.errorMessage + "Retrying");
                setTimeout(MQTTconnect, reconnectTimeout);
            }
        };

        mqtt.onConnectionLost = onConnectionLost;
        mqtt.onMessageArrived = onMessageArrived;

        if (username != null) {
            options.userName = username;
            options.password = password;
        }
        console.log("Host="+ host + ", port=" + port + " TLS = " + useTLS + " username=" + username + " password=" + password);
        mqtt.connect(options);
    }

    function onConnect() {
        dbg('Connected to ' + host + ':' + port);
        // Connection succeeded; subscribe to our topic
        mqtt.subscribe(lgn_topic+"#", {qos: 0,invocationContext:{mid:11},onSuccess:onSubscribe});  // subscribe to registration topic
        mqtt.subscribe(chat_topic, {qos: 0,invocationContext:{mid:12},onSuccess:onSubscribe});
		
		dbg("subscribe to two topics..");
    }

    function onConnectionLost(response) {
        setTimeout(MQTTconnect, reconnectTimeout);
       dbg("connection lost: " + response.errorMessage + ". Reconnecting");

    };

    function onMessageArrived(message) {

        var topic = message.destinationName;
        var msg = message.payloadString;

        dbg("Got msg on topic:"+topic+"  Msg:"+msg);
		
		var OL = JSON.parse(msg);
		for(var i=0;i<OL.length;i++)
		{
			var O=OL[i];
			if(topic==chat_topic)
			{
				
					var did=O.dId,msg_body=O.msg;
					if(did!=my_did)
					{
						var u=findUser(did);
						var desc=u?u.desc:"unknown";
						addChatMessage(msg_body,desc);
					}
					else
					 dbg("Got msg -> NOT FOR ME");
					
				
			}
			else if(topic.indexOf(lgn_topic)!=-1)
			{
				
				var did=O.dId,name=O.dN;
					if(did!=my_did)
					{
						addNewUser(did,name);
					}
					
					
			}
		}
    }
    
	function onSubscribe(obj)
	{
		var mid=obj.invocationContext.mid;
		//alert("OnSubscribe..OK "+obj.invocationContext.mid);
		if(mid==11)
		{   // sending login message
			_sendMsg(lgn_topic+my_did,"1",true);
		}else if(mid==12)
		{
			//alert(" mid="+mid);
			dbg("Subscribed to CHAT .. OK");
		}
	}
    function _sendMsg(topic,msg,retain)
	{
		var lo=[],dt=(new Date()).toISOString();
			var s={mT:1,dId:my_did,dN:my_name,msg:msg,dt:dt};
			lo.push(s);
			var mm=JSON.stringify(lo);
			mqtt.send(topic,mm,1,retain); 
	}
     
    function sendMsg(did_dest,msg)
	{
		var tt="mkr/test/"+did_dest;
		_sendMsg(tt,msg,false);
	}


//===========================================================
function getUL()
{
   var xx=$$("chat_cb_id");
   return xx.getPopup().getList(); 
}
function findUser(did)
{
   var zz=getUL(),rv=null; 
   var d=zz.data.pull;
	if(d)
	{
			 for (var k in d) {
			
				 var oo=d[k];
				 if(oo)
				 {
					 if(oo.id==did)
					 rv= oo;
					 break;
				 }
			 }
	}
	return rv;
}
//var selectedInd=0;	
function addNewUser(did,desc) {
   
	 if(findUser(did))
	  return;
	var q = { id:did, desc: desc, sel: true};
  	getUL().add(q);

}

//==================chat======================
var chat_open=false,n_msgs=0;
 //var user_name = "Guest " + Math.ceil(Math.random()*9999);
    function chat_template(obj){
		
       // return "<span "+(obj.user == user ? "class='chat_list_item'" : "" )+">"+obj.user+"</span> "+obj.value;
	    return "<span class='chat_list_item'>"+obj.user+"</span> "+obj.value;
    }
    function send_message(){
        var txt = $$("message").getValue();

       // addChatMessage(txt,getWS().id);   // do not add to the list here -> we add it when it comes back from th eserver !!!

        $$("message").setValue("");
        $$("message").focus();
		
		var rid = $$("chat_cb_id").getValue();
		sendMsg(rid,txt)
    }
	function addChatMessage(txt,un)
	{
		if (txt)
            $$("chat_list_id").add({
                user:  un,
                value: txt
            });
	}
//============================================	
var chat_body={
			
				rows:[
				//{ template:"Webix Based Chat", type:"header" },
				{
					cols:[
						{
							view:"combo",
							id:"chat_cb_id", 
							label:"",
							width:400,     //width:500,height:600,
							options:{
							  body:{
								data: [],
								template:function(o)
								{
									 /*
									 var cf="<img height='32' width='32' src='"+cPath+"v_img/"+o.photo+"' style='float:left;'>";
									 var h="<span style='margin-left:10px'>"+o.name+"</span>";
								     return "<div>"+cf+h+"</div>";*/
									
									 var h="<span style='margin-left:10px'>"+o.desc+"</span>";
								     return "<div>"+h+"</div>";
																		}
							  }
							},
							on:{
								'onChange': selectUser
							},

							placeholder:"Select User"
						}, 
						{ view: "icon",id:"chat_sts_id", icon: "" ,width:100 },
					]
				},
				{
					view: "list", id:"chat_list_id", gravity:3,
					type:{ height:"auto" },
					template:chat_template
				},
				{ cols: [
					{ view:"text", id:"message", placeholder:"Type message here", gravity: 3},
					{ view:"button", value: "Send", click:send_message }
				]}
			], type:"space", margin:2
		};
 function selectUser(id)
 {  
   //alert("user id="+id);
   
 }
 function setChatSts(f)
 {
 	var x = $$("chat_sts_id"),ic="";
	if(f) ic="plug";
    x.define("icon", ic);
	x.refresh();		
 }
function init_chat()
{
	 MQTTconnect();  //--> 
	//==============chat====================
	$$("msg_pop").attachEvent("onShow", function(){
    //alert("onShow chart..");
	 
	  updateNMsgs(0);
	  chat_open=true,n_msgs=0;
	});
	$$("msg_pop").attachEvent("onHide", function(){
    //alert("onHide chart.."); 
	chat_open=false,n_msgs=0;
	});
	webix.UIManager.addHotKey("Enter", send_message, $$("message"));
    webix.UIManager.setFocus($$("message"));

   //setChatSts(1);
}