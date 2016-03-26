/*  ===========================================================================
*
*   This file is part of HISE.
*   Copyright 2016 Christoph Hart
*
*   HISE is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   HISE is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*
*   Commercial licences for using HISE in an closed source project are
*   available on request. Please visit the project's website to get more
*   information about commercial licencing:
*
*   http://www.hartinstruments.net/hise/
*
*   HISE is based on the JUCE library,
*   which also must be licenced for commercial applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/

ProcessorPopupItem::ProcessorPopupItem(ModulatorSynth *synth, BackendProcessorEditor *mainEditor) :
PopupMenu::CustomComponent(false),
p(synth),
parent(nullptr),
isHeadline(true),
isSolo(false),
usedInList(false),
editor(mainEditor)
{
	isSolo = p->getEditorState("Solo");
    
	initButtons();
}

ProcessorPopupItem::ProcessorPopupItem(Processor *parentProcessor, Processor *p_, BackendProcessorEditor *mainEditor) :
PopupMenu::CustomComponent(false),
parent(parentProcessor),
p(p_),
editor(mainEditor),
isHeadline(false),
isSolo(false),
usedInList(false)
{
	isSolo = p->getEditorState("Solo");

	initButtons();
    
	bypassButton->setEnabled(!parent->isBypassed());
}

void ProcessorPopupItem::mouseDown(const MouseEvent &m)
{
	if (m.mods.isRightButtonDown())
	{
		PopupMenu m;

		enum
		{
			CreateScriptVariable = 1
		};

		PopupLookAndFeel plaf;

		m.setLookAndFeel(&plaf);
		m.addItem(CreateScriptVariable, "Create Script Variable Declaration");

		int result = m.show();

		if (result == CreateScriptVariable)
		{
			ProcessorHelpers::getScriptVariableDeclaration(p);
		}
	}
}


void ProcessorPopupItem::initButtons()
{
	addAndMakeVisible(visibleButton = new ShapeButton("", Colours::white.withAlpha(0.5f),
		Colours::white.withAlpha(0.8f),
		Colours::white));

	Path visiblePath;
	visiblePath.loadPathFromData(BackendBinaryData::ToolbarIcons::viewPanel, sizeof(BackendBinaryData::ToolbarIcons::viewPanel));

	visibleButton->setShape(visiblePath, false, true, true);

	visibleButton->addListener(this);

	visibleButton->setEnabled(!isSolo);


	addAndMakeVisible(setAsRootButton = new ShapeButton("", Colours::white.withAlpha(0.5f),
		Colours::white.withAlpha(0.8f),
		Colours::white));

	Path path;
	path.loadPathFromData(BackendBinaryData::PopupSymbols::rootShape, sizeof(BackendBinaryData::PopupSymbols::rootShape));

	setAsRootButton->setShape(path, false, true, true);

	setAsRootButton->addListener(this);

	setAsRootButton->setEnabled(!isSolo);

	addAndMakeVisible(soloButton = new ShapeButton("", Colours::white.withAlpha(0.5f),
		Colours::white.withAlpha(0.8f),
		Colours::white));

	Path soloPath;
	soloPath.loadPathFromData(BackendBinaryData::PopupSymbols::soloShape, sizeof(BackendBinaryData::PopupSymbols::soloShape));

	soloButton->setShape(soloPath, false, true, true);

	soloButton->addListener(this);

	addAndMakeVisible(bypassButton = new ShapeButton("", Colours::white.withAlpha(0.5f),
		Colours::white.withAlpha(0.8f),
		Colours::white));

	Path bypassPath;
	bypassPath.loadPathFromData(HiBinaryData::ProcessorEditorHeaderIcons::bypassShape, sizeof(HiBinaryData::ProcessorEditorHeaderIcons::bypassShape));

	bypassButton->setShape(bypassPath, false, true, true);

	bypassButton->addListener(this);

	addAndMakeVisible(popupButton = new ShapeButton("", Colours::white.withAlpha(0.5f),
		Colours::white.withAlpha(0.8f),
		Colours::white));

	Path popupPath;
	popupPath.loadPathFromData(HiBinaryData::ProcessorEditorHeaderIcons::popupShape, sizeof(HiBinaryData::ProcessorEditorHeaderIcons::popupShape));

	popupButton->setShape(popupPath, false, true, true);

	popupButton->addListener(this);


}

void ProcessorPopupItem::buttonClicked(Button *b)
{
	if (b == setAsRootButton)
	{
		editor->setRootProcessorWithUndo(p);

		if (!usedInList)
		{
			triggerMenuItem();
		}

		
	}
	else if (b == popupButton)
	{
		editor->showProcessorPopup(p, parent);
		
		if (!usedInList)
		{
			triggerMenuItem();
		}
	}
	else if (b == bypassButton)
	{

		p->setBypassed(!p->isBypassed());
		p->sendChangeMessage();
		repaint();


		for (int i = 0; i < getParentComponent()->getNumChildComponents(); i++)
		{
			getParentComponent()->getChildComponent(i)->repaint();
		}
	}
	else if (b == soloButton)
	{
		isSolo = !isSolo;

		if (isSolo)
		{
			editor->addProcessorToPanel(p);
		}
		else
		{
			editor->removeProcessorFromPanel(p);
		}

		setAsRootButton->setEnabled(!isSolo);

		repaint();

	}
	else if (b == visibleButton)
	{
		p->setEditorState(Processor::Visible, !p->getEditorState(Processor::Visible), sendNotification);
		p->sendChangeMessage();
		editor->getRootContainer()->refreshSize(false);
		repaint();
	}
}

void ProcessorPopupItem::resized()
{
	int w = isHeadline ? getWidth() : getWidth() - 6;
	int h = getHeight() - 12;

	if (isHeadline)
	{
		
		setAsRootButton->setBounds(w - h - 3, 6, h, h);
		bypassButton->setBounds(w - h - 3 - h - 3, 6, h, h);
		visibleButton->setBounds(w - h - 3 - h - 3 - h - 3, 6, h, h);
	}
	else
	{
		setAsRootButton->setBounds(w - h - 3, 6, h, h);
		popupButton->setBounds(w - h - 3 - h - 3, 6, h, h);
		bypassButton->setBounds(w - h - 3 - h - 3 - h - 3, 6, h, h);
		soloButton->setBounds(w - h - 3 - h - 3 - h - 3 - h - 3, 6, h, h);
	 visibleButton->setBounds(w - h - 3 - h - 3 - h - 3 - h - 3 - h - 3, 6, h, h);
	}
}

void ProcessorPopupItem::paint(Graphics &g)
{
	float w = (float)getWidth() - 4;
	float h = (float)getHeight();

	if (isHeadline)
	{
		Colour c = dynamic_cast<ModulatorSynth*>(p)->getIconColour();

		if (p->isBypassed()) c = c.withMultipliedAlpha(0.5f);
		
		g.setGradientFill(ColourGradient(c, 0, 0, c.withMultipliedAlpha(0.8f), w, 0, false));

		g.fillRoundedRectangle(0, 4, w + 4, h - 8, 4.0f);


		Colour textColour = p->isBypassed() ? Colours::white.withAlpha(0.3f) : Colours::white;
		g.setColour(textColour);

		g.setFont(GLOBAL_FONT().withHeight(15.0f).boldened());
		g.drawText(p->getId(), 35, 3, (int)w - 12, (int)h - 6, Justification::centredLeft, true);

		if (!p->isBypassed())
		{
			g.setColour(c);
			g.fillRect(0, 8, 4, getHeight() - 8);
		}

		

	}
	else
	{
		Colour c = p->getColour();

		if (p->isBypassed()) c = c.withMultipliedAlpha(0.5f);

		

		else if (parent->isBypassed()) c = c.withMultipliedAlpha(0.5f);

		Path path = p->getSymbol();




		ColourGradient grad(c.withMultipliedAlpha(0.7f).withMultipliedBrightness(isItemHighlighted() ? 1.5f : 1.0f), 0.0f, 0.0f, c.withMultipliedBrightness(isItemHighlighted() ? 1.5f : 1.0f), 0.0f, h, false);

		g.setGradientFill(grad);
		//g.setColour(c.withAlpha(0.7f).withMultipliedBrightness(isItemHighlighted() ? 1.4f : 1.0f));
		g.fillRoundedRectangle(11.0f, 2.0f, w - 14.0f, h - 4, 5.0f);

		g.setColour(isItemHighlighted() ? Colours::white : c);

		if (isSolo) g.setColour(Colours::red);

		g.drawRoundedRectangle(11.0f, 2.0f, w - 14.0f, h - 4.0f, 5.0f, 2.0f);

		Colour textColour = p->isBypassed() || parent->isBypassed() ? Colours::white.withAlpha(0.3f) : Colours::white;
		g.setColour(textColour);


		

		g.setFont(GLOBAL_BOLD_FONT());
		g.drawText(p->getId(), 35, 4, (int)w, (int)h - 8, Justification::topLeft, true);
       		g.setColour(textColour.withMultipliedAlpha(0.8f));
		g.setFont(GLOBAL_FONT());
		g.drawText(p->getName(), 35, 4, (int)w, (int)h - 8, Justification::bottomLeft, true);

		path.scaleToFit(13.0f, 6.0f, h - 12, h - 12, true);


		g.setGradientFill(ColourGradient(Colour(0xaaffffff),
			0.0f, 0.0f,
			Colour(0x55ffffff),
			0.0f, (float)getHeight(),
			false));

		g.fillPath(path);

		if (!parent->isBypassed())
		{
			Colour pc = dynamic_cast<ModulatorSynth*>(parent)->getIconColour();

			g.setColour(pc);
			g.fillRect(0, 0, 4, getHeight());
		}
	}

	Image hiddenArea(Image::ARGB, 30, 30, true);

	if (!p->getEditorState(Processor::Visible) || (parent != nullptr && !parent->getEditorState(Processor::Visible)))
	{


		Graphics imgG(hiddenArea);

		Path hiddenPath;

		hiddenPath.loadPathFromData(BackendBinaryData::PopupSymbols::hiddenPattern, sizeof(BackendBinaryData::PopupSymbols::hiddenPattern));

		hiddenPath.scaleToFit(0.0f, 0.0f, 30.0f, 30.0f, true);

		imgG.setColour(Colours::white.withAlpha(0.3f));

		imgG.fillPath(hiddenPath);

		g.setColour(Colours::black);

		g.setTiledImageFill(hiddenArea, 0, 0, 1.0f);

		g.fillAll();
	}

	
}

StupidRectangle::StupidRectangle()
{


	addAndMakeVisible(closeButton = new ShapeButton("Close Processor", Colours::white.withAlpha(0.7f), Colours::white, Colours::white));
	Path deletePath;
	deletePath.loadPathFromData(HiBinaryData::ProcessorEditorHeaderIcons::closeIcon, sizeof(HiBinaryData::ProcessorEditorHeaderIcons::closeIcon));
	closeButton->setShape(deletePath, true, true, true);
	closeButton->setToggleState(true, dontSendNotification);
	
}

void StupidRectangle::paint(Graphics &g)
{
	
	g.setColour(Colour(BACKEND_BG_COLOUR_BRIGHT));
	
	g.fillAll();

	g.setColour(Colours::black.withAlpha(0.4f));

	g.fillRect(0, 0, getWidth(), 40);

	g.setColour(Colours::black.withAlpha(0.5f));
	g.drawHorizontalLine(40, 0.0f, (float)getWidth());

	g.setColour(Colours::white.withAlpha(0.2f));
	g.drawHorizontalLine(41, 0.0f, (float)getWidth());


	g.setGradientFill(ColourGradient(Colours::black.withAlpha(0.2f), 0.0f, 0.0f,
		Colours::transparentBlack, 6.0f, 0.0, false));

	g.fillRect(0, 0, 6, getHeight());

	g.setGradientFill(ColourGradient(Colours::black.withAlpha(0.2f), (float)getWidth(), 0.0f,
		Colours::transparentBlack, (float)getWidth() - 6.0f, 0.0f, false));

	g.fillRect(getWidth()-6, 0, 6, getHeight());

	g.setGradientFill(ColourGradient(Colours::black.withAlpha(0.2f), 0.0f, 0.0f,
		Colours::transparentBlack, 0.0f, 6.0f, false));


	g.fillRect(0, 0, getWidth(), 6);


    g.setColour(Colours::white);
    
    //g.drawRect(getLocalBounds(), 1);

	g.setFont(GLOBAL_BOLD_FONT().withHeight(15.0f));

	g.setColour(Colours::white);

	g.drawText(path, 8, 8, getWidth() - 20, 24, Justification::centred, true);
}

void StupidRectangle::resized()
{
	closeButton->setBounds(getWidth() - 24, 12, 16, 16);
}

ProcessorList::ProcessorList(BackendProcessorEditor *editor_) :
editor(editor_),
mainSynthChain(editor_->getMainSynthChain())
{
	refreshList();
}

void ProcessorList::refreshList()
{
	items.clear();

	Processor::Iterator<Processor> iter(mainSynthChain, false);

	Processor *ip;
	ModulatorSynth *lastSynth = nullptr;

	while ((ip = iter.getNextProcessor()) != nullptr)
	{
		//if(ip->isBypassed()) continue;

		const bool isSynth = dynamic_cast<ModulatorSynth*>(ip) != nullptr;

		if (isSynth)
		{
			lastSynth = dynamic_cast<ModulatorSynth*>(ip);

			items.add(new ProcessorPopupItem(lastSynth, editor));
		}
		else
		{
			const bool allowClick = dynamic_cast<Chain*>(ip) == nullptr;

			if (allowClick)
			{
				items.add(new ProcessorPopupItem(lastSynth, ip, editor));
			}
		}
	}

	for (int i = 0; i < items.size(); i++)
	{
		addAndMakeVisible(items[i]);
		dynamic_cast<ProcessorPopupItem*>(items[i])->setUsedInList();
	}

	setSize(260, getHeightOfAllItems());
	resized();
}