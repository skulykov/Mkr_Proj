var mTb,mTv, gO,AJX, _URL = "http://www.mikerel.com/", URL = _URL + "nReq.ashx",cPath = "mkr_files/common/",tPath = cPath+"icons/tree/",mPath = cPath+"icons/map/",a_sel;
var lgin = ["1", "????", "", 0]/* ["3602", "k123", "", 0]*/, t_hb = "",gMap,gRtg,gRHos,gRIns,gWO;
function gPrms(n) { return "req=" + n + "&cid=" + lgin[0] + "&user=" + lgin[1] + "&rnd=" + g_rand() }
function LO() { window.location = "index.html" }
function onLoad()
{
	t_hb="GGGG";
}
function _onload() {
  

    _onready();
}

function unloadA()
{
	AJX.post(URL,gPrms(8) , function (d) {
        var v = d;
		
    });
}
function unLoad() {}
function _onready() {
  

   
    var toolbar = {
        view: "toolbar",
		css: "highlighted_header header1",
        paddingY: 0,
        cols: [
          
        
             
        ]
    };
   
    var toolbar1 = {
        view: "toolbar",
		//css: "highlighted_header header1",
        paddingY: 0,
        cols: [
          
             { view: "icon",id:"msgs_btn_id",width:60, icon: "comments-o" ,/*badge:12,*/popup:"msg_pop"},
			{ view: "button",id:"tb_btn_2",width:160, type: "iconButton", icon: "plus", label: "" ,disabled:true},
           /* { view: "button", type: "iconButton", icon: "cog", label: "Cog" },*/
            { gravity: 14 },
			{ view: "icon", icon: "search" ,popup:"search_pop"},
            { view: "icon",id:"email_btn_id",width:60, icon: "envelope", /*badge:1 ,*/badge:1,popup:"email_pop"},
            
             
        ]
    };
  
    webix.ui({
        rows: [
            { type: "clean", cols: [toolbar, toolbar1] },
            {
						cols:[
							{ 
							header:"Left list", body:
								{
								view:"list", 
								width:320,
								template:"#rank#. #title#",
								data:[] 
								}
							},

							{ },
							{ view:"resizer" },

							{ 
							view:"list",
							id:"right_ddbg_panel", 
							width:320,
							template:"#rank#. #title#",
							data:[] 
							}
						]
					},
					{ template:"footer", height:50 }
 
            
        ]
    });

    //TRACK("tb1");
   // webix.ui(ui_order);

    //$$("dsc").bind($$('countries'));

  
	window.setTimeout(m_timer,5000);
	
	//init popups
	webix.ui({
		view:"popup",
		id:"msg_pop",
		head:"Submenu",
		width:500,height:600,
		body:chat_body
	});
	var form1 = [
			{ view:"search", value:'', label:""},
			
		];
	webix.ui({
		view:"popup",
		id:"email_pop",
		head:"Submenu",
		width:300,
		body:{
			view:"list", 
			data:[ {id:"1", name:"Daniel Herby", location: "******"},
					{id:"2", name:"Sam Sanders", location:"========"},
					
			],
			datatype:"json",
			template:"#name# - #location#",
			autoheight:true,
			select:true
		}
	});
	webix.ui({
		view:"popup",
		id:"search_pop",
		head:"Submenu",
		width:300,
		body:{ view:"form", scroll:false, width:300, elements: form1 },
	});
	
	
//	g_r_list(null);
	
	init_chat();
	dbg("Hello there");
}//function _onready() {

function dbg(str)
{
	var x=$$("right_ddbg_panel");
	var d=new Date();
	var t=d.format("HH:MM:ss");
	var q={rank:t,title:"-- "+str};
	x.add(q);
}
	

webix.ready(function () {
    AJX=webix.ajax();
	_onload();
 
});

//===========================m timer=========================================================
function m_timer_cb(tt) {
        
		
}
function m_timer() {
}

//=====================toolbar functions call=================
function tbf1()
{
	if(gRtg)gRtg.genRoute();
}
function tbf2()
{
	//if(gRtg)gRtg.addRoute();
	sendAMsg("Test message from Browser!!")
}
function updateNEmails(n) {
    var x = $$("email_btn_id");
    x.define("badge", n);
    x.refresh();
}

function updateNMsgs(n) {
    var x = $$("msgs_btn_id");
    x.define("badge", n);
    x.refresh();
}

//==just for test==========
var nnnn=1;
function sendAMsg(m) {
	    var a=1,r=1001;
        var t_=new Date().toISOString();     // added timestamp
	    var q = { id: nnnn + 1, amsg: m, from: "To Mobile" }, q1 = {aID:a,rID:r,msg:m,ts:t_,type:2}, lo = [];  // type=2 -> FromBrowser
	    lo.push(q1);
	    var p = gPrms(53), p = p + "&json=" + JSON.stringify(lo)+"&aid="+a+"&rid="+r;
	    
	    AJX.post(URL, p, function (d) {
	        if (d == "OK") {
	            alert("Test msg was sent");
	        }
	    });
		nnnn++;
	}