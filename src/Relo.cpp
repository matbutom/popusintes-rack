#include "Popusintes.hpp"
#include "Tornillos.hpp"

// definir tiempo de trigger
#define TIEMPO_TRIGGER 1e-3f

// definir ancho modulo en mm (5 hp)
#define MODULO_ANCHO 25.4f

// definir altura modulo en mm (3 u)
#define MODULO_ALTURA 128.4f

// define porcentajes columnas
#define PORCENTAJE_COLUMNA_IZQ 0.30f
#define PORCENTAJE_COLUMNA_DER 0.70f

// define porcentaje delta en eje y
// entre boton y entrada
#define DELTA_Y_BOTON_ENTRADA 0.08f

// define porcentaje delta en eye y
// entre perilla y atenuversor y entrada
#define DELTA_Y_PERILLA_ATENUVERSOR 0.08f

// define porcentaje entre salida y su luz
#define DELTA_Y_SALIDA_LUZ 0.05f

// define coordenadas ppm
#define PORCENTAJE_PPM_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_PPM_Y 0.15f

// define coordenadas resinc
#define PORCENTAJE_BOTON_RESINC_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_BOTON_RESINC_Y 0.35f
#define PORCENTAJE_ENTRADA_RESINC_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_ENTRADA_RESINC_Y (PORCENTAJE_BOTON_RESINC_Y + DELTA_Y_BOTON_ENTRADA)

// definir coordenadas desfase
#define PORCENTAJE_DESFASE_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_DESFASE_B_Y 0.60f
#define PORCENTAJE_DESFASE_CV_ATEN_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_DESFASE_CV_ATEN_Y (PORCENTAJE_DESFASE_B_Y + DELTA_Y_PERILLA_ATENUVERSOR)
#define PORCENTAJE_ENTRADA_DESFASE_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_ENTRADA_DESFASE_B_Y (PORCENTAJE_DESFASE_B_Y + 2.f * DELTA_Y_PERILLA_ATENUVERSOR)

// definir coordenadas salida y luz a
#define PORCENTAJE_LUCES_A_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_LUCES_A_Y 0.85f
#define PORCENTAJE_SALIDA_A_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_SALIDA_A_Y (PORCENTAJE_LUCES_A_Y + DELTA_Y_SALIDA_LUZ)

// definir coordenadas salida y luz b
#define PORCENTAJE_LUCES_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_LUCES_B_Y 0.85f
#define PORCENTAJE_SALIDA_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_SALIDA_B_Y (PORCENTAJE_LUCES_B_Y + DELTA_Y_SALIDA_LUZ)

// modulo
struct ReloModule : Module
{
    enum ParamIds
    {
        // pulsos por minuto
        PARAM_PPM,
        // DESFASE de B en % de PARAM_PPM
        PARAM_DESFASE_B,
        // atenuversor para la entrada CV de desfase
        PARAM_DESFASE_CV_ATEN,
        // boton que resincroniza A y B
        PARAM_BOTON_RESINC,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_RESINC,
        // CV que se suma a PARAM_DESFASE_B (bipolar +-5V)
        ENTRADA_DESFASE_B,
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

        configParam(PARAM_DESFASE_B, -10.0, 10.0, 0.0, "desfase canal b", " %");
        configParam(PARAM_DESFASE_CV_ATEN, -1.0, 1.0, 0.0, "cv desfase atenuversor");
        configParam(PARAM_BOTON_RESINC, 0.0, 1.0, 0.0, "resincronizar");

        configInput(ENTRADA_RESINC, "resincronizar");
        configInput(ENTRADA_DESFASE_B, "cv desfase b");

        configOutput(SALIDA_PULSO_A, "pulso a");
        configOutput(SALIDA_PULSO_B, "pulso b");

        contadorA = 0.f;
        periodoA = 0.f;

        contadorB = 0.f;
        periodoB = 0.f;
    }

    void process(const ProcessArgs &args) override
    {
        // guardar en variables float los valor de la perillas
        float ppmA = params[PARAM_PPM].getValue();

        // desfase = perilla + cv atenuado
        // cv bipolar entre -5V y +5V
        // con atenuversor en 1.0 cubre el rango completo

        float desfase = params[PARAM_DESFASE_B].getValue();
        desfase = desfase + inputs[ENTRADA_DESFASE_B].getVoltage() * params[PARAM_DESFASE_CV_ATEN].getValue() * 2.f;

        // atajar desfase en rango de -10.0f a +10.f

        if (desfase > 10.f)
        {
            desfase = 10.f;
        }
        else if (desfase < -10.f)
        {
            desfase = -10.f;
        }

        float ppmB = ppmA * (1.f + desfase / 100.f);

        // periodo calculado en samples
        periodoA = 60.f * args.sampleRate / ppmA;
        periodoB = 60.f * args.sampleRate / ppmB;

        // resincronizar boton del panel o externo
        bool resetBoton = botonResinc.process(params[PARAM_BOTON_RESINC].getValue());
        bool resetExterno = entradaResinc.process(inputs[ENTRADA_RESINC].getVoltage());
        bool reset = resetBoton || resetExterno;

        if (reset)
        {
            contadorA = 0.f;
            contadorB = 0.f;

            // ademas de reiniciar los contadores
            // emitimuos un puslo en A y B
            // el resincronizado se escucha en ese instante
            generadorPulsosA.trigger(TIEMPO_TRIGGER);
            generadorPulsosB.trigger(TIEMPO_TRIGGER);
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
        agregarTornillos(this);

        // params
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(
                                                         MODULO_ANCHO * PORCENTAJE_PPM_X,
                                                         MODULO_ALTURA * PORCENTAJE_PPM_Y)),
                                                     module, ReloModule::PARAM_PPM));

        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(
                                                              MODULO_ANCHO * PORCENTAJE_DESFASE_B_X,
                                                              MODULO_ALTURA * PORCENTAJE_DESFASE_B_Y)),
                                                          module, ReloModule::PARAM_DESFASE_B));

        // atenuverter chico para la entrada CV de desfase
        addParam(createParamCentered<Trimpot>(mm2px(Vec(
                                                  MODULO_ANCHO * PORCENTAJE_DESFASE_CV_ATEN_X,
                                                  MODULO_ALTURA * PORCENTAJE_DESFASE_CV_ATEN_Y)),
                                              module, ReloModule::PARAM_DESFASE_CV_ATEN));

        addParam(createParamCentered<TL1105>(mm2px(Vec(
                                                 MODULO_ANCHO * PORCENTAJE_BOTON_RESINC_X,
                                                 MODULO_ALTURA * PORCENTAJE_BOTON_RESINC_Y)),
                                             module, ReloModule::PARAM_BOTON_RESINC));

        // entradas
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(
                                                     MODULO_ANCHO * PORCENTAJE_ENTRADA_RESINC_X,
                                                     MODULO_ALTURA * PORCENTAJE_ENTRADA_RESINC_Y)),
                                                 module, ReloModule::ENTRADA_RESINC));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(
                                                     MODULO_ANCHO * PORCENTAJE_ENTRADA_DESFASE_B_X,
                                                     MODULO_ALTURA * PORCENTAJE_ENTRADA_DESFASE_B_Y)),
                                                 module, ReloModule::ENTRADA_DESFASE_B));

        //  salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(
                                                       MODULO_ANCHO * PORCENTAJE_SALIDA_A_X,
                                                       MODULO_ALTURA * PORCENTAJE_SALIDA_A_Y)),
                                                   module, ReloModule::SALIDA_PULSO_A));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(
                                                       MODULO_ANCHO * PORCENTAJE_SALIDA_B_X,
                                                       MODULO_ALTURA * PORCENTAJE_SALIDA_B_Y)),
                                                   module, ReloModule::SALIDA_PULSO_B));

        // luces
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(
                                                                  MODULO_ANCHO * PORCENTAJE_LUCES_A_X,
                                                                  MODULO_ALTURA * PORCENTAJE_LUCES_A_Y)),
                                                              module, ReloModule::LUZ_PULSO_A));
        // luces
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(
                                                                  MODULO_ANCHO * PORCENTAJE_LUCES_B_X,
                                                                  MODULO_ALTURA * PORCENTAJE_LUCES_B_Y)),
                                                              module, ReloModule::LUZ_PULSO_B));
    }
};

Model *modelRelo = createModel<ReloModule, ReloModuleWidget>("relo");