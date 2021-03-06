
#include "JuceHeader.h"


#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4127 4706 4100)
#endif

#include "synthesisers/synths/PolyBlep.cpp"

namespace wdl
{

#include "effects/convolution/wdl/convoengine.cpp"
#include "effects/convolution/wdl/fft.c"

}

#if JUCE_MSVC
#pragma warning (pop)
#endif

#include "effects/fx/chunkware_simple_dynamics/chunkware_simple_dynamics.cpp"





/** @defgroup modulatorTypes ModulatorTypes =========================================================================
*	@ingroup modulator
*
*	Here are all actual Modulators that can be used.
*/

#include "modulators/mods/ConstantModulator.cpp"
#include "modulators/mods/ControlModulator.cpp"
#include "modulators/mods/LFOModulator.cpp"
#include "modulators/mods/AudioFileEnvelope.cpp"
#include "modulators/mods/MacroControlModulator.cpp"
#include "modulators/mods/PluginParameterModulator.cpp"
#include "modulators/mods/RandomModulator.cpp"
#include "modulators/mods/SimpleEnvelope.cpp"
#include "modulators/mods/KeyModulator.cpp"
#include "modulators/mods/AhdsrEnvelope.cpp"
#include "modulators/mods/PitchWheelModulator.cpp"
#include "modulators/mods/TableEnvelope.cpp"
#include "modulators/mods/VelocityModulator.cpp"
#include "modulators/mods/ArrayModulator.cpp"
#include "modulators/mods/GlobalModulators.cpp"
#include "modulators/mods/CCEnvelope.cpp"
#include "modulators/mods/CCDucker.cpp"
#include "modulators/mods/GainMatcher.cpp"

#if USE_BACKEND

#include "modulators/editors/AhdsrEnvelopeEditor.cpp"
#include "modulators/editors/ConstantEditor.cpp"
#include "modulators/editors/ControlEditor.cpp"
#include "modulators/editors/CCDuckerEditor.cpp"
#include "modulators/editors/CCEnvelopeEditor.cpp"
#include "modulators/editors/LfoEditor.cpp"
#include "modulators/editors/AudioFileEnvelopeEditor.cpp"
#include "modulators/editors/KeyEditor.cpp"
#include "modulators/editors/MacroControlModulatorEditor.cpp"
#include "modulators/editors/PitchWheelEditor.cpp"
#include "modulators/editors/PluginParameterEditor.cpp"
#include "modulators/editors/RandomEditor.cpp"
#include "modulators/editors/SimpleEnvelopeEditor.cpp"
#include "modulators/editors/TableEnvelopeEditor.cpp"
#include "modulators/editors/VelocityEditor.cpp"
#include "modulators/editors/ArrayModulatorEditor.cpp"
#include "modulators/editors/GlobalModulatorEditor.cpp"
#include "modulators/editors/GainMatcherEditor.cpp"

#endif

/** @defgroup midiTypes MidiProcessor Types =======================================================================
*	@ingroup midiProcessor
*
*	All actual MidiProcessors that can be used.
*	There are almost none, because everything can also be achieved using scripts (or hardcoded scripts)
*/

#include "midi_processor/mps/MidiDelay.cpp"
#include "midi_processor/mps/Transposer.cpp"
#include "midi_processor/mps/SampleRaster.cpp"
#include "midi_processor/mps/RoundRobin.cpp"

#if USE_BACKEND

#include "midi_processor/editors/TransposerEditor.cpp"

#endif

/** @defgroup effectTypes Effect Types ===========================================================================
*	@ingroup dsp
*
*	Contains all audio effect classes
*/

#include "effects/MdaEffectWrapper.cpp"

#include "effects/fx/RouteFX.cpp"
#include "effects/fx/Filters.cpp"
#include "effects/fx/HarmonicFilter.cpp"
#include "effects/fx/CurveEq.cpp"
#include "effects/fx/StereoFX.cpp"
#include "effects/fx/SimpleReverb.cpp"
#include "effects/fx/Delay.cpp"
#include "effects/fx/GainEffect.cpp"
#include "effects/fx/Chorus.cpp"
#include "effects/fx/Phaser.cpp"
#include "effects/fx/GainCollector.cpp"
#include "effects/convolution/AtkConvolution.cpp"
#include "effects/convolution/Convolution.cpp"
#include "effects/mda/mdaLimiter.cpp"
#include "effects/mda/mdaDegrade.cpp"
#include "effects/fx/Dynamics.cpp"
#include "effects/fx/Saturator.cpp"
#include "effects/fx/AudioProcessorWrapper.cpp"
#include "effects/fx/SlotFX.cpp"
#include "effects/fx/Analyser.cpp"
#include "effects/fx/WaveShapers.cpp"
#include "effects/fx/ShapeFX.cpp"

#if USE_BACKEND

#include "effects/editors/FilterEditor.cpp"
#include "effects/editors/HarmonicFilterEditor.cpp"
#include "effects/editors/CurveEqEditor.cpp"
#include "effects/editors/StereoEditor.cpp"
#include "effects/editors/ReverbEditor.cpp"
#include "effects/editors/DelayEditor.cpp"
#include "effects/editors/GainEditor.cpp"
#include "effects/editors/ChorusEditor.cpp"
#include "effects/editors/PhaserEditor.cpp"
#include "effects/editors/ConvolutionEditor.cpp"
#include "effects/editors/MdaLimiterEditor.cpp"
#include "effects/editors/MdaDegradeEditor.cpp"
#include "effects/editors/GainCollectorEditor.cpp"
#include "effects/editors/RouteFXEditor.cpp"
#include "effects/editors/SaturationEditor.cpp"
#include "effects/editors/DynamicsEditor.cpp"

#include "effects/editors/SlotFXEditor.cpp"
#include "effects/editors/AnalyserEditor.cpp"
#include "effects/editors/ShapeFXEditor.cpp"
#include "effects/editors/PolyShapeFXEditor.cpp"

#endif

#include "effects/fx/WrappedAudioProcessors/WrappedAudioProcessors.cpp"

#include "effects/editors/AudioProcessorEditorWrapper.cpp"

/** @defgroup synthTypes Synth Types ===========================================================================
*	@ingroup dsp
*
*	Contains all synth classes
*/

#include "synthesisers/synths/GlobalModulatorContainer.cpp"
#include "synthesisers/synths/SineSynth.cpp"
#include "synthesisers/synths/NoiseSynth.cpp"
#include "synthesisers/synths/WaveSynth.cpp"
#include "synthesisers/synths/WavetableTools.cpp"
#include "synthesisers/editors/WavetableComponents.cpp"
#include "synthesisers/synths/WavetableSynth.cpp"
#include "synthesisers/synths/AudioLooper.cpp"

#if USE_BACKEND

#include "synthesisers/editors/SineSynthBody.cpp"
#include "synthesisers/editors/WaveSynthBody.cpp"
#include "synthesisers/editors/GroupBody.cpp"
#include "synthesisers/editors/ModulatorSynthChainBody.cpp"
#include "synthesisers/editors/WavetableBody.cpp"
#include "synthesisers/editors/AudioLooperEditor.cpp"

#endif

