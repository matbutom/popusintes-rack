#include "Popusintes.hpp"

// definir tiempo de trigger
#define TIEMPO_TRIGGER 1e-3f

// modulo
struct ReloModule : Module
{
    enum ParamIds
    {
        // pulsos por minuto
        PARAM_PPM,
        // DESFASE de B en % de PARAM_PPM
        PARAM_DESFASE_B,
        // resincroniza A y B
        PARAM_RESINC,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_RESINC,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_PULSO_A,
        SALIDA_PULSO_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_PULSO_A,
        LUZ_PULSO_B,
        NUM_LIGHTS,
    };

    // clase auxiliar provista por rack
    dsp::PulseGenerator generadorPulsosA;
    dsp::PulseGenerator generadorPulsosB;
    dsp::SchmittTrigger botonResinc;
    dsp::SchmittTrigger entradaResinc;

    // variables de apoyo
    float contadorA, periodoA;
    float contadorB, periodoB;

    ReloModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // los numeros son valor minimo, maximo e inicial
        // los strings son nombre del parametro y su unidad, ppm empieza con espacio
        configParam(PARAM_PPM, 30.0, 360.0, 120.0, "tempo", " ppm");

        configParam(PARAM_DESFASE_B, -10.0, 10.0, 0.0, "desfase canal B", " %");
        configParam(PARAM_RESINC, 0.0, 1.0, 0.0, "resincronizar");

        contadorA = 0.f;
        periodoA = 0.f;

        contadorB = 0.f;
        periodoB = 0.f;
    }

    void process(const ProcessArgs &args) override
    {
        // guardar en variables float los valor de la perillas
        float ppmA = params[PARAM_PPM].getValue();
        float desfase = params[PARAM_DESFASE_B].getValue();
        float ppmB = ppmA * (1.f + desfase / 100.f);

        // periodo calculado en samples
        periodoA = 60.f * args.sampleRate / ppmA;
        periodoB = 60.f * args.sampleRate / ppmB;

        // resincronizar boton del panel o externo
        bool resetBoton = botonResinc.process(params[PARAM_RESINC].getValue());
        bool resetExterno = entradaResinc.process(inputs[ENTRADA_RESINC].getVoltage());
        bool reset = resetBoton || resetExterno;

        if (reset)
        {
            contadorA = 0.f;
            contadorB = 0.f;
        }

        // canal A
        if (contadorA > periodoA)
        {
            // TIEMPO_TRIGGER esta definida arriba
            // todavia no me gusta el nombre trigger
            // gate le puedo decir compuerta
            // pero trigger aun no se
            generadorPulsosA.trigger(TIEMPO_TRIGGER);
            contadorA = contadorA - periodoA;
        }

        contadorA = contadorA + 1;

        // canal B
        if (contadorB > periodoB)
        {
            generadorPulsosB.trigger(TIEMPO_TRIGGER);
            contadorB = contadorB - periodoB;
        }

        contadorB = contadorB + 1;

        float salidaA = generadorPulsosA.process(args.sampleTime);
        float salidaB = generadorPulsosB.process(args.sampleTime);

        outputs[SALIDA_PULSO_A].setVoltage(10.f * salidaA);
        lights[LUZ_PULSO_A].setSmoothBrightness(salidaA, 5e-6f);

        outputs[SALIDA_PULSO_B].setVoltage(10.f * salidaB);
        lights[LUZ_PULSO_B].setSmoothBrightness(salidaB, 5e-6f);
    }
};

// widget
struct ReloModuleWidget : ModuleWidget
{
    ReloModuleWidget(ReloModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ReloModule.svg")));

        // tornillos
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // params
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.35)), module, ReloModule::PARAM_PPM));
        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.50)), module, ReloModule::PARAM_DESFASE_B));
        addParam(createParamCentered<TL1105>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.55)), module, ReloModule::PARAM_RESINC));

        // entradas
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.60)), module, ReloModule::ENTRADA_RESINC));

        //  salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.3, 128.4 * 0.80)), module, ReloModule::SALIDA_PULSO_A));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.7, 128.4 * 0.80)), module, ReloModule::SALIDA_PULSO_B));

        // luces
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.3, 128.4 * 0.70)), module, ReloModule::LUZ_PULSO_A));
        // luces
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.7, 128.4 * 0.70)), module, ReloModule::LUZ_PULSO_B));
    }
};

Model *modelRelo = createModel<ReloModule, ReloModuleWidget>("relo");