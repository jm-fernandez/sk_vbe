# sk_vbe

## Introduccion

[School Kombat](https://github.com/jm-fernandez/sk) es un juego para MS-DOS, desarrollado para el concurso C:\DOS\CONTEST organizado por el [msdos.club](https://msdos.club/). El juego se desarrollo usando DOSBox como entorno de pruebas, y la version [0.2](https://github.com/jm-fernandez/sk/releases/tag/0.2) presentada al concuros no funcionaba correctamente en PCem. 

Este programa es un experimento para solucionar algunos de los problemas de School Kombat 0.2 en PCem. El valor de este repositorio, siguiendo el espiritu del concurso, es sobre todo documental. 

La nueva version [School Kombat 0.3](https://github.com/jm-fernandez/sk/releases/tag/0.3) soluciona los problemas de la version anterior, y funciona en PCem en varias de las tarjetas disponibles, siempre que se tenga instalado el driver de video adecuado (Por ejemplo, UniVBE, S3VBE20, etc.)

## El problema
Pero, por que no funcionaba a School Kombat 0.2 en PCem? Al intentar correr School Kombat 2.o en PCem, el usuario puede encotrarse dos escenarios diferentes:
   - El juego termina de forma inesperada sin mostrar nada en pantalla. Esto puede ser debido a:
        -> Driver de video no disponible, o
        -> Tarjeta grafica no soportada (Por ejemplo, que no tenga sufuciente memoria)
   - El juego comienza pero la pantalla se muetra completamente en negro, o completamente en rosa... o ambos
        -> Driver de video disponible y tarjeta soportada. Inicializacion de modo de video incorrecto.

En el primero de los escearios, la solucion es instalar el driver de video y cruzar los dedos esperando que la tarjeta sea soportada. 
El segundo de los casos es el caso interesante. Al inicilar un modo grafico, existe la posibilidad de que el acceso a la memoria de video sea por bancos o lineal. Un modo grafico puede soportar tan solo uno de esos modos de acceso, o puede soportar ambos a la vez. Si ambos modos de acceso son soportados, el usuario debe indicar la intencion de usar el acceso lineal activando un flag al inicializar el modo de video. En el caso de la implementacion en DosBox, este flag no es obligatorio, y School Kombat no lo estaba usando.

En caso de que se disponga de un driver que exponga un modo grafico exclusivamente lineal, entonces School Kombat funcionaria correctamente. En el caso de que School Kombat elige un modo grafico donde ambos modos de acceso estan disponibles, entonces la inicializacion del modo de video no es correcta, y el juego no renderizara el contenido adecuadamente

## La solucion

Una vez que se entiende el problema, una posible solucion es obvia: El objetivo es soportar tantas tarjetas de video como sea posible, por lo que implementar un driver desde cero no es razonable. Pero lo que si se puede hacer es implementar una extension para driver de video existente, proporcionando un modo de video a la medida de lo que necesita School Kombat. Esta extension se mantiene estrictamente en los limites de lo defino en el estandar VBE 2.0, por lo que no deberia romper otras aplicaciones.

## Mas problemas

Tambien parece ser que la reconfiguracion del DAC no funciona correctamente con algunos de los driver, no solo no llevando a cabo la configuracion, sino que incluso no reportando errores o reportando informacion incorrecta al consultar la configuracion actual. Para evitar este problema, School Kombat 0.3 evita reconfigurar el DAC y usa paletas de 6 bits por color. En esta extension implementa el soporte para la configuracion del DAC en software. De esa forma School Kombat 0.2, con la ayuda de la extension, es capaz de renderizar adecuadamente los contenidos.

## Aun mas problemas

Desafortunadamente, aunque esta extension permite a School Kombat 0.2 renderizar los contenidos adecuadamente, el codigo de reproduccion de ficheros WAV puede provocar un bloqueo debido a un porblema de reentrada. En algunas configuraciones el bloqueo es practicamente sistematico. En otros casos la probabilidad de experimentar el bloqueo es menor y se puede jugar una partida hasta el final.

## El Software

Esta extension es un TSR que se puede cargar y descargar, que extiende los servicios proporcionados por la interrupcion 0x10, y que muestra como proporcionar servicios a través de la interrupcion 0x2F (interrupcion Multiplex). El objetivo es eminementente didactico, por lo que se ha limitado al maximo el uso del ensamblador, y se ha tendido a proporcionar soluciones lo mas genericas posibles. 
El codigo esta pensado para ser compilado con OpenWatcom, y usa extensiones disponibles en dicho compilador. Portar el codigo para otros compiladores deberia ser relativamente sencillo.

## Como compilar

El repositorio cuenta con un Makefile para compilar la aplicion usando OpenWatcom. 

## Advertencia final

Este software es solo una prueba de concepto que intenta documentar como implmenetar un TSR en MS-DOS. No tiene mucho sentido, mas alla del reto tecnico. 
Si se quiere probar School Kombat en PCem, o en una maquina real, lo mas logico es usar la version 0.3
