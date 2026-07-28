# popusintes-rack

Popusintes es una línea de sintetizadores creada por piruetas.

Este repositorio aloja versiones en Eurorack virtual para ser usadas en el software VCV Rack.

## Módulos

### compa

### envo

### pane

un panel en blanco.

### relo

reloj

### rerelo

muchos relojes

### secu

secuenciador

### suma

mezclador

## Estructura del repositorio

- res/
- src/
- .gitignore
- LICENSE
- Makefile
- plugin.json
- README.md

## Desarrollo

Módulos programados en C++, usando el editor de texto Visual Studio Code.

## Compilar

```bash
make dep && make && make dist
```

## Gráficas

Las gráficas están hechas con el software Inkscape, y en formato SVG.

Los textos usan la tipografía Talagante de Uno Cá.

## Bibliografía

- <https://vcvrack.com/manual/PluginDevelopmentTutorial>
- <https://vcvrack.com/manual/Building>
- <https://github.com/VCVRack/library#vcv-library-database>
- <https://vcvrack.com/manual/Manifest>
