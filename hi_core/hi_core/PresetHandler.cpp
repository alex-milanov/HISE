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

void CopyPasteTarget::grabCopyAndPasteFocus()
{
#if STANDALONE_CONVOLUTION
    return;
#endif
    
#if USE_BACKEND
    Component *thisAsComponent = dynamic_cast<Component*>(this);
    
    if(thisAsComponent)
    {
        thisAsComponent->findParentComponentOfClass<BackendProcessorEditor>()->setCopyPasteTarget(this);

		isSelected = true;
		thisAsComponent->repaint();
    }
    else
    {
        // You can only use components as CopyAndPasteTargets!
        jassertfalse;
    }
#endif
}


void PresetHandler::saveProcessorAsPreset(Processor *p, const String &directoryPath/*=String::empty*/)
{
	const bool hasCustomName = p->getName() != p->getId();

	if(!hasCustomName) p->setId(getCustomName(p->getName()));

	File directory = directoryPath.isEmpty() ? getDirectory(p) : File(directoryPath);

	jassert(directory.isDirectory());

	String fileName = directory.getFullPathName() + "/" + p->getId() + ".hip";
	File outputFile(fileName);

	if(!outputFile.existsAsFile() || showYesNoWindow("Overwrite File " + fileName, "Do you want to overwrite the Preset?"))
	{
		debugToConsole(p, "Save " + p->getId() + " to " + directory.getFullPathName());

		ValueTree v = p->exportAsValueTree();

		outputFile.deleteFile();

		FileOutputStream fos(outputFile);

		v.writeToStream(fos);
	}
}

String PresetHandler::getProcessorNameFromClipboard(const FactoryType *t)
{
	if(SystemClipboard::getTextFromClipboard() == String::empty) return String::empty;

	String x = SystemClipboard::getTextFromClipboard();
	ScopedPointer<XmlElement> xml = XmlDocument::parse(x);

	if(xml == nullptr) return String::empty;
	
#if USE_OLD_FILE_FORMAT

	bool isProcessor = true;
	String type = xml->getTagName();

#else

	bool isProcessor = xml->getTagName() == "Processor";
	String type = xml->getStringAttribute("Type");

#endif

	String id = xml->getStringAttribute("ID");
	
	if(!isProcessor || type == String::empty || id == String::empty) return String::empty;

	if (t->allowType(type)) return id;
				else		return String::empty;
}

void PresetHandler::copyProcessorToClipboard(Processor *p)
{
	ScopedPointer<XmlElement> xml = p->exportAsValueTree().createXml();
	String x = xml->createDocument(String::empty);
	SystemClipboard::copyTextToClipboard(x);

	debugToConsole(p, p->getId() + " was copied to clipboard.");
}

String PresetHandler::getCustomName(const String &typeName)
{
	String message;
	message << "Enter the unique Name for the ";
	message << typeName;
	message << ". CamelCase is recommended.";

	AlertWindowLookAndFeel laf;

	ScopedPointer<AlertWindow> nameWindow = new AlertWindow("Enter name for " + typeName, message, AlertWindow::AlertIconType::QuestionIcon);

	nameWindow->setLookAndFeel(&laf);

	nameWindow->addTextEditor("Name", typeName );
	nameWindow->addButton("OK", 1, KeyPress(KeyPress::returnKey));
	nameWindow->addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	if(nameWindow->runModalLoop()) return nameWindow->getTextEditorContents("Name");
	else return String::empty;
};

bool PresetHandler::showYesNoWindow(const String &title, const String &message)
{
	AlertWindowLookAndFeel laf;

	ScopedPointer<AlertWindow> nameWindow = new AlertWindow(title, message, AlertWindow::AlertIconType::NoIcon);

	nameWindow->setLookAndFeel(&laf);

	
	nameWindow->addButton("OK", 1, KeyPress(KeyPress::returnKey));
	nameWindow->addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	return (nameWindow->runModalLoop() == 1);
};

void PresetHandler::showMessageWindow(const String &title, const String &message)
{
	AlertWindowLookAndFeel laf;

	ScopedPointer<AlertWindow> nameWindow = new AlertWindow(title, message, AlertWindow::AlertIconType::NoIcon);

	nameWindow->setLookAndFeel(&laf);

	
	nameWindow->addButton("OK", 1, KeyPress(KeyPress::returnKey));

	nameWindow->runModalLoop();

	return ;
};

struct CountedProcessorId
{
	CountedProcessorId(Processor *p) :
		processorName(p->getId())
	{
		processors.add(p);
	};

	const String processorName;
	Array<WeakReference<Processor>> processors;
};


void ProjectHandler::createNewProject(const File &workingDirectory)
{
	if (workingDirectory.exists() && workingDirectory.isDirectory())
	{
		if (workingDirectory.getNumberOfChildFiles(File::findFilesAndDirectories) != 0)
		{
			PresetHandler::showMessageWindow("Directory already exists", "The directory is not empty. Try another one...");
			return;
		}
	}

	for (int i = 0; i < (int)SubDirectories::numSubDirectories; i++)
	{
		File subDirectory = workingDirectory.getChildFile(getIdentifier((SubDirectories)i));

		subDirectory.createDirectory();
	}

	setWorkingProject(workingDirectory);
}

void ProjectHandler::setWorkingProject(const File &workingDirectory)
{
	if (workingDirectory == currentWorkDirectory) return;

	if (!isValidProjectFolder(workingDirectory))
	{
		jassertfalse;
		return;
	}

	currentWorkDirectory = workingDirectory;

	if (!workingDirectory.exists()) return;

	checkSettingsFile();

	checkSubDirectories();

	

	jassert(currentWorkDirectory.exists() && currentWorkDirectory.isDirectory());

	if (!recentWorkDirectories.contains(workingDirectory.getFullPathName()))
	{
		const int numTooMuch = recentWorkDirectories.size() - 12;

		if (numTooMuch > 0)
		{
			recentWorkDirectories.removeRange(0, numTooMuch);
		}

		recentWorkDirectories.addIfNotAlreadyThere(workingDirectory.getFullPathName());
	}
	else
	{
		const int index = recentWorkDirectories.indexOf(workingDirectory.getFullPathName());

		recentWorkDirectories.move(index, 0);
	}

	ScopedPointer<XmlElement> xml = new XmlElement("Projects");

	xml->setAttribute("current", currentWorkDirectory.getFullPathName());

	for (int i = 0; i < recentWorkDirectories.size(); i++)
	{
		XmlElement *child = new XmlElement("Recent");

		child->setAttribute("path", recentWorkDirectories[i]);

		xml->addChildElement(child);
	}

	File(PresetHandler::getDataFolder()).getChildFile("projects.xml").replaceWithText(xml->createDocument(""));

}

void ProjectHandler::restoreWorkingProjects()
{
	ScopedPointer<XmlElement> xml = XmlDocument::parse(File(PresetHandler::getDataFolder()).getChildFile("projects.xml"));

	if (xml != nullptr)
	{
		jassert(xml->getTagName() == "Projects");

		File current = File(xml->getStringAttribute("current"));

		recentWorkDirectories.clear();

		for (int i = 0; i < xml->getNumChildElements(); i++)
		{
			recentWorkDirectories.add(xml->getChildElement(i)->getStringAttribute("path"));
		}

		setWorkingProject(current);

		jassert(currentWorkDirectory.exists() && currentWorkDirectory.isDirectory());

		
	}
}

bool ProjectHandler::isValidProjectFolder(const File &file) const
{
	if (file == File::nonexistent) return true;

	const bool isDirectory = file.exists() && file.isDirectory();

	if (!isDirectory) return false;
	
	for (int i = 0; i < (int)SubDirectories::numSubDirectories; i++)
	{
		File sub = file.getChildFile(getIdentifier((SubDirectories)i));

		if (!(sub.exists() && sub.isDirectory()))
		{
			PresetHandler::showMessageWindow("Invalid Project Folder", "The subfolder " + sub.getFileName() + " does not exist.");
			return false;
		}
	}

	return true;
}

File ProjectHandler::getLinkFile(const File &subDirectory)
{
    if(subDirectory.isSymbolicLink())
    {
        DBG("TUT");
    }
    
    
#if JUCE_MAC
    File childFile = subDirectory.getChildFile("LinkOSX");
#else
    File childFile = subDirectory.getChildFile("LinkWindows");
#endif
    
    return childFile;
}


void ProjectHandler::checkSubDirectories()
{
	subDirectories.clear();

	for (int i = 0; i < (int)ProjectHandler::SubDirectories::numSubDirectories; i++)
	{
		SubDirectories dir = (SubDirectories)i;

		File f = checkSubDirectory(dir);
		jassert(f.exists() && f.isDirectory());

		File subDirectory = currentWorkDirectory.getChildFile(getIdentifier(dir));
		File linkFile = getLinkFile(subDirectory);
		const bool redirected = linkFile.existsAsFile();

		subDirectories.add(FolderReference(dir, redirected, f));
	}
}

File ProjectHandler::checkSubDirectory(SubDirectories dir)
{
	File subDirectory = currentWorkDirectory.getChildFile(getIdentifier(dir));

	jassert(subDirectory.exists());

	File childFile = getLinkFile(subDirectory);

	if (childFile.existsAsFile())
	{
		String absolutePath = childFile.loadFileAsString();

		if (File::isAbsolutePath(absolutePath))
		{
			jassert(File(absolutePath).exists());
			return File(absolutePath);
		}
	}

	if (subDirectory.isDirectory())
	{
		return subDirectory;
	}
	else if (subDirectory.isSymbolicLink())
	{
		return subDirectory.getLinkedTarget();
	}
	else
	{
		const String fileExtension = subDirectory.getFileExtension();
		jassertfalse;

		return File::nonexistent;
	}
}

void ProjectHandler::checkSettingsFile()
{
	if (!getWorkDirectory().getChildFile("project_info.xml").existsAsFile())
	{
		setProjectSettings();
	}
}

File ProjectHandler::getSubDirectory(SubDirectories dir) const
{
    jassert(isActive());
    
	return subDirectories[(int)dir].file;
}

File ProjectHandler::getWorkDirectory() const
{
	if (!isActive())
	{
		return PresetHandler::getPresetFolder();
	}

	else return currentWorkDirectory;
}

String ProjectHandler::getIdentifier(SubDirectories dir) const
{
	switch (dir)
	{
	case ProjectHandler::SubDirectories::Scripts:			return "Scripts/";
	case ProjectHandler::SubDirectories::Binaries:			return "Binaries/";
	case ProjectHandler::SubDirectories::Presets:			return "Presets/";
	case ProjectHandler::SubDirectories::XMLPresetBackups:	return "XmlPresetBackups/";
	case ProjectHandler::SubDirectories::Samples:			return "Samples/";
	case ProjectHandler::SubDirectories::Images:			return "Images/";
	case ProjectHandler::SubDirectories::AudioFiles:		return "AudioFiles/";
	case ProjectHandler::SubDirectories::UserPresets:		return "UserPresets/";
	case ProjectHandler::SubDirectories::numSubDirectories: 
	default:												jassertfalse; return String::empty;
	}
}

bool ProjectHandler::isActive() const
{
	return currentWorkDirectory != File::nonexistent;
}

bool ProjectHandler::isRedirected(ProjectHandler::SubDirectories dir) const
{
	return subDirectories[(int)dir].isReference;
}

void ProjectHandler::createLinkFile(ProjectHandler::SubDirectories dir, const File &relocation)
{
    File subDirectory = currentWorkDirectory.getChildFile(getIdentifier(dir));
    
    File linkFile = getLinkFile(subDirectory);
    
    if(linkFile.existsAsFile())
    {
        if(!PresetHandler::showYesNoWindow("Already there", "Link redirect file exists. Do you want to replace it?"))
        {
            return;
        }
    }
    
    linkFile.create();
        
    linkFile.replaceWithText(relocation.getFullPathName());
}


void ProjectHandler::setProjectSettings(Component *mainEditor)
{
	SettingWindows::ProjectSettingWindow *w = new SettingWindows::ProjectSettingWindow(this);

	window = w;

	if (mainEditor == nullptr)
	{
		w->showOnDesktop();
	}
	else
	{
		w->setModalComponentOfMainEditor(mainEditor);
	}
}

#define GET_FILE(x) {if (x.contains(id)) \
                         return getSubDirectory(subDir).getChildFile(x.fromFirstOccurrenceOf(id, false, true)).getFullPathName(); \
                     return x;}

String ProjectHandler::getFilePath(const String &pathToFile, SubDirectories subDir) const
{
	if (File::isAbsolutePath(pathToFile)) return pathToFile;

    static String id = "{PROJECT_FOLDER}";
    
#if JUCE_MAC
    String pathToUse = pathToFile.replace("\\", "/");
    
    GET_FILE(pathToUse)
#else
    
    GET_FILE(pathToFile)
#endif
}

#undef GET_FILE

const String ProjectHandler::getFileReference(const String &absoluteFileName, SubDirectories dir) const
{
	static String id = "{PROJECT_FOLDER}";

	if (absoluteFileName.contains(id)) return absoluteFileName;


	File subDir = getSubDirectory(dir);

	if (absoluteFileName.contains(subDir.getFullPathName()))
	{
		String fileName = File(absoluteFileName).getRelativePathFrom(subDir);

		return id + fileName;
	}
	else return absoluteFileName;
}

StringArray ProjectHandler::recentWorkDirectories = StringArray();

// ================================================================================================================================ Preset Handler

void PresetHandler::checkProcessorIdsForDuplicates(Processor *rootProcessor, bool /*silentMode*/)
{

	bool duplicatesFound;

	do
	{
		duplicatesFound = false;
		Processor::Iterator<Processor> iter(rootProcessor);

		Processor *ip;

		OwnedArray<CountedProcessorId> countedProcessorIds;

		Processor *currentSynth = nullptr;
		Processor *currentChain = nullptr;

		while ((ip = iter.getNextProcessor()) != nullptr)
		{
			const bool isModulatorChain = dynamic_cast<ModulatorChain*>(ip) != nullptr;
			const bool isMidiProcessorChain = dynamic_cast<MidiProcessorChain*>(ip) != nullptr;
			const bool isEffectChain = dynamic_cast<EffectProcessorChain*>(ip) != nullptr;

			if (isModulatorChain || isMidiProcessorChain || isEffectChain)
			{
				currentChain = ip;
				continue;
			}

			if (dynamic_cast<ModulatorSynth*>(ip) != nullptr)
			{
				currentSynth = ip;
			}

			String currentName = ip->getId();

			bool found = false;

			for (int i = 0; i < countedProcessorIds.size(); i++)
			{
				if (countedProcessorIds[i]->processorName == currentName)
				{
					countedProcessorIds[i]->processors.add(ip);
					found = true;
					duplicatesFound = true;
					break;
				}
			}
			if (!found)
			{
				countedProcessorIds.insert(-1, new CountedProcessorId(ip));
			}
		}

		if (duplicatesFound)
		{
			for (int i = 0; i < countedProcessorIds.size(); i++)
			{
				if (countedProcessorIds[i]->processors.size() == 1) continue;

				CountedProcessorId *currentId = countedProcessorIds[i];

				for (int j = 1; j < currentId->processors.size(); j++)
				{
					String newId = currentId->processorName + String(j);

					currentId->processors[j]->setId(newId);
				}
			}
		}
	} 
	while (duplicatesFound);
}

File PresetHandler::getDirectory(Processor *p)
{
	if (GET_PROJECT_HANDLER(p).isActive())
	{
		return GET_PROJECT_HANDLER(p).getSubDirectory(ProjectHandler::SubDirectories::Presets);
	}
	else
	{
		String typeName;

		if (dynamic_cast<ModulatorSynth*>(p) != nullptr)		typeName = "ModulatorSynths";
		else if (dynamic_cast<Modulator*>(p) != nullptr)			typeName = "Modulators";
		else if (dynamic_cast<MidiProcessor*>(p) != nullptr)		typeName = "MidiProcessors";
		else if (dynamic_cast<EffectProcessor*>(p) != nullptr)		typeName = "EffectProcessors";
		else														{ jassertfalse; return File::nonexistent; }

		File directory((getPresetFolder().getFullPathName() + "/" + typeName));

		jassert(directory.exists());
		jassert(directory.isDirectory());

		return directory;
	}
}

File PresetHandler::checkFile(const String &pathName)
{

	if(!File::isAbsolutePath(pathName))
	{
#if WARN_MISSING_FILE
		FileChooser fc("Resolve missing reference for " + pathName);

		if(fc.browseForFileToOpen())
		{
			return fc.getResult();
		}
		else return File::nonexistent;	
#else
		return File::nonexistent;
#endif

	}
	else
	{
#if WARN_MISSING_FILE
		File f = File(pathName);

		if(!f.existsAsFile())
		{
			FileChooser fc("Resolve missing reference for " + pathName);

			if(fc.browseForFileToOpen())
			{
				return fc.getResult();
			}
			else return File::nonexistent;
		}
		else return f;
#else
		return File::nonexistent;
#endif
	}
}

File PresetHandler::checkDirectory(const String &pathName)
{
	if(!File::isAbsolutePath(pathName))
	{
		FileChooser fc("Resolve missing reference for " + pathName);

		if(fc.browseForDirectory())
		{
			return fc.getResult();
		}
		else return File::nonexistent;		
	}
	else
	{
		File f = File(pathName);

		if(!f.exists() || !f.isDirectory())
		{
			FileChooser fc("Resolve missing reference for " + pathName);

			if(fc.browseForDirectory())
			{
				return fc.getResult();
			}
			else return File::nonexistent;
		}
		else return f;

	}
}


PopupMenu PresetHandler::getAllSavedPresets(int minIndex, Processor *p)
{

	

	PopupMenu m;
	File directoryToScan = PresetHandler::getDirectory(p);
	DirectoryIterator directoryIterator(directoryToScan, false, "*", File::TypesOfFileToFind::findFilesAndDirectories);

	while(directoryIterator.next())
	{
		File directory = directoryIterator.getFile();

		if (directory.isDirectory())
		{
			PopupMenu sub;
			DirectoryIterator presetIterator(directory, false, "*.hip", File::TypesOfFileToFind::findFiles);

			while(presetIterator.next())
			{
				File preset = presetIterator.getFile();

				sub.addItem(minIndex++, preset.getFileNameWithoutExtension());
			}

			m.addSubMenu(directory.getFileName(), sub, true);

		}
		else if (directory.hasFileExtension(".hip"))
		{
			m.addItem(minIndex++, directory.getFileNameWithoutExtension());
		}

	}

	return m;
}

File PresetHandler::getPresetFileFromMenu(int menuIndexDelta, Processor *parent)
{
	File directory = getDirectory(parent);
	DirectoryIterator it(directory, true, "*", File::findFilesAndDirectories);
	DirectoryIterator directoryIterator(directory, false, "*", File::TypesOfFileToFind::findFilesAndDirectories);

	int i = 0;

	while (directoryIterator.next())
	{
		File fileToCheck = directoryIterator.getFile();

		if (fileToCheck.isDirectory())
		{
			DirectoryIterator presetIterator(fileToCheck, false, "*.hip", File::TypesOfFileToFind::findFiles);

			while (presetIterator.next())
			{
				if (i == menuIndexDelta) return presetIterator.getFile();
				i++;
			}
		}

		else if (fileToCheck.hasFileExtension(".hip"))
		{
			if (i == menuIndexDelta) return directoryIterator.getFile();

			i++;
		}
	}

	return File::nonexistent;
}


Processor *PresetHandler::createProcessorFromPreset(int menuIndexDelta, Processor *parent)
{
	File f = getPresetFileFromMenu(menuIndexDelta, parent);

	if (f.existsAsFile())
	{
		return loadProcessorFromFile(f, parent);
	}
	else
	{
		debugToConsole(parent, "Nothing found");
		jassertfalse;
		return nullptr;
	}

	
}


Processor* PresetHandler::createProcessorFromClipBoard(Processor *parent)
{
	try
	{
		String x = SystemClipboard::getTextFromClipboard();
		ScopedPointer<XmlElement> parsedXml = XmlDocument::parse(x);
		ValueTree v = ValueTree::fromXml(*parsedXml);

		if(parsedXml->getStringAttribute("ID") != v.getProperty("ID", String::empty).toString() )
		{
			jassertfalse;
			debugToConsole(parent, "Clipboard could not be loaded");
			return nullptr;
		};

		String name = v.getProperty("ID", "Unnamed");
		
#if USE_OLD_FILE_FORMAT
		Identifier type = v.getType();
#else
		Identifier type = v.getProperty("Type", String::empty).toString();
#endif

		FactoryType *t = dynamic_cast<Chain*>(parent)->getFactoryType();
		
		// Look in every processor when inserting from clipboard.

		const bool validType = type.isValid();
		const bool allowedType = t->allowType(type.toString());

		if(!(validType && allowedType) )
		{
			jassertfalse;
			return nullptr;
		}

		Processor *p = MainController::createProcessor(t, type.toString(), name);
		p->restoreFromValueTree(v);

		p->setId(FactoryType::getUniqueName(p));

		debugToConsole(p, name + " added from Clipboard.");

		return p;
	}
	catch(...)
	{
		debugToConsole(parent, "ERROR WHILE READING");
		jassertfalse;
		return nullptr;
	};
}

void PresetHandler::setUniqueIdsForProcessor(Processor * p)
{
	const String uniqueId = FactoryType::getUniqueName(p);
	p->setId(uniqueId);

	if (ProcessorHelpers::is<ModulatorSynthChain>(p) || ProcessorHelpers::is<ModulatorSynthGroup>(p))
	{
		const int numChildSynths = dynamic_cast<Chain*>(p)->getHandler()->getNumProcessors();

		for (int i = 0; i < numChildSynths; i++)
		{
			Processor *cp = dynamic_cast<Chain*>(p)->getHandler()->getProcessor(i);

			setUniqueIdsForProcessor(cp);
		}
	}
	else
	{
		for (int i = 0; i < p->getNumChildProcessors(); i++)
		{
			Chain *childChain = dynamic_cast<Chain*>(p->getChildProcessor(i));

			for (int j = 0; j < childChain->getHandler()->getNumProcessors(); j++)
			{
				Processor *cp = childChain->getHandler()->getProcessor(j);

				const String uniqueId = FactoryType::getUniqueName(cp);

				cp->setId(uniqueId);
			}
		}
	}
}

Processor *PresetHandler::loadProcessorFromFile(File fileName, Processor *parent)
{
	FileInputStream fis(fileName);
	ValueTree v = ValueTree::readFromStream(fis);

	if(v.getType() != Identifier("Processor"))
	{
		debugToConsole(parent, "Old file format detected, converting to new format ...");

		v = PresetHandler::changeFileStructureToNewFormat(v);

	}

	String name = v.getProperty("ID", "Unnamed");

	// Look in every processor when inserting from clipboard.

#if USE_OLD_FILE_FORMAT

	if(fileName.getFileNameWithoutExtension() != v.getProperty("ID", String::empty).toString() )
	{

		jassertfalse;
		debugToConsole(parent, fileName.getFullPathName() + " could not be loaded");
		return nullptr;
	};

	Processor *p = MainController::createProcessor(dynamic_cast<Chain*>(parent)->getFactoryType(), 
																			String(v.getType()), 
																			name);
#else
	Processor *p = MainController::createProcessor(dynamic_cast<Chain*>(parent)->getFactoryType(), 
																			Identifier(v.getProperty("Type", String::empty)).toString(), 
																			name);
#endif

	

	if(p != nullptr)
	{
		p->restoreFromValueTree(v);
		
		

		debugToConsole(parent, fileName.getFileNameWithoutExtension() + " was loaded");
		return p;
	}
	else
	{
		debugToConsole(parent, "Error with loading " +  fileName.getFileNameWithoutExtension());

		return nullptr;
	}	
}



void PresetHandler::buildProcessorDataBase(Processor *root)
{
	File f(getDataFolder() + "/moduleEnums.xml");

	if (f.existsAsFile()) return;

	ScopedPointer<XmlElement> xml = new XmlElement("Parameters");
	

	ScopedPointer<FactoryType> t = new ModulatorSynthChainFactoryType(NUM_POLYPHONIC_VOICES, root);

	xml->addChildElement(buildFactory(t, "ModulatorSynths"));

	t = new MidiProcessorFactoryType(root);
	xml->addChildElement(buildFactory(t, "MidiProcessors"));


	t = new VoiceStartModulatorFactoryType(NUM_POLYPHONIC_VOICES, Modulation::GainMode, root);
	xml->addChildElement(buildFactory(t, "VoiceStartModulators"));

	t = new TimeVariantModulatorFactoryType(Modulation::GainMode, root);

	xml->addChildElement(buildFactory(t, "TimeVariantModulators"));

	t = new EnvelopeModulatorFactoryType(NUM_POLYPHONIC_VOICES, Modulation::GainMode, root);

	xml->addChildElement(buildFactory(t, "EnvelopeModulators"));

	t = new EffectProcessorChainFactoryType(NUM_POLYPHONIC_VOICES, root);

	xml->addChildElement(buildFactory(t, "Effects"));


	xml->writeToFile(f, "");
}

XmlElement * PresetHandler::buildFactory(FactoryType *t, const String &factoryName)
{
	
	XmlElement *xml = new XmlElement(factoryName);

	for (int j = 0; j < t->getNumProcessors(); j++)
	{

		ScopedPointer<Processor> p = t->createProcessor(j, "X");

		if (p == nullptr) continue;

		XmlElement *child = new XmlElement(p->getType());

		for (int i = 0; i < p->getNumParameters(); i++)
		{
			Identifier id = p->getIdentifierForParameterIndex(i);


			child->setAttribute(Identifier("id" + String(i)), id.toString());
		}
        
        

		xml->addChildElement(child);
	}

	return xml;
}

String PresetHandler::getGlobalSampleFolder()
{
	checkDirectory(false);

#if JUCE_WINDOWS

	String returnString = WindowsRegistry::getValue("HKEY_CURRENT_USER\\Software\\Hart Instruments\\GlobalSampleFolder");

	jassert(returnString.isNotEmpty());

	return returnString;

#else
    
	return getSettingsValue("GlobalSampleFolder");

#endif
}

String PresetHandler::getDataFolder()
{
#if JUCE_WINDOWS

    return File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "/Hart Instruments";

#else

    return File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "/Application Support/Hart Instruments";

#endif
    
}

void PresetHandler::writeSampleMapsToValueTree(ValueTree &sampleMapTree, ValueTree &preset)
{
	if (preset.getChildWithName("samplemap").isValid())
	{
		ValueTree copy = preset.getChildWithName("samplemap");

		preset.removeChild(copy, nullptr);

		copy.setProperty("FileName", preset.getProperty("ID"), nullptr);

		sampleMapTree.addChild(copy, -1, nullptr);
	}

	ValueTree childProcessors = preset.getChildWithName("ChildProcessors");

	if (childProcessors.isValid())
	{
		for (int i = 0; i < childProcessors.getNumChildren(); i++)
		{
			ValueTree child = childProcessors.getChild(i);

			writeSampleMapsToValueTree(sampleMapTree, child);
		}
	}
}

void CompileExporter::exportMainSynthChainAsPackage(ModulatorSynthChain *chainToExport)
{
	String uniqueId, version, solutionDirectory, publicKey;
	BuildOption buildOption = showCompilePopup(publicKey, uniqueId, version, solutionDirectory);
	
	publicKey = "12345678"; // Todo
	uniqueId = SettingWindows::getSettingValue((int)SettingWindows::ProjectSettingWindow::Attributes::Name, &GET_PROJECT_HANDLER(chainToExport));
	version = SettingWindows::getSettingValue((int)SettingWindows::ProjectSettingWindow::Attributes::Version, &GET_PROJECT_HANDLER(chainToExport));

	solutionDirectory = GET_PROJECT_HANDLER(chainToExport).getSubDirectory(ProjectHandler::SubDirectories::Binaries).getFullPathName();

	if (buildOption != Cancelled)
	{
		createPluginDataHeaderFile(solutionDirectory, uniqueId, version, publicKey);
		createResourceFile(solutionDirectory, uniqueId, version);

		const String directoryPath = solutionDirectory + "/temp";

		writePresetFile(chainToExport, directoryPath, uniqueId);
		writeReferencedAudioFiles(chainToExport, directoryPath);
		writeReferencedImageFiles(chainToExport, directoryPath);

		String presetDataString("PresetData");

		String sourceDirectory = solutionDirectory + "/Source";

		CppBuilder::exportValueTreeAsCpp(directoryPath, sourceDirectory, presetDataString);

		compileSolution(solutionDirectory, uniqueId, buildOption);
		
	}
}


CompileExporter::BuildOption CompileExporter::showCompilePopup(String &publicKey, String &uniqueId, String &version, String &/*solutionDirectory*/)
{
	AlertWindowLookAndFeel pplaf;

	AlertWindow w("Compile Patch as plugin", String::empty, AlertWindow::AlertIconType::NoIcon);

	w.setLookAndFeel(&pplaf);

	w.setUsingNativeTitleBar(true);

	w.setColour(AlertWindow::backgroundColourId, Colour(0xff222222));
	w.setColour(AlertWindow::textColourId, Colours::white);
	
	StringArray items;

	items.add("32bit VST");
	items.add("64bit VST");
	items.add("32bit & 64bit VSTs");

	w.addComboBox("buildOption", items, "Plugin Format");


	w.addButton("OK", 1, KeyPress(KeyPress::returnKey));
	w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	w.getComboBoxComponent("buildOption")->setLookAndFeel(&pplaf);

	if (w.runModalLoop())
	{
		int i = w.getComboBoxComponent("buildOption")->getSelectedItemIndex() + 1;

		return (BuildOption)i;
	}
	else
	{
		return Cancelled;
	}
	
}

void CompileExporter::writeReferencedImageFiles(ModulatorSynthChain * chainToExport, const String directoryPath)
{
	// Export the interface

	ImagePool *imagePool = chainToExport->getMainController()->getSampleManager().getImagePool();
	ValueTree imageTree = imagePool->exportAsValueTree();
	PresetHandler::writeValueTreeAsFile(imageTree, directoryPath + "/images");
}

void CompileExporter::writeReferencedAudioFiles(ModulatorSynthChain * chainToExport, const String directoryPath)
{
	// Search for impulse responses

	AudioSampleBufferPool *samplePool = chainToExport->getMainController()->getSampleManager().getAudioSampleBufferPool();
	ValueTree sampleTree = samplePool->exportAsValueTree();
	PresetHandler::writeValueTreeAsFile(sampleTree, directoryPath + "/impulses");
}

void CompileExporter::writePresetFile(ModulatorSynthChain * chainToExport, const String directoryPath, const String &uniqueName)
{
	ValueTree preset = chainToExport->exportAsValueTree();

#if USE_OLD_FILE_FORMAT

	// Search for sample maps
	SampleMapSearcher searcher = SampleMapSearcher(preset);
	searcher.search();
	StringArray sampleMapPaths = searcher.getFileNames();

	preset = searcher.getStrippedPreset();

	// Export the sample maps

	SampleMapExporter exporter(sampleMapPaths);

#else

	ValueTree sampleMapTree("samplemaps");

	PresetHandler::writeSampleMapsToValueTree(sampleMapTree, preset);

	SampleMapExporter exporter(sampleMapTree);

#endif

	exporter.exportSamples(directoryPath, uniqueName, true);

	PresetHandler::stripViewsFromPreset(preset);

	File presetFile(directoryPath + "/preset");

	ReferenceSearcher::writeValueTreeToFile(preset, presetFile);

	

}

void CompileExporter::compileSolution(const String &solutionDirectory, const String &uniqueId, BuildOption buildOption)
{
	if (buildOption == VSTx86 || buildOption == VSTx64x86)
	{
		String batchCommand = solutionDirectory + "/batch32.bat";

		batchCommand = "\"" + batchCommand.replaceCharacter('/', '\\') + "\"" + " " + uniqueId + " ";

		const char *batchC = batchCommand.getCharPointer();

		system(batchC);
	}
	
	if (buildOption == VSTx64 || buildOption == VSTx64x86)
	{
		String batchCommand = solutionDirectory + "/batch64.bat";

		batchCommand = "\"" + batchCommand.replaceCharacter('/', '\\') + "\"" + " " + uniqueId + " ";

		const char *batchC = batchCommand.getCharPointer();

		system(batchC);
	}

}

void CompileExporter::createPluginDataHeaderFile(const String &solutionDirectory, const String &uniqueName, const String &version, const String &publicKey)
{
	

	jassert(version.matchesWildcard("*.*.*.*", true));

	StringArray segments = StringArray::fromTokens(version, ".", "");

	int versionAsHex = (segments[0].getIntValue() << 16)
		+ (segments[1].getIntValue() << 8)
		+ segments[2].getIntValue();

	if (segments.size() >= 4)
		versionAsHex = (versionAsHex << 8) + segments[3].getIntValue();

	String hexString = "0x" + String::toHexString(versionAsHex);

	//String publicKey = "3,57eaf85713e943702068568b343569b4b2eb497211bad3f06b701e65ce9a701d"; // hardcoded, make this dynamic

	String author = "Christoph Hart";

	String pluginDataHeaderFile;

	pluginDataHeaderFile << "#ifndef PLUGINDATA_H_INCLUDED\n";
	pluginDataHeaderFile << "#define PLUGINDATA_H_INCLUDED\n";
	pluginDataHeaderFile << "\n";
	pluginDataHeaderFile << "#define PRODUCT_ID \"" << uniqueName << "\"\n";
	pluginDataHeaderFile << "#define AUTHOR \"" << author << "\"\n";
	if (publicKey.isNotEmpty())
	{
		pluginDataHeaderFile << "#define PUBLIC_KEY \"" << publicKey << "\"\n";
	}
	else
	{
		pluginDataHeaderFile << "#define PUBLIC_KEY \"UNUSED\"\n";
		pluginDataHeaderFile << "#define USE_COPY_PROTECTION 0\n";
	}
	pluginDataHeaderFile << "#define PRODUCT_VERSION " << hexString << "\n";
	pluginDataHeaderFile << "#define PRODUCT_VERSION_STRING \"" << version << "\"\n";
	pluginDataHeaderFile << "\n";
	pluginDataHeaderFile << "#endif";

	File pluginDataHeader(solutionDirectory + "/Source/PluginData.h");

	jassert(pluginDataHeader.existsAsFile());
	pluginDataHeader.deleteFile();

	pluginDataHeader.create();
	pluginDataHeader.appendText(pluginDataHeaderFile);


	

}

void CompileExporter::createResourceFile(const String &solutionDirectory, const String & uniqueName, const String &version)
{
	// WRITE THE RESOURCES.RC file

	String resourcesFile;

	resourcesFile << "#undef  WIN32_LEAN_AND_MEAN" << "\n";
	resourcesFile << "#define WIN32_LEAN_AND_MEAN" << "\n";
	resourcesFile << "#include <windows.h>" << "\n";
	resourcesFile << "" << "\n";
	resourcesFile << "VS_VERSION_INFO VERSIONINFO" << "\n";
	resourcesFile << "FILEVERSION " << version.replaceCharacter('.', ',') << "\n";
	resourcesFile << "PRODUCTVERSION " << version.replaceCharacter('.', ',') << "\n";
	resourcesFile << "BEGIN" << "\n";
	resourcesFile << "  BLOCK \"StringFileInfo\"" << "\n";
	resourcesFile << "  BEGIN" << "\n";
	resourcesFile << "    BLOCK \"040904E4\"" << "\n";
	resourcesFile << "    BEGIN" << "\n";
	resourcesFile << "      VALUE \"CompanyName\", \"Hart Instruments\\0\"" << "\n";
	resourcesFile << "      VALUE \"FileDescription\", \"" << uniqueName << "\\0\"" << "\n";
	resourcesFile << "      VALUE \"FileVersion\", \"" << version << "\\0\"" << "\n";
	resourcesFile << "      VALUE \"ProductName\", \"" << uniqueName << "\\0\"" << "\n";
	resourcesFile << "      VALUE \"ProductVersion\", \"" << version << "\\0\"" << "\n";
	resourcesFile << "    END" << "\n";
	resourcesFile << "  END" << "\n";
	resourcesFile << "" << "\n";
	resourcesFile << "  BLOCK \"VarFileInfo\"" << "\n";
	resourcesFile << "  BEGIN" << "\n";
	resourcesFile << "    VALUE \"Translation\", 0x409, 65001" << "\n";
	resourcesFile << "  END" << "\n";
	resourcesFile << "END" << "\n";

	File resourcesFileObject(solutionDirectory + "/Builds/VisualStudio2013/resources.rc");

	resourcesFileObject.deleteFile();

	resourcesFileObject.create();
	resourcesFileObject.appendText(resourcesFile);
}

//==============================================================================
int CppBuilder::addFile (const File& file,
                    const String& classname,
                    OutputStream& headerStream,
                    OutputStream& cppStream)
{
    MemoryBlock mb;
    file.loadFileAsData (mb);

    const String name (file.getFileName()
                           .replaceCharacter (' ', '_')
                           .replaceCharacter ('.', '_')
                           .retainCharacters ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789"));

    std::cout << "Adding " << name << ": "
              << (int) mb.getSize() << " bytes" << std::endl;

    headerStream << "    extern const char*  " << name << ";\r\n"
                    "    const int           " << name << "Size = "
                 << (int) mb.getSize() << ";\r\n\r\n";

    static int tempNum = 0;

    cppStream << "static const unsigned char temp" << ++tempNum << "[] = {";

    size_t i = 0;
    const uint8* const data = (const uint8*) mb.getData();

    while (i < mb.getSize() - 1)
    {
        if ((i % 40) != 39)
            cppStream << (int) data[i] << ",";
        else
            cppStream << (int) data[i] << ",\r\n  ";

        ++i;
    }

    cppStream << (int) data[i] << ",0,0};\r\n";

    cppStream << "const char* " << classname << "::" << name
              << " = (const char*) temp" << tempNum << ";\r\n\r\n";

    return (int)mb.getSize();
}

bool CppBuilder::isHiddenFile (const File& f, const File& root)
{
    return f.getFileName().endsWithIgnoreCase (".scc")
         || f.getFileName() == ".svn"
         || f.getFileName().startsWithChar ('.')
         || (f.getSize() == 0 && ! f.isDirectory())
         || (f.getParentDirectory() != root && isHiddenFile (f.getParentDirectory(), root));
}


//==============================================================================
int CppBuilder::exportValueTreeAsCpp (const File &sourceDirectory, const File &destDirectory, String &className)
{
	className = className.trim();

    const File headerFile (destDirectory.getChildFile (className).withFileExtension (".h"));
    const File cppFile    (destDirectory.getChildFile (className).withFileExtension (".cpp"));

    std::cout << "Creating " << headerFile.getFullPathName()
              << " and " << cppFile.getFullPathName()
              << " from files in " << sourceDirectory.getFullPathName()
              << "..." << std::endl << std::endl;

	headerFile.deleteFile();
    cppFile.deleteFile();

    Array<File> files;
    sourceDirectory.findChildFiles (files, File::findFiles, false, "*");

    ScopedPointer<OutputStream> header (headerFile.createOutputStream());

    if (header == nullptr)
    {
        std::cout << "Couldn't open "
                  << headerFile.getFullPathName() << " for writing" << std::endl << std::endl;
        return 0;
    }

    ScopedPointer<OutputStream> cpp (cppFile.createOutputStream());

    if (cpp == nullptr)
    {
        std::cout << "Couldn't open "
                  << cppFile.getFullPathName() << " for writing" << std::endl << std::endl;
        return 0;
    }

    *header << "/* (Auto-generated binary data file). */\r\n\r\n"
               "#ifndef BINARY_" << className.toUpperCase() << "_H\r\n"
               "#define BINARY_" << className.toUpperCase() << "_H\r\n\r\n"
               "namespace " << className << "\r\n"
               "{\r\n";

    *cpp << "/* (Auto-generated binary data file). */\r\n\r\n"
            "#include \"" << className << ".h\"\r\n\r\n";

    int totalBytes = 0;

    for (int i = 0; i < files.size(); ++i)
    {
        const File file (files[i]);

        // (avoid source control files and hidden files..)
        if (! isHiddenFile (file, sourceDirectory))
        {
            if (file.getParentDirectory() != sourceDirectory)
            {
                *header << "  #ifdef " << file.getParentDirectory().getFileName().toUpperCase() << "\r\n";
                *cpp << "#ifdef " << file.getParentDirectory().getFileName().toUpperCase() << "\r\n";

                totalBytes += addFile (file, className, *header, *cpp);

                *header << "  #endif\r\n";
                *cpp << "#endif\r\n";
            }
            else
            {
                totalBytes += addFile (file, className, *header, *cpp);
            }
        }
    }

    *header << "}\r\n\r\n"
               "#endif\r\n";

    header = nullptr;
    cpp = nullptr;

    std::cout << std::endl << " Total size of binary data: " << totalBytes << " bytes" << std::endl;

    return 0;
}


void AboutPage::paint(Graphics &g)
{
#if USE_BACKEND

	g.fillAll(Colour(BACKEND_BG_COLOUR_BRIGHT));
	g.setColour(Colour(0xFFbbbbbb));

#else

	g.fillAll(Colour(0xFF252525));
	g.setColour(Colours::white.withAlpha(0.4f));

#endif

	Path p;
	p.loadPathFromData(HiBinaryData::commonBinaryData::aboutHeader, sizeof(HiBinaryData::commonBinaryData::aboutHeader));





	g.drawRect(getLocalBounds(), 1);

	p.scaleToFit(40.0f, 20.0f, (float)getWidth() - 80.0f, 90.0f, true);
	g.fillPath(p);

	infoData.draw(g, Rectangle<float>(40.0f, 130.0f, getWidth() - 80.0f, getHeight() - 130.0f));

	g.drawHorizontalLine(120, 40.0f, (float)getWidth() - 40.0f);

}


void AboutPage::refreshText()
{
	infoData.clear();

	Font normal = GLOBAL_FONT();
	Font bold = GLOBAL_BOLD_FONT();
	

#if USE_FRONTEND

	Colour bright(0xFF999999);

	infoData.append("Product: ", bold, bright);
	infoData.append(PRODUCT_ID, normal, bright);
	infoData.append("\nVersion: ", bold, bright);
	infoData.append(String(PRODUCT_VERSION_STRING), normal, bright);
	infoData.append("\nBuild date: ", bold, bright);
	infoData.append(__DATE__, normal, bright);
	infoData.append("\nCreated by: ", bold, bright);
	infoData.append(AUTHOR, normal, bright);
	infoData.append("\n\nRegistered to: ", bold, bright);
	infoData.append(userEmail, normal, bright);

#else


	Colour bright(0xFFbbbbbb);

	infoData.append("HISE beta\n", bold, bright);
	infoData.append("Hart Instruments Sampler Engine\n", normal, bright);

	infoData.append("\nVersion: ", bold, bright);
	infoData.append(String(JUCE_APP_VERSION), normal, bright);
	infoData.append("\nBuild time: ", bold, bright);
	infoData.append(Time::getCompilationDate().toString(true, false, false, true), normal, bright);
	infoData.append("\nBuild version: ", bold, bright);
	infoData.append(String(BUILD_SUB_VERSION), normal, bright);


#if USE_IPP
	infoData.append("\n\naccelerated by FFT routines from the IPP library\n", normal, bright);
#endif

	infoData.append("\nCreated by: ", bold, bright);
	infoData.append("Christoph Hart", normal, bright);


#endif

	repaint();
}

void AboutPage::buttonClicked(Button *)
{

}

String PresetPlayerHandler::getSpecialFolder(FolderType type, const String &packageName /*= String::empty*/, bool ignoreMissingDirectory)
{
	File globalSampleFolder = PresetHandler::getGlobalSampleFolder();

	String packageToUse = packageName;

	
	String returnFile;

	switch (type)
	{
	case FolderType::GlobalSampleDirectory: returnFile = globalSampleFolder.getFullPathName(); break;
	case FolderType::PackageDirectory:		returnFile = globalSampleFolder.getFullPathName() + "/" + packageToUse; break;
	case FolderType::StreamedSampleFolder:	returnFile = globalSampleFolder.getFullPathName() + "/" + packageToUse + "/streamed_samples"; break;
	case FolderType::AudioFiles:			returnFile = globalSampleFolder.getFullPathName() + "/" + packageToUse + "/audio_files"; break;
	case FolderType::ImageResources:		returnFile = globalSampleFolder.getFullPathName() + "/" + packageToUse + "/image_resources"; break;
	default: jassertfalse;
	}

	// You must create the package before using it!
	jassert(ignoreMissingDirectory || File(returnFile).isDirectory());

	return returnFile;
}



void PresetPlayerHandler::checkAndCreatePackage(const String &packageName)
{
	
	File packageFolder = File(getSpecialFolder(PackageDirectory, packageName, true));

	if (packageFolder.isDirectory())
	{

	}
	else
	{
		packageFolder.createDirectory();

		File audioFileFolder = getSpecialFolder(AudioFiles, packageName, true);
		File imageFolder = getSpecialFolder(ImageResources, packageName, true);
		File sampleFolder = getSpecialFolder(StreamedSampleFolder, packageName, true);

		audioFileFolder.createDirectory();
		imageFolder.createDirectory();
		sampleFolder.createDirectory();

		String niceName = PresetHandler::getCustomName("Nice Package Name");
		String version = PresetHandler::getCustomName("Version");
		String author = PresetHandler::getCustomName("Author");

		ScopedPointer<XmlElement> packageXml = new XmlElement("package");
		packageXml->setAttribute("name", niceName);
		packageXml->setAttribute("version", version);
		packageXml->setAttribute("author", author);

		File xmlFile(packageFolder.getFullPathName() + "/package.xml");

		packageXml->writeToFile(xmlFile, "");
	}
}

void PresetPlayerHandler::addInstrumentToPackageXml(const String &instrumentFileName, const String &packageName)
{
	checkAndCreatePackage(packageName);

	File xmlFile(getSpecialFolder(PackageDirectory, packageName) + "/package.xml");

	ScopedPointer<XmlElement> xml = XmlDocument::parse(xmlFile);

	jassert(xml != nullptr);

	if (xml != nullptr)
	{
		XmlElement *child = new XmlElement("instrument");
		child->setAttribute("name", instrumentFileName);
		child->setAttribute("file", instrumentFileName + ".hip");

		xml->addChildElement(child);

		xml->writeToFile(xmlFile, "");
	}


}