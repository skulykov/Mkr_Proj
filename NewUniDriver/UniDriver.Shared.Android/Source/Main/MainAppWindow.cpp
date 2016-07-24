#include "MainAppWindow.h"
#include "MainComponent.h"

MainAppWindow::MainAppWindow(String name) : DocumentWindow(name,
	Colours::lightgrey,
	DocumentWindow::allButtons)
{
	setUsingNativeTitleBar(true);
	//this->currentComponent = new MainContentComponent();
	setContentOwned(new MainContentComponent(), true);

	centreWithSize(getWidth(), getHeight());
	setVisible(true);
}
void MainAppWindow::SetComponent(Component *comp)
{
	setContentOwned(comp, true);
	/*
	if (this->currentComponent != NULL)
	{
		delete this->currentComponent;
		this->currentComponent = NULL;
	}
	this->currentComponent = comp;
	*/
}

