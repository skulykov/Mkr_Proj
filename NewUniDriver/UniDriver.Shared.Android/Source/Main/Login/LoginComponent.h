/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef LOGINCOMPONENT_H_INCLUDED
#define LOGINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"
#include "../MainAppWindow.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class LoginComponent : public Component, public ButtonListener, public Thread
{
public:
    //==============================================================================
    LoginComponent();
    ~LoginComponent();

    void paint (Graphics&);
    void resized();

	void buttonClicked(Button* buttonThatWasClicked);

	void run() override;
	String getResultText(const URL& url);
	void createConnect();
	TextEditor *getTextEditor1() { return tedit1; }
	TextEditor *getTextEditor2() { return tedit2; }
private:
	bool focusGiven;
	//ScopedPointer<ToggleButton> toggleButton;
	TextButton* textButton;
	
	TextEditor *tedit1;
	TextEditor *tedit2;
	ToggleButton *cb;
	KeyListener  *k_listener;
    //==============================================================================
	static void response_cb(void *o);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
