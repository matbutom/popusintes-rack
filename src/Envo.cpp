#include "Popusintes.hpp"

// modulo
struct EnvoModule : Module
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

    EnvoModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct EnvoModuleWidget : ModuleWidget
{
    EnvoModuleWidget(EnvoModule *module)
    {

        setModule(module);
        setEnvo(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EnvoModule.svg")));

        // tornillos
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    }
};

Model *modelEnvo = createModel<EnvoModule, EnvoModuleWidget>("envo");