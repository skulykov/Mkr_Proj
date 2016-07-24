/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "Login/LoginComponent.h"
#include "NetTcp.h"
#include "CommonCls.h"

#include "db_interface.h"
#include "ToggleButtonLook.h"
//==============================================================================

MainContentComponent::MainContentComponent() : Thread("Network Demo"), textButton(0)
{
	//addAndMakeVisible(toggleButton = new ToggleButton("toggle button"));
	//textButton->addListener(this);
	
	addAndMakeVisible(textButton = new TextButton("new button"));
	textButton->addListener(this);
	textButton->setButtonText(L"CLEAR MC");

	addAndMakeVisible(tedit1 = new TextEditor("new_te1"));
	addAndMakeVisible(tedit2 = new TextEditor("new_te2"));
	Font font=tedit1->getFont();
	font.setHeight(20.0);
	tedit1->setFont(font);
	//tedit1->addKeyListener(new MyKeyListener(this));
	tedit2->setPasswordCharacter(0x2022);
	//tedit2->setEscapeAndReturnKeysConsumed(true);
	//tedit2->addKeyListener(new MyKeyListener(this));
	//tedit1->addListener(this);
	//tedit1->setKeyboardType(TextInputTarget::VirtualKeyboardType::textKeyboard);
	

	addAndMakeVisible(cb = new ToggleButton("new_cb"));
	cb->setButtonText(L"Remember Me");
	cb->setColour(ToggleButton::ColourIds::textColourId, Colours::yellow);
	cb->setLookAndFeel(new ToggleButtonLook());

	addAndMakeVisible(label = new Label("mLabel"));
	label->setText(L"Response",NotificationType::dontSendNotification);
	label->setColour(Label::ColourIds::textColourId, Colours::yellow);
	setSize (320, 480);
}

MainContentComponent::~MainContentComponent()
{
	try {
		if (textButton != nullptr)
			delete textButton;
		if (label != nullptr)
			delete label;
		stopThread(20);

		//clear_employee_db();
	}
	catch (...) {}
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff001F36));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);

	//tedit1->setWantsKeyboardFocus(true);
	if (!/*tedit1->hasKeyboardFocus(true)*/focusGiven  && tedit1->isVisible())
	{
		tedit1->grabKeyboardFocus();
		DBG("Focus given");
		focusGiven = true;
	}

}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	textButton->setBounds(10, 32, 150, 44);
	tedit1->setBounds(10, 90, 250, 32);
	tedit2->setBounds(10, 130, 250, 32);
	cb->setBounds(10, 180, 250, 32);
	//cb->setBounds(proportionOfWidth(0.5867f), proportionOfHeight(0.3800f), proportionOfWidth(0.2500f), proportionOfHeight(0.060f));
	label->setBounds(10, 280, 150, 124);
}

void MainContentComponent::run()
{
	/*
	String result(getResultText(String("http://www.google.com")));

	MessageManagerLock mml(this);

	if (mml.lockWasGained())
		label->setText(result, NotificationType::dontSendNotification);//	resultsBox.loadContent(result);
		*/
	createConnect();
}
String MainContentComponent::getResultText(const URL& url)
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

void get_time()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	int64 millisSinceEpoch= ((int64)tv.tv_sec) * 1000 + tv.tv_usec / 1000;


	struct tm *tinfo;
	time_t rawtime;
	time(&rawtime);
	tinfo = localtime(&rawtime);

	char *str1 = asctime(tinfo);
	if (str1 != 0)
	{
	}
}


void MainContentComponent::createConnect()
{
 	//NetTcp::LoginRequest(juce::String("K123"), juce::String("1234"), 1032, response_cb);
	/*get_time();
	juce::Time t1 = juce::Time::getCurrentTime();
	String tzone=t1.getTimeZone();
	String strT = t1.toString(true, true);
	if (strT != "")
	{
	}
	EmployeeRow * emp = read_employee("1008");
	if (emp != 0)
	{
		delete emp;
	}*/
}

void MainContentComponent::response_cb(void *obj)
{
	
}

//---------------------------------------------

void MainContentComponent::buttonClicked(Button* buttonThatWasClicked)
{
	//EmployeeRow * emp = read_employee(/*lgn.driverProf.ID.toStdString()*/"1000");
	//if (emp != 0)
	//{
	//	delete emp;
	//}
	////clear_employee_db();
	test_db();
	return;
	
	
	JUCE_Mkr_MobileApplication *inst=(JUCE_Mkr_MobileApplication *)JUCE_Mkr_MobileApplication::getInstance();
	if (inst != NULL)
	{
		inst->SetMainComponent(new LoginComponent());
	}
	return;
	/*
	if (buttonThatWasClicked == toggleButton)
	{
		//[UserButtonCode_toggleButton] -- add your button handler code here..
		//[/UserButtonCode_toggleButton]
	}
	else */
	if (buttonThatWasClicked == textButton)
	{

		String str1 = tedit1->getText();
		String str2 = tedit2->getText();

		startThread();
		//[UserButtonCode_textButton] -- add your button handler code here..
		//[/UserButtonCode_textButton]
	}

	//[UserbuttonClicked_Post]
	//[/UserbuttonClicked_Post]
}
