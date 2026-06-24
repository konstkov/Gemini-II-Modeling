#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48,
};

struct ChainSettings
{
    float peakFreq { 0 }, peakGainInDecibels{ 0 }, peakQuality {1.f};
    float lowCutFreq { 0 }, highCutFreq { 0 };
    Slope lowCutSlope { Slope_12 }, highCutSlope { Slope_12 };
    float volume { 0 };
};

ChainSettings getChainSettings (juce::AudioProcessorValueTreeState &apvts);

//==============================================================================

// class Distortion : public juce::AudioProcessor
// {
// public:
//
//     Distortion()
//     {
//
//     }
//
// private:
//
//     juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::WaveShaper<float>,
//     juce::dsp::Gain<float>> processorChain; // [1]
// };

class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:

    //==============================================================================
    // AudioPluginAudioProcessor()
    // {
    //     auto& waveshaper = leftChain.template get<waveshaperIndex>();
    //     waveshaper.functionToUse = [] (const float x) {
    //         return std::tanh (x); // [4]
    //     };
    //     auto& preGain = processorChain.template get<preGainIndex>(); // [5]
    //     preGain.setGainDecibels (30.0f); // [6]
    //     auto& postGain = processorChain.template get<postGainIndex>(); // [7]
    //     postGain.setGainDecibels (-20.0f); // [8]
    // }
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:

    using Filter = juce::dsp::IIR::Filter<float>;

    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    using Coefficients = Filter::CoefficientsPtr;

    MonoChain leftChain, rightChain;

    enum {
        preGainIndex, // [2]
        waveshaperIndex,
        postGainIndex // [3]
    };

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    void updatePeakFilter(const ChainSettings & chainSettings);

    static void updateCoefficients(Coefficients & old, const Coefficients & replacements);

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& leftLowCut,
                     const CoefficientType& cutCoefficients,
                     const Slope& lowCutSlope)
    {
        leftLowCut.template setBypassed<0>(true);
        leftLowCut.template setBypassed<1>(true);
        leftLowCut.template setBypassed<2>(true);
        leftLowCut.template setBypassed<3>(true);

        switch (lowCutSlope)
        {
            case Slope_48:
            {
                update<3>(leftLowCut, cutCoefficients);
            }
            case Slope_36:
            {
                update<2>(leftLowCut, cutCoefficients);
            }
            case Slope_24:
            {
                update<1>(leftLowCut, cutCoefficients);
            }
            case Slope_12:
            {
                update<0>(leftLowCut, cutCoefficients);
            }
        }
    }

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor);
};
