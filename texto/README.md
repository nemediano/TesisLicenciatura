# Modelado Gráfico de un Cuerpo Neumático con OpenGL a Base de Ecuaciones Diferenciales

Este directorio contiene el texto trabajo que presente para titularme de la licenciatura de [Matemáticas Aplicadas y Computación](https://mac.acatlan.unam.mx/) en la [FES Acatlan](https://www.acatlan.unam.mx/) de la [UNAM](https://www.unam.mx/).

[Modelado Gráfico de un Cuerpo Neumático con OpenGL a Base de Ecuaciones Diferenciales](https://github.com/nemediano/TesisLicenciatura/texto) © 2008 por [Jorge Antonio García Galicia](https://www.linkedin.com/in/jorgegarciagalicia/) está licenciado bajo Creative Commons Attribution 4.0 International.

![cc](https://mirrors.creativecommons.org/presskit/icons/cc.svg?) ![by](https://mirrors.creativecommons.org/presskit/icons/by.svg)

Para ver una copia de esta licencia, visite http://creativecommons.org/licenses/by/4.0/

El trabajo trata de cómo hacer una simulación gráfica interactiva de un cuerpo neumático. Es en esencia el desarrollo de un modelo que aplica la idea que presentaron Matyka Maciej y Ollila Mark en su trabajo [Pressure Model of Soft Body Simulation](https://arxiv.org/abs/physics/0407003)

La tesis fue presentada en 2008. Sin embargo, decidí hacer una nueva edición usando herramientas modernas. Si por alguna razón quisieras comparar con la versión del 2008, la UNAM permite descargar pdf de todas sus tesis desde la página del [sistema de bibliotecas](https://tesiunam.dgb.unam.mx).

Para compilar se requiere el engine [XeLaTeX](https://tug.org/xetex/), de [Biber](http://biblatex-biber.sourceforge.net/) y el paquere [pygment](https://pygments.org/) de python dado que hago uso del paquete [minted](https://ctan.org/pkg/minted?lang=en) que tiene esa dependencia. Por lo mismo para compilar se requiere que se pase una opción extra a `xelatex`:

```
$xelatex -shell-escape ...
```

En [Ubuntu](https://ubuntu.com/), se pueden instalar todas estas dependencias por medio del gestor de paquetes y se puede usar [Kile](https://kile.sourceforge.io/) como IDE haciendo minimos cambios en la configuracion.

La primera vez, para compilar desde cero, se puede hacer:

1. Ejecutar XeLaTeX
1. Ejecutar Biber
1. Ejecutar XeLaTeX
1. Ejecutar XeLaTeX
1. Ver el pdf

