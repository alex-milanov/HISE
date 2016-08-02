/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 4.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_A3E3B1B2DF55A952__
#define __JUCE_HEADER_A3E3B1B2DF55A952__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"

class SamplerBody;

//[/Headers]

#include "ValueSettingComponent.h"


//==============================================================================
/**
                                                                    //[Comments]
    \cond HIDDEN_SYMBOLS
	An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SampleEditor  : public Component,
                      public SamplerSubEditor,
                      public ApplicationCommandTarget,
                      public AudioDisplayComponent::Listener,
                      public SafeChangeListener
{
public:
    //==============================================================================
    SampleEditor (ModulatorSampler *s, SamplerBody *b);
    ~SampleEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void changeListenerCallback(SafeChangeBroadcaster *)
	{
		updateWaveform();
	}

	void rangeChanged(AudioDisplayComponent *c, int areaThatWasChanged) override
	{
		SamplerSoundWaveform *waveForm = dynamic_cast<SamplerSoundWaveform*>(c);

		if(waveForm == nullptr) return;

		const ModulatorSamplerSound *currentSound = waveForm->getCurrentSound();

		if(currentSound != nullptr && selection.getLast() == currentSound)
		{
			ModulatorSamplerSound *soundToChange = selection.getLast();

			AudioDisplayComponent::SampleArea *area = c->getSampleArea(areaThatWasChanged);

			const int64 startSample = jmax<int64>(0, area->getSampleRange().getStart());
			const int64 endSample = (int64)(area->getSampleRange().getEnd());

			switch (areaThatWasChanged)
			{
			case SamplerSoundWaveform::SampleStartArea:
			{
				soundToChange->setProperty(ModulatorSamplerSound::SampleStartMod, (int)(endSample - startSample));
				soundToChange->closeFileHandle();
				break;
			}
			case SamplerSoundWaveform::LoopArea:
			{
				if (!area->leftEdgeClicked)
				{
					soundToChange->setProperty(ModulatorSamplerSound::LoopEnd, (int)endSample);
				}
				else
				{
					soundToChange->setProperty(ModulatorSamplerSound::LoopStart, (int)startSample);
				}
				break;
			}
			case SamplerSoundWaveform::PlayArea:
			{
				if (!area->leftEdgeClicked)
				{
					soundToChange->setProperty(ModulatorSamplerSound::SampleEnd, (int)endSample);
				}
				else
				{
					soundToChange->setProperty(ModulatorSamplerSound::SampleStart, (int)startSample);
					soundToChange->closeFileHandle();
				}
				break;
			}
			case SamplerSoundWaveform::LoopCrossfadeArea:
			{
				jassert(area->leftEdgeClicked);
				soundToChange->setProperty(ModulatorSamplerSound::LoopXFade, (int)(endSample - startSample));
				break;
			}
			}

			currentWaveForm->updateRanges();
		}
	};

	/** All application commands are collected here. */
	enum SampleMapCommands
	{
		// Undo / Redo
		ZoomIn = 0x3000,
		ZoomOut,

		ToggleLoopView,

		EnableSampleStartArea,
		EnableLoopArea,
		EnablePlayArea,

		SelectWithMidi,
		NormalizeVolume,
		LoopEnabled,

		numCommands
	};


	ApplicationCommandTarget* getNextCommandTarget() override
	{
		return findFirstTargetParentComponent();
	};

	void getAllCommands (Array<CommandID>& commands) override
	{
		const CommandID id[] = {ZoomIn,
								ZoomOut,
								ToggleLoopView,
								EnableSampleStartArea,
								EnableLoopArea,
								EnablePlayArea,
								SelectWithMidi,
								NormalizeVolume,
								LoopEnabled,
								};

		commands.addArray(id, numElementsInArray(id));
	};

	void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result) override
	{
		const bool isSelected = selection.size() > 0 && selection.getLast() != nullptr;

		switch (commandID)
		{
		case ZoomIn:			result.setInfo("Zoom In", "Zoom in the sample map", "Zooming", 0);
								result.addDefaultKeypress('+', ModifierKeys::commandModifier);
								result.setActive(isSelected && zoomFactor != 16.0f);
								break;
		case ZoomOut:			result.setInfo("Zoom Out", "Zoom out the sample map", "Zooming", 0);
								result.addDefaultKeypress('-', ModifierKeys::commandModifier);
								result.setActive(isSelected && zoomFactor != 1.0f);
								break;
		case ToggleLoopView:   result.setInfo("Toggle Loop View", "Toggle between linear and loop start / end view", "Zooming", 0);
								result.addDefaultKeypress('-', ModifierKeys::commandModifier);
								result.setActive(isSelected && selection.getLast()->getProperty(ModulatorSamplerSound::LoopEnabled));
								break;
		case EnableSampleStartArea:	result.setInfo("Enable SampleStart Dragging", "Enable Sample Start Modulation Area Dragging", "Areas", 0);
									result.setActive(isSelected && (int)selection.getLast()->getProperty(ModulatorSamplerSound::SampleStartMod) != 0);
									break;
		case EnableLoopArea:	result.setInfo("Enable SampleStart Dragging", "Enable Loop Area Dragging", "Areas", 0);
								result.setActive(isSelected && selection.getLast()->getProperty(ModulatorSamplerSound::LoopEnabled));
								break;
		case EnablePlayArea:	result.setInfo("Enable Play Area Dragging", "Enable Playback Area Dragging", "Areas", 0);
								result.setActive(isSelected);
								break;
		case SelectWithMidi:	result.setInfo("Midi Select", "Autoselect the most recently triggered sound", "Tools", 0);
								result.setActive(true);
								result.setTicked(sampler->getEditorState(ModulatorSampler::MidiSelectActive));
								break;
		case NormalizeVolume:	result.setInfo("Normalize Volume", "Normalize the sample volume to 0dB", "Properties", 0);
								result.setActive(isSelected);
								result.setTicked(isSelected && (int)selection.getLast()->getProperty(ModulatorSamplerSound::Normalized));
								break;
		case LoopEnabled:		result.setInfo("Loop Enabled", "Enable Loop Playback", "Properties", 0);
								result.setActive(isSelected);
								result.setTicked(isSelected && (int)selection.getLast()->getProperty(ModulatorSamplerSound::LoopEnabled));
								break;
		}
	};

	bool perform (const InvocationInfo &info) override;



	void soundsSelected(const Array<ModulatorSamplerSound*>  &selectedSoundList) override
	{


		for (int i = 0; i < selectedSoundList.size(); i++)
			selection.add(selectedSoundList[i]);

		panSetter->setCurrentSelection(selectedSoundList);
		volumeSetter->setCurrentSelection(selectedSoundList);
		pitchSetter->setCurrentSelection(selectedSoundList);
		sampleStartSetter->setCurrentSelection(selectedSoundList);
		sampleEndSetter->setCurrentSelection(selectedSoundList);
		startModulationSetter->setCurrentSelection(selectedSoundList);
		loopStartSetter->setCurrentSelection(selectedSoundList);
		loopEndSetter->setCurrentSelection(selectedSoundList);
		loopCrossfadeSetter->setCurrentSelection(selectedSoundList);

		samplerEditorCommandManager->commandStatusChanged();

		if(selection.size() != 0 && selection.getLast() != 0) currentWaveForm->setSoundToDisplay(selection.getLast());
		else currentWaveForm->setSoundToDisplay(nullptr);
	}

	void paintOverChildren(Graphics &g)
	{
		if(selection.size() != 0)
		{
			g.setColour(Colours::black.withAlpha(0.4f));

			const bool useGain = selection.getLast()->getNormalizedPeak() != 1.0f;

			const String fileName = selection.getLast()->getPropertyAsString(ModulatorSamplerSound::FileName);

			const String autogain = useGain ? ("Autogain: " + String(Decibels::gainToDecibels(selection.getLast()->getNormalizedPeak()), 1) + " dB") : String::empty;

			int width = jmax<int>(GLOBAL_BOLD_FONT().getStringWidth(autogain), GLOBAL_BOLD_FONT().getStringWidth(fileName)) + 8;

			Rectangle<int> area(viewport->getRight() - width-1, viewport->getY()+1, width, useGain ? 32 : 16);

			g.fillRect(area);


			g.setColour(Colours::white.withAlpha(0.7f));
			g.setFont(GLOBAL_BOLD_FONT());

			
			

			g.drawText(fileName, area, Justification::topRight, false);

			if(useGain)
			{
				g.drawText(autogain, area, Justification::bottomRight, false);
			}





		}





	}

	void updateWaveform()
	{
		samplerEditorCommandManager->commandStatusChanged();

		currentWaveForm->updateRanges();
	}

	void zoom(bool zoomOut)
	{
		if(!zoomOut)
		{
			zoomFactor = jmin(16.0f, zoomFactor * 1.5f); resized();
		}
		else
		{
			zoomFactor = jmax(1.0f, zoomFactor / 1.5f); resized();
		}

		resized();
	}

	void mouseWheelMove	(	const MouseEvent & 	event, const MouseWheelDetails & 	wheel )	override
	{
		if(event.mods.isCtrlDown())
		{
			zoom(wheel.deltaY < 0);
		}
		else
		{
			getParentComponent()->mouseWheelMove(event, wheel);
		}

	};

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	friend class SampleEditorToolbarFactory;

	float zoomFactor;

	ModulatorSampler *sampler;
	SamplerBody	*body;

	ScopedPointer<SamplerSoundWaveform> currentWaveForm;

	ReferenceCountedArray<ModulatorSamplerSound> selection;

	ScopedPointer<SampleEditorToolbarFactory> toolbarFactory;

	ScopedPointer<ApplicationCommandManager> samplerEditorCommandManager;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Viewport> viewport;
    ScopedPointer<ValueSettingComponent> volumeSetter;
    ScopedPointer<ValueSettingComponent> pitchSetter;
    ScopedPointer<ValueSettingComponent> sampleStartSetter;
    ScopedPointer<ValueSettingComponent> sampleEndSetter;
    ScopedPointer<ValueSettingComponent> loopStartSetter;
    ScopedPointer<ValueSettingComponent> loopEndSetter;
    ScopedPointer<ValueSettingComponent> loopCrossfadeSetter;
    ScopedPointer<ValueSettingComponent> startModulationSetter;
    ScopedPointer<Toolbar> toolbar;
    ScopedPointer<ValueSettingComponent> panSetter;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleEditor)
};

//[EndFile] You can add extra defines here...
/** \endcond */
//[/EndFile]

#endif   // __JUCE_HEADER_A3E3B1B2DF55A952__
