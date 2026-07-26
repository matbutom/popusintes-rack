#include "Popusintes.hpp"

// modulo
struct SumaModule : Module
{
    enum ParamIds
    {
        NUM_PARAMS,
    };

    enum InputIds
    {
        NUM_INPUTS,
    };

    enum OutputIds
    {
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        NUM_LIGHTS,
    };

    SumaModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct SumaModuleWidget : ModuleWidget
{
    SumaModuleWidget(SumaModule *module)
    {

        setModule(module);
        setSuma(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SumaModule.svg")));

        // tornillos
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    }
};

Model *modelSuma = createModel<SumaModule, SumaModuleWidget>("suma");