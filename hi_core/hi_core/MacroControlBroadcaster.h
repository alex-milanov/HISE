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

#ifndef MACROCONTROLBROADCASTER_H_INCLUDED
#define MACROCONTROLBROADCASTER_H_INCLUDED

/** A class that handles macro controlled parameters of its children.
*	@ingroup macroControl
*
*	This is supposed to be a base class of ModulatorSynthChain to encapsulate all macro control handling,
*	so you should not use this class directly.
*/
class MacroControlBroadcaster
{
public:

	/** Creates a new MacroControlBroadcaster with eight Macro slots. */
	MacroControlBroadcaster(ModulatorSynthChain *chain);

	/** A simple POD object to store information about a macro controlled parameter. 
	*	@ingroup macroControl
	*
	*/
	struct MacroControlledParameterData
	{
	public:

		/** Creates a new Parameter data object. */
		MacroControlledParameterData(Processor *p, int  parameter_, const String &parameterName_, NormalisableRange<double> range_, bool readOnly=true);

		/** Restores a Parameter object from an exported XML document. 
		*
		*	You have to supply the ModulatorSynthChain to find the Processor with the exported ID.
		*/
		MacroControlledParameterData(ModulatorSynthChain *chain, XmlElement &xml);

		/** Allows comparison. This only compares the Processor and the parameter (not the range). */
		bool operator== (const MacroControlledParameterData& other) const;

		void setAttribute(double normalizedInputValue);

		/** Inverts the range of the parameter. */
		void setInverted(bool shouldBeInverted) { inverted = shouldBeInverted; };

		/** Sets the parameter to be read only. 
		*
		*	By default it is activated. if not, it can change the whole macro control. 
		*/
		void setReadOnly(bool shouldBeReadOnly) { readOnly = shouldBeReadOnly; };

		/** Checks if the parameter data is read only. */
		bool isReadOnly() const { return readOnly;};

		/** Returns true if the parameter range is inverted. */
		bool isInverted() const {return inverted; };

		/** Returns the min and max values for the parameter range. This is determined by the Controls that are
		*	connected to the parameter. */
		NormalisableRange<double> getTotalRange() const { return range; };

		/** Returns the actual limit of the range.
		*
		*	@param getHighLimit if true, it returns the upper limit.
		*/
		double getParameterRangeLimit(bool getHighLimit) const;

		NormalisableRange<double> getParameterRange() const {return parameterRange;};

		/** Returns the value of the parameter.
		*
		*	@param normalizedSliderInput the input from 0.0 to 1.0
		*	@returns the scaled and inverted (if enabled) value
		*/
		float getNormalizedValue(double normalizedSliderInput);

		/** set the range start that is used by the macro control. */
		void setRangeStart(double min) {parameterRange.start = min; };

		/** set the range end that is used by the macro control. */
		void setRangeEnd(double max) {	parameterRange.end = max; };

		/** returns the processor that the parameter is connected to. This may be nullptr, if the Processor was deleted. */
		Processor *getProcessor() {return controlledProcessor.get(); };

		/** returns the processor (read only) that the parameter is connected to. This may be nullptr, if the Processor was deleted. */
		const Processor *getProcessor() const {return controlledProcessor.get(); };

		/** Returns the parameter index that the parameter is controlling. This is a Processor::SpecialParameter enum value in most cases. */
		int getParameter() const {	return parameter; };

		void setParameterIndex(int newParameter)
		{
			parameter = newParameter;
		}

		/** Returns the parameter name. This is the name of the interface control (Processor parameter have no name per se). */
		String getParameterName() const { return parameterName; };

		
		/** Exports all data as xml element which can be added as ValueTreeProperty. */
		XmlElement *exportAsXml();

	private:

		// The ID of the Processor that is controlled
		const String id;

		// The controlled parameter
		int parameter;

		const String parameterName;

		WeakReference<Processor> controlledProcessor;

		// The maximal range
		//const Range<double> range;

		// The actual range
		//Range<double> parameterRange;

		NormalisableRange<double> range;

		NormalisableRange<double> parameterRange;

		bool inverted;


		bool readOnly;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroControlledParameterData)
	};

	

	/** A MacroControlData object stores information about all parameters that are mapped to one macro control. 
	*	@ingroup macroControl
	*
	*/
	struct MacroControlData
	{
		/** Creates an empty data object. */
		MacroControlData(int index):
			macroName("Macro " + String(index + 1))
		{};

		virtual ~MacroControlData()
		{
			controlledParameters.clear();
		};

		/** Creates a new data object from an XmlElement. 
		*
		*	The chain is used to find the child processor with the given id.
		*/
		MacroControlData(ModulatorSynthChain *chain, XmlElement *xml);

		/** Returns the last value. */
		float getCurrentValue() const;

		/** returns the display value (which takes the first mapped parameter and applies the value to its range). */
		float getDisplayValue() const;

		/** sets the value of the macro controller. 
		*
		*	@param newValue the value from 0 to 127 (the knob range).
		*
		*	This iterates all parameters that this controller is connected to and sets the attribute.
		*	It also saves the parameter so that other objects have access to the current value (useful if using scripting)
		*/
		void setValue(float newValue);

		/** Checks if the processor of the parameter still exists. */
		bool isDanglingProcessor(int parameterIndex);

		/** Removes all parameters with deleted processors. */
		void clearDanglingProcessors();

		/** Removes all parameters that control a certain processor. */
		void removeAllParametersWithProcessor(Processor *p);

		/** Exports the data as XML. */
		XmlElement *exportAsXml();

		/** checks if the parameter exists. */
		bool hasParameter(Processor *p, int parameterIndex);

		/** adds the parameter to the parameter list and renames the macro if it is the only parameter. */
		void addParameter(Processor *p, int parameterId, const String &parameterName, NormalisableRange<double> range, bool readOnly=true);

		/** Removes the parameter. */
		void removeParameter(int parameterIndex);
		
		/** Removes the parameter with the name. */
		void removeParameter(const String &parameterName, const Processor *processor=nullptr);

		/** returns the parameter at the supplied index. */
		MacroControlledParameterData *getParameter(int parameterIndex)
		{
			return controlledParameters[parameterIndex];
		}

		const MacroControlledParameterData *getParameter(int parameterIndex) const
		{
			return controlledParameters[parameterIndex];
		}

		

		/** Searches the parameters for a match with the processor and index. */
		MacroControlledParameterData *getParameterWithProcessorAndIndex(Processor *p, int parameterIndex)
		{
			for(int i = 0; i < controlledParameters.size(); i++)
			{
				if(controlledParameters[i]->getProcessor() == p && controlledParameters[i]->getParameter() == parameterIndex)
				{
					return controlledParameters[i];
				}
			}

			return nullptr;
		}

		/** Searches the parameters for a match with the processor and parameter name. */
		MacroControlledParameterData *getParameterWithProcessorAndName(Processor *p, const String &parameterName)
		{
			for(int i = 0; i < controlledParameters.size(); i++)
			{
				if(controlledParameters[i]->getProcessor() == p && controlledParameters[i]->getParameterName() == parameterName)
				{
					return controlledParameters[i];
				}
			}

			return nullptr;
		}

		/** Returns the macro name. */
		String getMacroName() const {return macroName; };

		/** Sets the macro name that is displayed beyond the knob. */
		void setMacroName(const String &name)
		{
			macroName = name;
		}

		/** Returns the number of mapped parameters. */
		int getNumParameters() const { return controlledParameters.size(); };

		/** sets the MidiController number that controls this macro when loaded as main chain. */
		void setMidiController(int newControllerNumber)
		{ 
			midiController = newControllerNumber;	
		};

		int getMidiController() const noexcept { return midiController; };

	private:

		String macroName;

		float currentValue;

		int midiController;

		OwnedArray<MacroControlledParameterData> controlledParameters;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroControlData)
	};

	/** Small helper function that iterates all child processors and returns the matching Processor with the given ID. */
	static Processor *findProcessor(Processor *p, const String &idToSearch);

	/** sets the macro control to the supplied value and sends a notification message if desired. */
	void setMacroControl(int macroIndex, float newValue, NotificationType notifyEditor=dontSendNotification);

	/** searches all macroControls and returns the index of the control if the supplied parameter is mapped or -1 if it is not mapped. */
	int getMacroControlIndexForProcessorParameter(const Processor *p, int parameter) const
	{
		for(int i = 0; i < macroControls.size(); i++)
		{
			for(int j = 0; j < macroControls[i]->getNumParameters(); j++)
			{
				if(macroControls[i]->getParameter(j)->getProcessor() == p &&
				   macroControls[i]->getParameter(j)->getParameter() == parameter)
				{
					return i;
				}
			}
		}

		return -1;
	}

	/** Adds a parameter.
	*
	*	@param macroControllerIndex the index from 1 to 8 where the parameter should be added.
	*	@param processorId the unique id of the processor. If the ID is not unique, then the first processor is used, which can have undesired effects,
	*			           so make sure, you rename the processor before mapping a controller!
	*	@param parameterId the parameter id (use the SpecialParameters enum from the Processor)
	*	@param parameterName the name of the control. This is supplied by the name of the widget component, but it can be useful for eg. scripted controls.
	*	@param range the total range of the parameter.
	*	@param readOnly if the knob can be changed when assigned. 
	*/
	void addControlledParameter(int macroControllerIndex, 
								const String &processorId, 
								int parameterId, 
								const String &parameterName,
								NormalisableRange<double> range,
								bool readOnly=true);

	/** Returns the MacroControlData object at the supplied index. */
	MacroControlData *getMacroControlData(int index) { return macroControls[index];	}

	void saveMacrosToValueTree(ValueTree &v) const
	{
		ScopedPointer<XmlElement> macroControlData = new XmlElement("macro_controls");

		for(int i = 0; i < macroControls.size(); i++)
		{
			macroControlData->addChildElement(macroControls[i]->exportAsXml());
		}

#if USE_OLD_FILE_FORMAT

		v.setProperty("MacroControls", macroControlData->createDocument(""), nullptr);

#else
		ValueTree macros = ValueTree::fromXml(*macroControlData);
		
		v.addChild(macros, -1, nullptr);

#endif
	}

	void loadMacrosFromValueTree(const ValueTree &v);
	
	/** Removes all parameters and resets the name. */
	void clearData(int macroIndex);
    
    void clearAllMacroControls()
    {
        const int numMacros = macroControls.size();
        
        
        for(int i = 0; i < numMacros; i++)
        {
            clearData(i);
        }
    }

	/** Checks if the macro control has any parameters. */
	bool hasActiveParameters(int macroIndex)
	{
		return macroControls[macroIndex]->getNumParameters() != 0;
	}

	/** this replaces the macro control data object at the specified index with the new one. 
	*
	*	@param index the index of the macro control which will be replaced
	*	@param newData a pointer to the macro control data that should replace the old data
	*	@param parentChain a pointer to the ModulatorSynthChain where the newData resides. This is used to get the correct Processor for the given ID.
	*
	*/
	void replaceMacroControlData(int index, MacroControlData *newData, ModulatorSynthChain *parentChain);

private:

	OwnedArray<MacroControlData> macroControls;
	
	ModulatorSynthChain *thisAsSynth;

};



#endif  // MACROCONTROLBROADCASTER_H_INCLUDED