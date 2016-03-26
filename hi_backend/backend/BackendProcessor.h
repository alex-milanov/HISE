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

#ifndef BACKEND_PROCESSOR_H_INCLUDED
#define BACKEND_PROCESSOR_H_INCLUDED


class BackendProcessor;

class AudioDeviceDialog: public Component,
						 public ButtonListener
{
public:

	AudioDeviceDialog(BackendProcessor *ownerProcessor_);

	void resized() override
	{
		selector->setBounds(0, 0, getWidth(), getHeight() - 36);
		cancelButton->setBounds(getWidth() - 100, getHeight() - 36, 80, 32);
		applyAndCloseButton->setBounds(20, getHeight() - 36, 200, 32);
	}

	void buttonClicked(Button *b);

	void paint(Graphics &g) override
	{
		g.fillAll(Colour(0xFF444444));
	}

	~AudioDeviceDialog();


private:

	ScopedPointer<AudioDeviceSelectorComponent> selector;

	ScopedPointer<TextButton> applyAndCloseButton;
	ScopedPointer<TextButton> cancelButton;

	BackendProcessor *ownerProcessor;

	HiPropertyPanelLookAndFeel pplaf;
	AlertWindowLookAndFeel alaf;

};



/** This is the main audio processor for the backend application. 
*
*	It connects to a BackendProcessorEditor and has extensive development features.
*
*	It is a wrapper for all plugin types and provides 8 parameters for the macro controls.
*	It also acts as global MainController to allow every child object to get / set certain global information
*/
class BackendProcessor: public PluginParameterAudioProcessor,
						public MainController
{
public:
	BackendProcessor(AudioDeviceManager *deviceManager_=nullptr, AudioProcessorPlayer *callback_=nullptr);

	~BackendProcessor();

	void handleEditorData(bool save)
	{
#if IS_STANDALONE_APP
		File xmlFile(PresetHandler::getDataFolder() + "/editorData.xml");
#else
        File xmlFile(PresetHandler::getDataFolder() + "/editorDataPlugin.xml");
#endif

		if (save)
		{
			ScopedPointer<XmlElement> xml = editorInformation.createXml();
			xml->writeToFile(xmlFile, "");
		}
		else
		{
			ScopedPointer<XmlElement> xml = XmlDocument::parse(xmlFile);
			if (xml != nullptr)
			{
				editorInformation = ValueTree::fromXml(*xml);
			}
			else
			{
				editorInformation = ValueTree("invalid");
			}
			
		}
	}

	static XmlElement *getSettings()
	{
#if JUCE_WINDOWS

		String parentDirectory = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName() + "/Hart Instruments";

#else

		String parentDirectory = File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile).getParentDirectory().getFullPathName();

#endif

		File savedDeviceData = File(parentDirectory + "/DeviceSettings.xml");

		return XmlDocument::parse(savedDeviceData);
	}

	

	void prepareToPlay (double sampleRate, int samplesPerBlock);
	void releaseResources() 
	{
		writeToConsole("RELEASE_RESOURCES_CALLED", Console::Error);
	};

	void getStateInformation	(MemoryBlock &destData) override
	{
		MemoryOutputStream output(destData, false);

		ValueTree v = synthChain->exportAsValueTree();

		v.setProperty("ProjectRootFolder", GET_PROJECT_HANDLER(synthChain).getWorkDirectory().getFullPathName(), nullptr);

		v.writeToStream(output);
	};

	

	void setStateInformation(const void *data,int sizeInBytes) override
	{
		ValueTree v = ValueTree::readFromData(data, sizeInBytes);

		String fileName = v.getProperty("ProjectRootFolder", String::empty);

		if (fileName.isNotEmpty())
		{
			File root(fileName);
			if (root.exists() && root.isDirectory())
			{
				GET_PROJECT_HANDLER(synthChain).setWorkingProject(root);
			}
		}

        loadPreset(v);
        
		//synthChain->restoreFromValueTree(v);
		//synthChain->compileAllScripts();
	}

	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

	virtual void processBlockBypassed (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
	{
		buffer.clear();
		midiMessages.clear();
		allNotesOff();
	};

	void handleControllersForMacroKnobs(const MidiBuffer &midiMessages);

	void initialiseAudioDriver(XmlElement *deviceData)
	{
		if (deviceData != nullptr && deviceManager->initialise(0, 2, deviceData, false) == String::empty)
		{
			callback->setProcessor(this);

			deviceManager->addAudioCallback(callback);
			deviceManager->addMidiInputCallback(String::empty, callback);
		}
	}
 
	AudioProcessorEditor* createEditor();
	bool hasEditor() const {return true;};

	bool acceptsMidi() const {return true;};
	bool producesMidi() const {return false;};
	bool silenceInProducesSilenceOut() const {return false;};
	double getTailLengthSeconds() const {return 0.0;};

	ModulatorSynthChain *getMainSynthChain() override {return synthChain; };

	const ModulatorSynthChain *getMainSynthChain() const override { return synthChain; }

	/// @brief returns the number of PluginParameter objects, that are added in the constructor
    int getNumParameters() override
	{
		return 8;
	}

	/// @brief returns the PluginParameter value of the indexed PluginParameter.
    float getParameter (int index) override
	{
		

		return synthChain->getMacroControlData(index)->getCurrentValue() / 127.0f;
	}

	/** @brief sets the PluginParameter value.
	
		This method uses the 0.0-1.0 range to ensure compatibility with hosts. 
		A more user friendly but slower function for GUI handling etc. is setParameterConverted()
	*/
    void setParameter (int index, float newValue) override
	{
		synthChain->setMacroControl(index, newValue * 127.0f, sendNotificationAsync);
	}


	/// @brief returns the name of the PluginParameter
    const String getParameterName (int index) override
	{
		return "Macro " + String(index + 1);
	}

	/// @brief returns a converted and labeled string that represents the current value
    const String getParameterText (int index) override
	{
		
		return String(synthChain->getMacroControlData(index)->getDisplayValue(), 1);
	}
	void setEditorState(ValueTree &editorState);
private:

	AudioPlayHead::CurrentPositionInfo lastPosInfo;

	friend class AudioDeviceDialog;
	friend class BackendProcessorEditor;
	friend class BackendCommandTarget;
	friend class CombinedDebugArea;

	ScopedPointer<ModulatorSynthChain> synthChain;

	AudioDeviceManager *deviceManager;
	AudioProcessorPlayer *callback;
	
	ScopedPointer<UndoManager> viewUndoManager;

	AudioSampleBuffer multiChannelBuffer;

	ValueTree editorInformation;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackendProcessor)
};

#endif