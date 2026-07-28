# popusintes-rack

Popusintes es una línea de sintetizadores creada por piruetas.

Este repositorio aloja versiones en Eurorack virtual para ser usadas en el software VCV Rack.

## Criterios de diseño e inspiraciones

- Cada módulo desarrollado es un clásico de la síntesis modular.
- Cada módulo tiene 2 canales, sin concursos ni sortesos.
- Están en español con lenguaje no militar.
- Las simetrías y las sincronizaciones son evitadas.
- Hay luces.

## Módulos

### compa

Comparadores

### envo

Generador de envolventes.

### pane

Panel en blanco.

### relo

Relojes

### rerelo

Más relojes

### secu

Secuenciadores

### suma

Mezcladores

## Estructura del repositorio

- [res/](./res/)
- [src/](./src/)
- [.gitignore](./.gitignore)
- [LICENSE](./LICENSE)
- [Makefile](./Makefile)
- [plugin.json](./plugin.json)
- [README.md](./README.md): este archivo.

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
