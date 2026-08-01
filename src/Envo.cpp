#include "Popusintes.hpp"
#include "Tiempos.hpp"
#include "Tornillos.hpp"

// modulo
struct EnvoModule : Module
{
    enum ParamIds
    {
        PARAM_SUBIDA,
        PARAM_BAJADA,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_PULSO_A,
        ENTRADA_PULSO_A_FORZAR,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_ENVO_A,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_ENVO_A,
        NUM_LIGHTS,
    };

    enum EstadoEnvo
    {
        ESTADO_REPOSO,
        ESTADO_SUBIDA,
        ESTADO_BAJADA,
        ESTADO_FORZAR
    };

    EstadoEnvo estado = ESTADO_REPOSO;
    float salida = 0.0f;
    dsp::SchmittTrigger detectorPulso;
    dsp::SchmittTrigger detectorForzar;

    EnvoModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(
            PARAM_SUBIDA, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "subida", " s");
        configParam(
            PARAM_BAJADA, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "bajada"
                  " s");

        configInput(ENTRADA_PULSO_A, "pulso");
        configInput(ENTRADA_PULSO_A_FORZAR, "forzar");
        configOutput(SALIDA_ENVO_A, "envolvente");
    }

    void process(const ProcessArgs &args) override
    {
        // detectar subida de pulso (umbral de 1V)
        bool detectado = detectorPulso.process(inputs[ENTRADA_PULSO_A].getVoltage(), 0.1f, 1.0f);

        // pulso normal solamente ocurre si esatado actual es reposo
        if (detectado && estado == ESTADO_REPOSO)
        {
            estado = ESTADO_SUBIDA;
        }

        // detectar forzardo
        bool forzadoDetectado = detectorForzar.process(
            inputs[ENTRADA_PULSO_A_FORZAR].getVoltage(), 0.1f, 1.0f);

        if (forzadoDetectado)
        {
            estado = ESTADO_FORZAR;
        }

        float tiempoSubida = params[PARAM_SUBIDA].getValue();
        float tiempoBajada = params[PARAM_BAJADA].getValue();

        switch (estado)
        {
        case ESTADO_SUBIDA:
        {
            // incremento para llegar a 10V en tiempoSubida
            float incremento = (10.0f / tiempoSubida) * args.sampleTime;
            salida = salida + incremento;

            // detectar si ya subimos, para empezar la bajada
            if (salida >= 10.0f)
            {
                salida = 10.0f;
                estado = ESTADO_BAJADA;
            }
            break;
        }

        case ESTADO_BAJADA:
        {
            // dencremento para llegar a 0V en tiempoBajada
            float decremento = (10.0f / tiempoBajada) * args.sampleTime;
            salida = salida - decremento;

            if (salida <= 0.0f)
            {
                salida = 0.0f;
                estado = ESTADO_REPOSO;
            }
            break;
        }

        case ESTADO_FORZAR:
        {
            // rampa rapida a 0V, luego subida
            float decrementoRapido = (10.0f - tiempos::PULSO_MS) * args.sampleTime;
            salida = salida - decrementoRapido;

            if (salida <= 0.0f)
            {
                salida = 0.0f;
                estado = ESTADO_SUBIDA;
            }
            break;
        }

        case ESTADO_REPOSO:
        default:
            break;
        }

        // emitir voltaje de salida
        outputs[SALIDA_ENVO_A].setVoltage(salida);
        lights[LUZ_ENVO_A].setBrightness(salida * 0.1f);
    }
};

// widget
struct EnvoModuleWidget : ModuleWidget
{
    EnvoModuleWidget(EnvoModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EnvoModule.svg")));

        // tornillos
        agregarTornillos(this);

        // parametros
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.35, 30)), module, EnvoModule::PARAM_SUBIDA));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.05, 30)), module, EnvoModule::PARAM_BAJADA));

        // entradas
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(6.35, 60)), module,
            EnvoModule::ENTRADA_PULSO_A));
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(19.05, 60)), module,
            EnvoModule::ENTRADA_PULSO_A_FORZAR));

        // salida
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 100)), module, EnvoModule::SALIDA_ENVO_A));

        // luz
        addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(12.7, 90)), module, EnvoModule::LUZ_ENVO_A));
    }
};

Model *modelEnvo = createModel<EnvoModule, EnvoModuleWidget>("envo");