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

ToolbarItemComponent * MainToolbarFactory::createItem(int itemId)
{
	ToolbarButton *b = new ToolbarButton(itemId, "", MainToolbarPaths::createPath(itemId, false),
												 MainToolbarPaths::createPath(itemId, true));
	
	b->setCommandToTrigger(dynamic_cast<MainController*>(editor->getAudioProcessor())->getCommandManager(), itemId, true);

	return b;
}



void MainToolbarFactory::getAllToolbarItemIds(Array<int> &ids)
{
    const CommandID id[] = { BackendProcessorEditor::ModulatorList,
        BackendProcessorEditor::CustomInterface,
        //BackendProcessorEditor::DebugPanel,
        //BackendProcessorEditor::ViewPanel,
        //BackendProcessorEditor::Mixer,
        BackendProcessorEditor::Macros,
        BackendProcessorEditor::Keyboard,
		ToolbarItemFactory::spacerId,
		BackendCommandTarget::MenuViewBack,
		BackendCommandTarget::MenuViewForward
        //BackendProcessorEditor::Settings
    };

    
	ids.addArray(id, numElementsInArray(id));
};

Drawable *MainToolbarFactory::MainToolbarPaths::createPath(int id, bool isOn)
{
	Path path;

	switch(id)
	{
	case BackendProcessorEditor::CustomInterface:
		{
		
		path.loadPathFromData (BackendBinaryData::ToolbarIcons::customInterface, sizeof (BackendBinaryData::ToolbarIcons::customInterface));

		break;
		}
	case BackendProcessorEditor::ModulatorList:
		{
			path.loadPathFromData (BackendBinaryData::ToolbarIcons::modulatorList, sizeof (BackendBinaryData::ToolbarIcons::modulatorList));

		break;

		}

	case BackendProcessorEditor::ViewPanel:
		{
		
		path.loadPathFromData (BackendBinaryData::ToolbarIcons::viewPanel, sizeof (BackendBinaryData::ToolbarIcons::viewPanel));
		break;
		}
	case BackendProcessorEditor::Mixer:
		{
		path.loadPathFromData (BackendBinaryData::ToolbarIcons::mixer, sizeof (BackendBinaryData::ToolbarIcons::mixer));
		break;

		}
	case BackendProcessorEditor::Keyboard:
		{
		path.loadPathFromData (BackendBinaryData::ToolbarIcons::keyboard, sizeof (BackendBinaryData::ToolbarIcons::keyboard));
		break;

		}
	case BackendProcessorEditor::DebugPanel:
		{
		path.loadPathFromData (BackendBinaryData::ToolbarIcons::debugPanel, sizeof (BackendBinaryData::ToolbarIcons::debugPanel));
		break;
		}
	case BackendProcessorEditor::Settings:
		{
			path.loadPathFromData (BackendBinaryData::ToolbarIcons::settings, sizeof (BackendBinaryData::ToolbarIcons::settings));
		break;
		}
	case BackendProcessorEditor::Macros:
		{
			path.loadPathFromData (BackendBinaryData::ToolbarIcons::macros, sizeof (BackendBinaryData::ToolbarIcons::macros));
		break;
		}
	case BackendCommandTarget::MenuViewBack:
	{
		static const unsigned char pathData[] = { 110, 109, 0, 0, 62, 67, 0, 239, 122, 67, 108, 0, 0, 62, 67, 0, 239, 122, 67, 108, 27, 0, 61, 67, 102, 245, 122, 67, 108, 218, 0, 60, 67, 148, 8, 123, 67, 108, 224, 2, 59, 67, 126, 40, 123, 67, 108, 208, 6, 58, 67, 15, 85, 123, 67, 108, 75, 13, 57, 67, 43, 142, 123, 67, 108, 240, 22, 56, 67, 173, 211, 123, 67, 108, 93, 36, 55,
			67, 105, 37, 124, 67, 108, 46, 54, 54, 67, 43, 131, 124, 67, 108, 250, 76, 53, 67, 182, 236, 124, 67, 108, 87, 105, 52, 67, 199, 97, 125, 67, 108, 215, 139, 51, 67, 19, 226, 125, 67, 108, 8, 181, 50, 67, 72, 109, 126, 67, 108, 114, 229, 49, 67, 13, 3, 127, 67, 108, 155, 29, 49, 67, 2, 163, 127, 67, 108, 2, 94, 48, 67, 96,
			38, 128, 67, 108, 35, 167, 47, 67, 238, 127, 128, 67, 108, 114, 249, 46, 67, 242, 221, 128, 67, 108, 94, 85, 46, 67, 46, 64, 129, 67, 108, 80, 187, 45, 67, 100, 166, 129, 67, 108, 171, 43, 45, 67, 84, 16, 130, 67, 108, 203, 166, 44, 67, 184, 125, 130, 67, 108, 5, 45, 44, 67, 75, 238, 130, 67, 108, 166, 190, 43, 67, 198,
			97, 131, 67, 108, 246, 91, 43, 67, 221, 215, 131, 67, 108, 51, 5, 43, 67, 70, 80, 132, 67, 108, 149, 186, 42, 67, 180, 202, 132, 67, 108, 76, 124, 42, 67, 216, 70, 133, 67, 108, 127, 74, 42, 67, 98, 196, 133, 67, 108, 79, 37, 42, 67, 3, 67, 134, 67, 108, 211, 12, 42, 67, 105, 194, 134, 67, 108, 27, 1, 42, 67, 68, 66, 135,
			67, 108, 0, 0, 42, 67, 128, 119, 135, 67, 108, 0, 0, 42, 67, 128, 119, 135, 67, 108, 0, 0, 42, 67, 128, 119, 135, 67, 108, 102, 6, 42, 67, 114, 247, 135, 67, 108, 148, 25, 42, 67, 19, 119, 136, 67, 108, 126, 57, 42, 67, 16, 246, 136, 67, 108, 15, 102, 42, 67, 24, 116, 137, 67, 108, 43, 159, 42, 67, 219, 240, 137, 67, 108,
			174, 228, 42, 67, 9, 108, 138, 67, 108, 106, 54, 43, 67, 82, 229, 138, 67, 108, 44, 148, 43, 67, 106, 92, 139, 67, 108, 183, 253, 43, 67, 4, 209, 139, 67, 108, 200, 114, 44, 67, 213, 66, 140, 67, 108, 20, 243, 44, 67, 149, 177, 140, 67, 108, 74, 126, 45, 67, 253, 28, 141, 67, 108, 15, 20, 46, 67, 200, 132, 141, 67, 108,
			4, 180, 46, 67, 180, 232, 141, 67, 108, 195, 93, 47, 67, 128, 72, 142, 67, 108, 223, 16, 48, 67, 240, 163, 142, 67, 108, 229, 204, 48, 67, 200, 250, 142, 67, 108, 94, 145, 49, 67, 210, 76, 143, 67, 108, 203, 93, 50, 67, 217, 153, 143, 67, 108, 170, 49, 51, 67, 171, 225, 143, 67, 108, 114, 12, 52, 67, 27, 36, 144, 67, 108,
			153, 237, 52, 67, 254, 96, 144, 67, 108, 142, 212, 53, 67, 46, 152, 144, 67, 108, 189, 192, 54, 67, 134, 201, 144, 67, 108, 143, 177, 55, 67, 231, 244, 144, 67, 108, 107, 166, 56, 67, 54, 26, 145, 67, 108, 178, 158, 57, 67, 90, 57, 145, 67, 108, 199, 153, 58, 67, 65, 82, 145, 67, 108, 9, 151, 59, 67, 216, 100, 145, 67,
			108, 214, 149, 60, 67, 22, 113, 145, 67, 108, 138, 149, 61, 67, 242, 118, 145, 67, 108, 0, 0, 62, 67, 128, 119, 145, 67, 108, 0, 0, 62, 67, 128, 119, 145, 67, 108, 0, 0, 62, 67, 128, 119, 145, 67, 108, 229, 255, 62, 67, 77, 116, 145, 67, 108, 37, 255, 63, 67, 182, 106, 145, 67, 108, 31, 253, 64, 67, 193, 90, 145, 67, 108,
			48, 249, 65, 67, 120, 68, 145, 67, 108, 181, 242, 66, 67, 234, 39, 145, 67, 108, 16, 233, 67, 67, 41, 5, 145, 67, 108, 162, 219, 68, 67, 75, 220, 144, 67, 108, 210, 201, 69, 67, 107, 173, 144, 67, 108, 5, 179, 70, 67, 165, 120, 144, 67, 108, 168, 150, 71, 67, 29, 62, 144, 67, 108, 40, 116, 72, 67, 247, 253, 143, 67, 108,
			248, 74, 73, 67, 92, 184, 143, 67, 108, 141, 26, 74, 67, 122, 109, 143, 67, 108, 100, 226, 74, 67, 127, 29, 143, 67, 108, 253, 161, 75, 67, 160, 200, 142, 67, 108, 220, 88, 76, 67, 18, 111, 142, 67, 108, 142, 6, 77, 67, 15, 17, 142, 67, 108, 161, 170, 77, 67, 211, 174, 141, 67, 108, 175, 68, 78, 67, 156, 72, 141, 67, 108,
			84, 212, 78, 67, 173, 222, 140, 67, 108, 52, 89, 79, 67, 73, 113, 140, 67, 108, 251, 210, 79, 67, 181, 0, 140, 67, 108, 89, 65, 80, 67, 59, 141, 139, 67, 108, 10, 164, 80, 67, 36, 23, 139, 67, 108, 205, 250, 80, 67, 186, 158, 138, 67, 108, 107, 69, 81, 67, 77, 36, 138, 67, 108, 180, 131, 81, 67, 41, 168, 137, 67, 108, 128,
			181, 81, 67, 159, 42, 137, 67, 108, 177, 218, 81, 67, 254, 171, 136, 67, 108, 44, 243, 81, 67, 151, 44, 136, 67, 108, 229, 254, 81, 67, 189, 172, 135, 67, 108, 0, 0, 82, 67, 128, 119, 135, 67, 108, 0, 0, 82, 67, 128, 119, 135, 67, 108, 0, 0, 82, 67, 128, 119, 135, 67, 108, 154, 249, 81, 67, 142, 247, 134, 67, 108, 108, 230,
			81, 67, 237, 119, 134, 67, 108, 130, 198, 81, 67, 240, 248, 133, 67, 108, 241, 153, 81, 67, 232, 122, 133, 67, 108, 213, 96, 81, 67, 38, 254, 132, 67, 108, 83, 27, 81, 67, 248, 130, 132, 67, 108, 151, 201, 80, 67, 175, 9, 132, 67, 108, 213, 107, 80, 67, 151, 146, 131, 67, 108, 74, 2, 80, 67, 253, 29, 131, 67, 108, 57, 141,
			79, 67, 44, 172, 130, 67, 108, 237, 12, 79, 67, 108, 61, 130, 67, 108, 184, 129, 78, 67, 4, 210, 129, 67, 108, 243, 235, 77, 67, 57, 106, 129, 67, 108, 254, 75, 77, 67, 78, 6, 129, 67, 108, 64, 162, 76, 67, 130, 166, 128, 67, 108, 36, 239, 75, 67, 18, 75, 128, 67, 108, 30, 51, 75, 67, 114, 232, 127, 67, 108, 165, 110, 74,
			67, 94, 68, 127, 67, 108, 56, 162, 73, 67, 80, 170, 126, 67, 108, 90, 206, 72, 67, 172, 26, 126, 67, 108, 145, 243, 71, 67, 203, 149, 125, 67, 108, 106, 18, 71, 67, 5, 28, 125, 67, 108, 117, 43, 70, 67, 166, 173, 124, 67, 108, 70, 63, 69, 67, 246, 74, 124, 67, 108, 116, 78, 68, 67, 51, 244, 123, 67, 108, 153, 89, 67, 67,
			149, 169, 123, 67, 108, 81, 97, 66, 67, 76, 107, 123, 67, 108, 60, 102, 65, 67, 127, 57, 123, 67, 108, 250, 104, 64, 67, 79, 20, 123, 67, 108, 45, 106, 63, 67, 212, 251, 122, 67, 108, 121, 106, 62, 67, 28, 240, 122, 67, 108, 0, 0, 62, 67, 0, 239, 122, 67, 108, 0, 0, 62, 67, 0, 239, 122, 67, 99, 109, 128, 219, 69, 67, 192,
			248, 128, 67, 108, 128, 219, 69, 67, 64, 246, 141, 67, 108, 128, 91, 47, 67, 128, 119, 135, 67, 108, 128, 219, 69, 67, 192, 248, 128, 67, 99, 101, 0, 0 };
		path.loadPathFromData(pathData, sizeof(pathData));
		break;
	}
	case BackendCommandTarget::MenuViewForward:
	{
		static const unsigned char pathData[] = { 110, 109, 0, 183, 107, 67, 0, 239, 122, 67, 108, 0, 183, 107, 67, 0, 239, 122, 67, 108, 27, 183, 106, 67, 102, 245, 122, 67, 108, 218, 183, 105, 67, 148, 8, 123, 67, 108, 224, 185, 104, 67, 126, 40, 123, 67, 108, 208, 189, 103, 67, 15, 85, 123, 67, 108, 75, 196, 102, 67, 43, 142, 123, 67, 108, 240, 205, 101, 67, 173, 211,
			123, 67, 108, 93, 219, 100, 67, 105, 37, 124, 67, 108, 46, 237, 99, 67, 43, 131, 124, 67, 108, 250, 3, 99, 67, 182, 236, 124, 67, 108, 87, 32, 98, 67, 199, 97, 125, 67, 108, 215, 66, 97, 67, 19, 226, 125, 67, 108, 8, 108, 96, 67, 72, 109, 126, 67, 108, 114, 156, 95, 67, 13, 3, 127, 67, 108, 155, 212, 94, 67, 2, 163, 127,
			67, 108, 2, 21, 94, 67, 96, 38, 128, 67, 108, 35, 94, 93, 67, 238, 127, 128, 67, 108, 114, 176, 92, 67, 242, 221, 128, 67, 108, 94, 12, 92, 67, 46, 64, 129, 67, 108, 80, 114, 91, 67, 100, 166, 129, 67, 108, 171, 226, 90, 67, 84, 16, 130, 67, 108, 203, 93, 90, 67, 184, 125, 130, 67, 108, 5, 228, 89, 67, 75, 238, 130, 67, 108,
			166, 117, 89, 67, 198, 97, 131, 67, 108, 246, 18, 89, 67, 221, 215, 131, 67, 108, 51, 188, 88, 67, 70, 80, 132, 67, 108, 149, 113, 88, 67, 180, 202, 132, 67, 108, 76, 51, 88, 67, 216, 70, 133, 67, 108, 127, 1, 88, 67, 98, 196, 133, 67, 108, 79, 220, 87, 67, 3, 67, 134, 67, 108, 211, 195, 87, 67, 105, 194, 134, 67, 108, 27,
			184, 87, 67, 68, 66, 135, 67, 108, 0, 183, 87, 67, 128, 119, 135, 67, 108, 0, 183, 87, 67, 128, 119, 135, 67, 108, 0, 183, 87, 67, 128, 119, 135, 67, 108, 102, 189, 87, 67, 114, 247, 135, 67, 108, 148, 208, 87, 67, 19, 119, 136, 67, 108, 126, 240, 87, 67, 16, 246, 136, 67, 108, 15, 29, 88, 67, 24, 116, 137, 67, 108, 43,
			86, 88, 67, 219, 240, 137, 67, 108, 174, 155, 88, 67, 9, 108, 138, 67, 108, 106, 237, 88, 67, 82, 229, 138, 67, 108, 44, 75, 89, 67, 106, 92, 139, 67, 108, 183, 180, 89, 67, 4, 209, 139, 67, 108, 200, 41, 90, 67, 213, 66, 140, 67, 108, 20, 170, 90, 67, 149, 177, 140, 67, 108, 74, 53, 91, 67, 253, 28, 141, 67, 108, 15, 203,
			91, 67, 200, 132, 141, 67, 108, 4, 107, 92, 67, 180, 232, 141, 67, 108, 195, 20, 93, 67, 128, 72, 142, 67, 108, 223, 199, 93, 67, 240, 163, 142, 67, 108, 229, 131, 94, 67, 200, 250, 142, 67, 108, 94, 72, 95, 67, 210, 76, 143, 67, 108, 203, 20, 96, 67, 217, 153, 143, 67, 108, 170, 232, 96, 67, 171, 225, 143, 67, 108, 114,
			195, 97, 67, 27, 36, 144, 67, 108, 153, 164, 98, 67, 254, 96, 144, 67, 108, 142, 139, 99, 67, 46, 152, 144, 67, 108, 189, 119, 100, 67, 134, 201, 144, 67, 108, 143, 104, 101, 67, 231, 244, 144, 67, 108, 107, 93, 102, 67, 54, 26, 145, 67, 108, 178, 85, 103, 67, 90, 57, 145, 67, 108, 199, 80, 104, 67, 65, 82, 145, 67, 108,
			9, 78, 105, 67, 216, 100, 145, 67, 108, 214, 76, 106, 67, 22, 113, 145, 67, 108, 138, 76, 107, 67, 242, 118, 145, 67, 108, 0, 183, 107, 67, 128, 119, 145, 67, 108, 0, 183, 107, 67, 128, 119, 145, 67, 108, 0, 183, 107, 67, 128, 119, 145, 67, 108, 229, 182, 108, 67, 77, 116, 145, 67, 108, 37, 182, 109, 67, 182, 106, 145,
			67, 108, 31, 180, 110, 67, 193, 90, 145, 67, 108, 48, 176, 111, 67, 120, 68, 145, 67, 108, 181, 169, 112, 67, 234, 39, 145, 67, 108, 16, 160, 113, 67, 41, 5, 145, 67, 108, 162, 146, 114, 67, 75, 220, 144, 67, 108, 210, 128, 115, 67, 107, 173, 144, 67, 108, 5, 106, 116, 67, 165, 120, 144, 67, 108, 168, 77, 117, 67, 29,
			62, 144, 67, 108, 40, 43, 118, 67, 247, 253, 143, 67, 108, 248, 1, 119, 67, 92, 184, 143, 67, 108, 141, 209, 119, 67, 122, 109, 143, 67, 108, 100, 153, 120, 67, 127, 29, 143, 67, 108, 253, 88, 121, 67, 160, 200, 142, 67, 108, 220, 15, 122, 67, 18, 111, 142, 67, 108, 142, 189, 122, 67, 15, 17, 142, 67, 108, 161, 97, 123,
			67, 211, 174, 141, 67, 108, 175, 251, 123, 67, 156, 72, 141, 67, 108, 84, 139, 124, 67, 173, 222, 140, 67, 108, 52, 16, 125, 67, 73, 113, 140, 67, 108, 251, 137, 125, 67, 181, 0, 140, 67, 108, 89, 248, 125, 67, 59, 141, 139, 67, 108, 10, 91, 126, 67, 36, 23, 139, 67, 108, 205, 177, 126, 67, 186, 158, 138, 67, 108, 107,
			252, 126, 67, 77, 36, 138, 67, 108, 180, 58, 127, 67, 41, 168, 137, 67, 108, 128, 108, 127, 67, 159, 42, 137, 67, 108, 177, 145, 127, 67, 254, 171, 136, 67, 108, 44, 170, 127, 67, 151, 44, 136, 67, 108, 229, 181, 127, 67, 189, 172, 135, 67, 108, 0, 183, 127, 67, 128, 119, 135, 67, 108, 0, 183, 127, 67, 128, 119, 135,
			67, 108, 0, 183, 127, 67, 128, 119, 135, 67, 108, 154, 176, 127, 67, 142, 247, 134, 67, 108, 108, 157, 127, 67, 237, 119, 134, 67, 108, 130, 125, 127, 67, 240, 248, 133, 67, 108, 241, 80, 127, 67, 232, 122, 133, 67, 108, 213, 23, 127, 67, 38, 254, 132, 67, 108, 83, 210, 126, 67, 248, 130, 132, 67, 108, 151, 128, 126,
			67, 175, 9, 132, 67, 108, 213, 34, 126, 67, 151, 146, 131, 67, 108, 74, 185, 125, 67, 253, 29, 131, 67, 108, 57, 68, 125, 67, 44, 172, 130, 67, 108, 237, 195, 124, 67, 108, 61, 130, 67, 108, 184, 56, 124, 67, 4, 210, 129, 67, 108, 243, 162, 123, 67, 57, 106, 129, 67, 108, 254, 2, 123, 67, 78, 6, 129, 67, 108, 64, 89, 122,
			67, 130, 166, 128, 67, 108, 36, 166, 121, 67, 18, 75, 128, 67, 108, 30, 234, 120, 67, 114, 232, 127, 67, 108, 165, 37, 120, 67, 94, 68, 127, 67, 108, 56, 89, 119, 67, 80, 170, 126, 67, 108, 90, 133, 118, 67, 172, 26, 126, 67, 108, 145, 170, 117, 67, 203, 149, 125, 67, 108, 106, 201, 116, 67, 5, 28, 125, 67, 108, 117, 226,
			115, 67, 166, 173, 124, 67, 108, 70, 246, 114, 67, 246, 74, 124, 67, 108, 116, 5, 114, 67, 51, 244, 123, 67, 108, 153, 16, 113, 67, 149, 169, 123, 67, 108, 81, 24, 112, 67, 76, 107, 123, 67, 108, 60, 29, 111, 67, 127, 57, 123, 67, 108, 250, 31, 110, 67, 79, 20, 123, 67, 108, 45, 33, 109, 67, 212, 251, 122, 67, 108, 121,
			33, 108, 67, 28, 240, 122, 67, 108, 0, 183, 107, 67, 0, 239, 122, 67, 108, 0, 183, 107, 67, 0, 239, 122, 67, 99, 109, 0, 128, 99, 67, 0, 203, 128, 67, 108, 0, 0, 122, 67, 192, 73, 135, 67, 108, 0, 128, 99, 67, 128, 200, 141, 67, 108, 0, 128, 99, 67, 0, 203, 128, 67, 99, 101, 0, 0 };
		path.loadPathFromData(pathData, sizeof(pathData));
		break;
	}

	default: jassertfalse;
	}
	
	DrawablePath *p = new DrawablePath();

    p->setFill(FillType(!isOn ? Colour(BACKEND_ICON_COLOUR_ON) : Colour(BACKEND_ICON_COLOUR_OFF)));
	p->setPath(path);

	return p;	
};