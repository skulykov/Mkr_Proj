/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "LoginComponent.h"
#include "NetTcp.h"
#include "CommonCls.h"

#include "db_interface.h"
#include "ToggleButtonLook.h"
//==============================================================================
class MyKeyListener :public KeyListener
{
	LoginComponent *parent;
public:
	MyKeyListener(LoginComponent *parent)
	{
		this->parent = parent;
	}
	bool keyPressed(const juce::KeyPress& key,Component* originatingComponent)
	{
		int kc = key.getKeyCode();
		int retK = key.returnKey;
		if (kc == retK)
		{
			if (originatingComponent != 0)
			{
				if (originatingComponent == parent->getTextEditor1())
				{
					TextEditor *te = parent->getTextEditor2();  // pass focus to next 
					te->grabKeyboardFocus();
				}
				else if (originatingComponent == parent->getTextEditor2())
				{
					TextEditor *te = parent->getTextEditor2();  // pass focus to next 
					te->unfocusAllComponents();
				}
			}
		}
		return false;
	}
};


LoginComponent::LoginComponent() : Thread("Network Demo"), textButton(0)
{
	//addAndMakeVisible(toggleButton = new ToggleButton("toggle button"));
	//textButton->addListener(this);
	
	addAndMakeVisible(textButton = new TextButton("new button"));
	textButton->addListener(this);
	textButton->setButtonText(L"clear");

	addAndMakeVisible(tedit1 = new TextEditor("new_te1"));
	addAndMakeVisible(tedit2 = new TextEditor("new_te2"));
	Font font=tedit1->getFont();
	font.setHeight(20.0);
	tedit1->setFont(font);
	k_listener = new MyKeyListener(this);
	tedit1->addKeyListener(k_listener);
	tedit2->setPasswordCharacter(0x2022);
	tedit2->setFont(font);
	//tedit2->setEscapeAndReturnKeysConsumed(true);
	tedit2->addKeyListener(k_listener);
	//tedit1->addListener(this);
	//tedit1->setKeyboardType(TextInputTarget::VirtualKeyboardType::textKeyboard);
	

	addAndMakeVisible(cb = new ToggleButton("new_cb"));
	cb->setButtonText(L"Remember Me");
	cb->setColour(ToggleButton::ColourIds::textColourId, Colours::yellow);
	cb->setLookAndFeel(new ToggleButtonLook());


	setSize (320, 480);

	response_cb(0);
}

LoginComponent::~LoginComponent()
{
	if (textButton != nullptr)
		delete textButton;
	if (k_listener != NULL)
		delete k_listener;
	if (cb)
		delete cb;
	if (tedit1)
		delete tedit1;
	if (tedit2)
		delete tedit2;
	stopThread(20);

	//clear_employee_db();
}

void LoginComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff001F36));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("LOGIN COMP", getLocalBounds(), Justification::centred, true);

	//tedit1->setWantsKeyboardFocus(true);
	if (!/*tedit1->hasKeyboardFocus(true)*/focusGiven  && tedit1->isVisible())
	{
		tedit1->grabKeyboardFocus();
		DBG("Focus given");
		focusGiven = true;
		//response_cb(0);
	}

}

void LoginComponent::resized()
{
    // This is called when the LoginComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	
	tedit1->setBounds(10, 90, 250, 32);
	tedit2->setBounds(10, 130, 250, 32);
	cb->setBounds(10, 180, 250, 32);
	//cb->setBounds(proportionOfWidth(0.5867f), proportionOfHeight(0.3800f), proportionOfWidth(0.2500f), proportionOfHeight(0.060f));
	textButton->setBounds(10, 280, 150, 44);
}

void LoginComponent::run()
{
	/*
	String result(getResultText(String("http://www.google.com")));

	MessageManagerLock mml(this);

	if (mml.lockWasGained())
		label->setText(result, NotificationType::dontSendNotification);//	resultsBox.loadContent(result);
		*/
	createConnect();
}
String LoginComponent::getResultText(const URL& url)
{
	StringPairArray responseHeaders;
	int statusCode = 0;

	ScopedPointer<InputStream> stream(url.createInputStream(false, nullptr, nullptr, String(),
		10000, // timeout in millisecs
		&responseHeaders, &statusCode));
	if (stream != nullptr)
		return (statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
		+ "Response headers: " + newLine
		+ responseHeaders.getDescription() + newLine
		+ "----------------------------------------------------" + newLine
		+ stream->readEntireStreamAsString();

	if (statusCode != 0)
		return "Failed to connect, status code = " + String(statusCode);

	return "Failed to connect!";
}
//---------------------------------------------


void LoginComponent::createConnect()
{
	clear_employee_db();
	/*
	String user = tedit1->getText();
	String pass = tedit2->getText();
	NetTcp::LoginRequest(user, pass, -1, response_cb);
	*/
	//response_cb(0);
}

void LoginComponent::response_cb(void *obj)
{
	if (obj != 0)
	{
		TcpRequestObj *o = (TcpRequestObj *)obj;
		juce::String str = o->GetResponse();
		m_printf("\r\nResponse length:%d \r\n", str.length());
		if (!str.isEmpty())
		{
			ClsLoginMD lgn;
			lgn.FromJson(str);
			//save_employee(&lgn.driverProf);
			EmployeeRow * emp = read_employee(/*lgn.driverProf.ID.toStdString()*/"1000");
			if (emp != 0)
			{
				delete emp;
			}
		}
	}
	else
	{
		EmployeeRow * emp = read_employee(/*lgn.driverProf.ID.toStdString()*/"1000");
		if (emp != 0)
		{
			delete emp;
		}
	}
}

//---------------------------------------------
void LoginComponent::buttonClicked(Button* buttonThatWasClicked)
{
	JUCE_Mkr_MobileApplication *inst=(JUCE_Mkr_MobileApplication *)JUCE_Mkr_MobileApplication::getInstance();

	/*
	if (buttonThatWasClicked == toggleButton)
	{
		//[UserButtonCode_toggleButton] -- add your button handler code here..
		//[/UserButtonCode_toggleButton]
	}
	else */
	if (buttonThatWasClicked == textButton)
	{

		startThread();
		//[UserButtonCode_textButton] -- add your button handler code here..
		//[/UserButtonCode_textButton]
	}

	//[UserbuttonClicked_Post]
	//[/UserbuttonClicked_Post]
}
