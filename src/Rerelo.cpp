#include "Popusintes.hpp"

// define porcentajes de las cuatro columnas, una por canal
#define PORCENTAJE_COLUMNA_A 0.125f
#define PORCENTAJE_COLUMNA_B 0.375f
#define PORCENTAJE_COLUMNA_C 0.625f
#define PORCENTAJE_COLUMNA_D 0.875f

// define coordenadas ppm, control compartido por los cuatro canales
// se mantiene en columna a, arriba de todo
#define PORCENTAJE_PPM_X (PORCENTAJE_COLUMNA_A)
#define PORCENTAJE_PPM_Y 0.12f

// define coordenadas resinc
// a proposito NO va centrado: queda apilado bajo el ppm, en la misma columna a
// esto imita el layout original de relo, donde resinc vivia debajo del tempo
#define PORCENTAJE_BOTON_RESINC_X (PORCENTAJE_COLUMNA_A)
#define PORCENTAJE_BOTON_RESINC_Y 0.30f
#define PORCENTAJE_ENTRADA_RESINC_X (PORCENTAJE_COLUMNA_A)
#define PORCENTAJE_ENTRADA_RESINC_Y (PORCENTAJE_BOTON_RESINC_Y + espaciado::DELTA_Y_BOTON_ENTRADA)

// definir coordenadas desfase canal b
// arranca mas arriba que c y d, a proposito, para el efecto escalera
#define PORCENTAJE_DESFASE_B_X (PORCENTAJE_COLUMNA_B)
#define PORCENTAJE_DESFASE_B_Y 0.56f
#define PORCENTAJE_DESFASE_CV_ATEN_B_X (PORCENTAJE_COLUMNA_B)
#define PORCENTAJE_DESFASE_CV_ATEN_B_Y (PORCENTAJE_DESFASE_B_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR)
#define PORCENTAJE_ENTRADA_DESFASE_B_X (PORCENTAJE_COLUMNA_B)
#define PORCENTAJE_ENTRADA_DESFASE_B_Y (PORCENTAJE_DESFASE_B_Y + 2.f * espaciado::DELTA_Y_PERILLA_ATENUVERSOR)

// definir coordenadas desfase canal c
// arranca mas abajo que b, siguiendo la diagonal
#define PORCENTAJE_DESFASE_C_X (PORCENTAJE_COLUMNA_C)
#define PORCENTAJE_DESFASE_C_Y 0.42f
#define PORCENTAJE_DESFASE_CV_ATEN_C_X (PORCENTAJE_COLUMNA_C)
#define PORCENTAJE_DESFASE_CV_ATEN_C_Y (PORCENTAJE_DESFASE_C_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR)
#define PORCENTAJE_ENTRADA_DESFASE_C_X (PORCENTAJE_COLUMNA_C)
#define PORCENTAJE_ENTRADA_DESFASE_C_Y (PORCENTAJE_DESFASE_C_Y + 2.f * espaciado::DELTA_Y_PERILLA_ATENUVERSOR)

// definir coordenadas desfase canal d
// la mas abajo de las tres, cierra la diagonal
#define PORCENTAJE_DESFASE_D_X (PORCENTAJE_COLUMNA_D)
#define PORCENTAJE_DESFASE_D_Y 0.28f
#define PORCENTAJE_DESFASE_CV_ATEN_D_X (PORCENTAJE_COLUMNA_D)
#define PORCENTAJE_DESFASE_CV_ATEN_D_Y (PORCENTAJE_DESFASE_D_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR)
#define PORCENTAJE_ENTRADA_DESFASE_D_X (PORCENTAJE_COLUMNA_D)
#define PORCENTAJE_ENTRADA_DESFASE_D_Y (PORCENTAJE_DESFASE_D_Y + 2.f * espaciado::DELTA_Y_PERILLA_ATENUVERSOR)

// definir coordenadas salida y luz, una fila abajo para los cuatro canales
// sin cambios: se mantienen fijas como pediste
#define PORCENTAJE_LUCES_Y 0.85f
#define PORCENTAJE_SALIDA_Y (PORCENTAJE_LUCES_Y + espaciado::DELTA_Y_SALIDA_LUZ)

#define PORCENTAJE_LUCES_A_X (PORCENTAJE_COLUMNA_A)
#define PORCENTAJE_SALIDA_A_X (PORCENTAJE_COLUMNA_A)

#define PORCENTAJE_LUCES_B_X (PORCENTAJE_COLUMNA_B)
#define PORCENTAJE_SALIDA_B_X (PORCENTAJE_COLUMNA_B)

#define PORCENTAJE_LUCES_C_X (PORCENTAJE_COLUMNA_C)
#define PORCENTAJE_SALIDA_C_X (PORCENTAJE_COLUMNA_C)

#define PORCENTAJE_LUCES_D_X (PORCENTAJE_COLUMNA_D)
#define PORCENTAJE_SALIDA_D_X (PORCENTAJE_COLUMNA_D)

// modulo
struct RereloModulo : Module
{
    enum ParamIds
    {
        // pulsos por minuto, tempo base compartido por los cuatro canales
        PARAM_PPM,

        // canal a es la referencia, siempre al tempo base, sin desfase

        // desfase de b en % de PARAM_PPM
        PARAM_DESFASE_B,
        // atenuversor para la entrada CV de desfase b
        PARAM_DESFASE_CV_ATEN_B,

        // desfase de c en % de PARAM_PPM
        PARAM_DESFASE_C,
        // atenuversor para la entrada CV de desfase c
        PARAM_DESFASE_CV_ATEN_C,

        // desfase de d en % de PARAM_PPM
        PARAM_DESFASE_D,
        // atenuversor para la entrada CV de desfase d
        PARAM_DESFASE_CV_ATEN_D,

        // boton que resincroniza los cuatro canales
        PARAM_BOTON_RESINC,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_RESINC,
        // CV que se suma a cada PARAM_DESFASE (bipolar +-5V)
        ENTRADA_DESFASE_B,
        ENTRADA_DESFASE_C,
        ENTRADA_DESFASE_D,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_PULSO_A,
        SALIDA_PULSO_B,
        SALIDA_PULSO_C,
        SALIDA_PULSO_D,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_PULSO_A,
        LUZ_PULSO_B,
        LUZ_PULSO_C,
        LUZ_PULSO_D,
        NUM_LIGHTS,
    };

    // un CanalRelo por canal: agrupa generador de pulsos, contador y periodo
    CanalRelo canalA;
    CanalRelo canalB;
    CanalRelo canalC;
    CanalRelo canalD;

    dsp::SchmittTrigger botonResinc;
    dsp::SchmittTrigger entradaResinc;

    RereloModulo()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // los numeros son valor minimo, maximo e inicial
        // los strings son nombre del parametro y su unidad, ppm empieza con espacio
        configParam(PARAM_PPM, 30.0, 360.0, 120.0, "tempo", " ppm");

        configParam(PARAM_DESFASE_B, -10.0, 10.0, 0.0, "desfase canal b", " %");
        configParam(PARAM_DESFASE_CV_ATEN_B, -1.0, 1.0, 0.0, "cv desfase b atenuversor");

        configParam(PARAM_DESFASE_C, -10.0, 10.0, 0.0, "desfase canal c", " %");
        configParam(PARAM_DESFASE_CV_ATEN_C, -1.0, 1.0, 0.0, "cv desfase c atenuversor");

        configParam(PARAM_DESFASE_D, -10.0, 10.0, 0.0, "desfase canal d", " %");
        configParam(PARAM_DESFASE_CV_ATEN_D, -1.0, 1.0, 0.0, "cv desfase d atenuversor");

        configParam(PARAM_BOTON_RESINC, 0.0, 1.0, 0.0, "resincronizar");

        configInput(ENTRADA_RESINC, "resincronizar");
        configInput(ENTRADA_DESFASE_B, "cv desfase b");
        configInput(ENTRADA_DESFASE_C, "cv desfase c");
        configInput(ENTRADA_DESFASE_D, "cv desfase d");

        configOutput(SALIDA_PULSO_A, "pulso a");
        configOutput(SALIDA_PULSO_B, "pulso b");
        configOutput(SALIDA_PULSO_C, "pulso c");
        configOutput(SALIDA_PULSO_D, "pulso d");
    }

    // calcula el ppm de un canal a partir del ppm base y sus controles de desfase
    // agrupado en una funcion para no repetir la misma cuenta tres veces
    float calcularPpmCanal(float ppmBase, float desfasePerilla, float desfaseCvAten, float desfaseCvVoltaje)
    {
        float desfase = desfasePerilla + desfaseCvVoltaje * desfaseCvAten * 2.f;

        // atajar desfase en rango de -10.0f a +10.f
        if (desfase > 10.f)
        {
            desfase = 10.f;
        }
        else if (desfase < -10.f)
        {
            desfase = -10.f;
        }

        return ppmBase * (1.f + desfase / 100.f);
    }

    void process(const ProcessArgs &args) override
    {
        // guardar en variable float el valor de la perilla de tempo base
        float ppmA = params[PARAM_PPM].getValue();

        // desfase de cada canal = perilla + cv atenuado
        // cv bipolar entre -5V y +5V, con atenuversor en 1.0 cubre el rango completo
        float ppmB = calcularPpmCanal(ppmA, params[PARAM_DESFASE_B].getValue(),
                                      params[PARAM_DESFASE_CV_ATEN_B].getValue(),
                                      inputs[ENTRADA_DESFASE_B].getVoltage());

        float ppmC = calcularPpmCanal(ppmA, params[PARAM_DESFASE_C].getValue(),
                                      params[PARAM_DESFASE_CV_ATEN_C].getValue(),
                                      inputs[ENTRADA_DESFASE_C].getVoltage());

        float ppmD = calcularPpmCanal(ppmA, params[PARAM_DESFASE_D].getValue(),
                                      params[PARAM_DESFASE_CV_ATEN_D].getValue(),
                                      inputs[ENTRADA_DESFASE_D].getVoltage());

        // resincronizar boton del panel o externo
        bool resetBoton = botonResinc.process(params[PARAM_BOTON_RESINC].getValue());
        bool resetExterno = entradaResinc.process(inputs[ENTRADA_RESINC].getVoltage());
        bool reset = resetBoton || resetExterno;

        if (reset)
        {
            canalA.reiniciar();
            canalB.reiniciar();
            canalC.reiniciar();
            canalD.reiniciar();

            // ademas de reiniciar los contadores
            // emitimos un pulso en los cuatro canales
            // el resincronizado se escucha en ese instante
            canalA.generarPulso(tiempos::PULSO_MS);
            canalB.generarPulso(tiempos::PULSO_MS);
            canalC.generarPulso(tiempos::PULSO_MS);
            canalD.generarPulso(tiempos::PULSO_MS);
        }

        // canal a, es la referencia, nunca se desfasa
        float salidaA = canalA.procesar(args.sampleTime, args.sampleRate, ppmA, tiempos::PULSO_MS);
        float salidaB = canalB.procesar(args.sampleTime, args.sampleRate, ppmB, tiempos::PULSO_MS);
        float salidaC = canalC.procesar(args.sampleTime, args.sampleRate, ppmC, tiempos::PULSO_MS);
        float salidaD = canalD.procesar(args.sampleTime, args.sampleRate, ppmD, tiempos::PULSO_MS);

        outputs[SALIDA_PULSO_A].setVoltage(10.f * salidaA);
        lights[LUZ_PULSO_A].setSmoothBrightness(salidaA, 5e-6f);

        outputs[SALIDA_PULSO_B].setVoltage(10.f * salidaB);
        lights[LUZ_PULSO_B].setSmoothBrightness(salidaB, 5e-6f);

        outputs[SALIDA_PULSO_C].setVoltage(10.f * salidaC);
        lights[LUZ_PULSO_C].setSmoothBrightness(salidaC, 5e-6f);

        outputs[SALIDA_PULSO_D].setVoltage(10.f * salidaD);
        lights[LUZ_PULSO_D].setSmoothBrightness(salidaD, 5e-6f);
    }
};

// widget
struct RereloModuloWidget : ModuleWidget
{
    RereloModuloWidget(RereloModulo *modulo)
    {

        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RereloModulo.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::MODULO_ANCHO_10_HP, dimensiones::MODULO_ALTURA_3_U);

        // params

        // tempo base, compartido por los cuatro canales
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(PORCENTAJE_PPM_X, PORCENTAJE_PPM_Y),
            modulo, RereloModulo::PARAM_PPM));

        // boton y entrada de resincronizado, tambien compartidos
        addParam(createParamCentered<TL1105>(
            posicionador.posicion(PORCENTAJE_BOTON_RESINC_X, PORCENTAJE_BOTON_RESINC_Y),
            modulo, RereloModulo::PARAM_BOTON_RESINC));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_RESINC_X, PORCENTAJE_ENTRADA_RESINC_Y),
            modulo, RereloModulo::ENTRADA_RESINC));

        // canal b: perilla de desfase, atenuversor y entrada cv
        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(PORCENTAJE_DESFASE_B_X, PORCENTAJE_DESFASE_B_Y),
            modulo, RereloModulo::PARAM_DESFASE_B));

        addParam(createParamCentered<Trimpot>(
            posicionador.posicion(PORCENTAJE_DESFASE_CV_ATEN_B_X, PORCENTAJE_DESFASE_CV_ATEN_B_Y),
            modulo, RereloModulo::PARAM_DESFASE_CV_ATEN_B));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_DESFASE_B_X, PORCENTAJE_ENTRADA_DESFASE_B_Y),
            modulo, RereloModulo::ENTRADA_DESFASE_B));

        // canal c: perilla de desfase, atenuversor y entrada cv
        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(PORCENTAJE_DESFASE_C_X, PORCENTAJE_DESFASE_C_Y),
            modulo, RereloModulo::PARAM_DESFASE_C));

        addParam(createParamCentered<Trimpot>(
            posicionador.posicion(PORCENTAJE_DESFASE_CV_ATEN_C_X, PORCENTAJE_DESFASE_CV_ATEN_C_Y),
            modulo, RereloModulo::PARAM_DESFASE_CV_ATEN_C));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_DESFASE_C_X, PORCENTAJE_ENTRADA_DESFASE_C_Y),
            modulo, RereloModulo::ENTRADA_DESFASE_C));

        // canal d: perilla de desfase, atenuversor y entrada cv
        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(PORCENTAJE_DESFASE_D_X, PORCENTAJE_DESFASE_D_Y),
            modulo, RereloModulo::PARAM_DESFASE_D));

        addParam(createParamCentered<Trimpot>(
            posicionador.posicion(PORCENTAJE_DESFASE_CV_ATEN_D_X, PORCENTAJE_DESFASE_CV_ATEN_D_Y),
            modulo, RereloModulo::PARAM_DESFASE_CV_ATEN_D));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_DESFASE_D_X, PORCENTAJE_ENTRADA_DESFASE_D_Y),
            modulo, RereloModulo::ENTRADA_DESFASE_D));

        // salidas y luces, una columna por canal

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_A_X, PORCENTAJE_SALIDA_Y),
            modulo, RereloModulo::SALIDA_PULSO_A));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_A_X, PORCENTAJE_LUCES_Y),
            modulo, RereloModulo::LUZ_PULSO_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_B_X, PORCENTAJE_SALIDA_Y),
            modulo, RereloModulo::SALIDA_PULSO_B));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_B_X, PORCENTAJE_LUCES_Y),
            modulo, RereloModulo::LUZ_PULSO_B));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_C_X, PORCENTAJE_SALIDA_Y),
            modulo, RereloModulo::SALIDA_PULSO_C));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_C_X, PORCENTAJE_LUCES_Y),
            modulo, RereloModulo::LUZ_PULSO_C));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_D_X, PORCENTAJE_SALIDA_Y),
            modulo, RereloModulo::SALIDA_PULSO_D));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_D_X, PORCENTAJE_LUCES_Y),
            modulo, RereloModulo::LUZ_PULSO_D));
    }
};

Model *modeloRerelo = createModel<RereloModulo, RereloModuloWidget>("rerelo");