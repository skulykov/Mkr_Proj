/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"
#include "MainAppWindow.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public Component, public ButtonListener, public Thread
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

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
	Label * label;
	TextEditor *tedit1;
	TextEditor *tedit2;
	ToggleButton *cb;
    //==============================================================================
	void response_cb(void *o);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
