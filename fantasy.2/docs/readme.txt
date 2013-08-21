			COMO USAR EL FANTASY
                        --------------------

Bueno, de momento empezaremos por un documento simpl¢n, y seg£n vaya
teniendo tiempo ya lo ampliaremos. Empecemos por lo b sico.

1 - A tener en cuenta por los grafistas:
----------------------------------------

La escena debe estar en el formato 3ds de toda la vida (ya mejorar ,
tranquis). Aparte los bugs/partes no implementadas, s¢lo hay que tener
en cuenta una cosa: el fichero .mat con los materiales debe existir, o 
si no va a petar. En caso de que no se encuentre la textura, le pone 
una por defecto, que es el hermoso color blanco.

Las limitaciones/bugs m s importantes por el momento son:

- S¢lo soporta luces omni. Creo que implementa correctamente las luces,
aunque como no tengo una documentaci¢n "real" del 3D Studio no puedo
estar seguro...

- Ojo con los números de frame. Si la escena tiene 400 frames y ponemos
una key en el frame 407, es bastante probable que el fantasy casque.

- Una aplicación que graba unos 3ds que suelen venirle bien al fantasy
es el Deep Exploration. Tan solo hay que reescalar los objetos y revisar
los parámetros de cámara tras exportarlo.

2 - A tener en cuenta por los coders:
-------------------------------------

El test.cpp da una idea muy buena de c¢mo usar el engine. De todas
formas, si se quiere partir desde cero los puntos b sicos son:

- Incluir el fichero "fantasy.h"
- Crear la ventana y el contexto OpenGL _ANTES_ de cualquier llamada al
engine.
- Tener en alg£n sitio el fichero img_png.h (lo utiliza en
textura.cpp).
- Y saber utilizar el interfaz del objeto Fantasy :). A saber:

* Fantasy::Fantasy(int maxscenes)

Inicializa el engine. Como par metro se le pasa el n£mero m ximo de
escenas que va a almacenar. OJO, no se pueden descargar escenas de
momento, y arreglarlo no tiene mucha prioridad, as¡ que se le pone un
n£mero lo bastante grande como para gestionar todas las escenas (si
s¢lo hay una, pues 1 ¢ 2 para evitar l¡mites raros) y punto. No hay
problema, la estructura necesaria no necesita mucha memoria si no se
carga la escena.

* Fantasy::~Fantasy()

Limpia todo. El engine est  pendiente de una gran revisi¢n de las
reservas y liberaciones de memoria, por lo que est  garantizado que se
va a dejar un mont¢n de memoria atr s, y lo que es peor memoria de
texturas, aunque no s‚ si OpenGL es lo bastante listo como para liberar
las texturas una vez se cierra la aplicaci¢n (por definici¢n deber¡a,
pero vete t£ a saber). No deber¡a afectar en una demo, pero para que el
c¢digo sea lo bastante limpio debe revisarse.

* int Fantasy::Load(char *3ds_filename,char *mat_filename)

Carga una escena. Devuelve un identificador de la escena, seg£n la
nomenclatura interna del engine. Es mejor NO FIARSE y almacenar este
identificador, para utilizarlo luego con SetScene. Por mucho que
empiece por 0 y suba de 1 en 1, nunca se sabe si puede cambiar con el
tiempo.

* void Fantasy::SetScene(int scene)

Establece la escena que se especifique como escena activa. Esta escena
ser  la que se renderice en la pr¢xima llamada a DrawFrame.

* void Fantasy::SetCamera(int camera)

Establece la c mara activa en la escena actual (por lo tanto se debe
llamar tras SetScene). Debe controlar que no se utilice una c mara
inexistente.

* void Fantasy::DrawFrame(int frame)

Renderiza el frame que se le especifique de la animaci¢n. Si se le
especifica un frame no v lido, renderiza el £ltimo frame v lido. Como
se puede ver, el engine no utiliza ning£n tipo de temporizaci¢n, s¢lo
dibuja el frame que se le diga. Esto fuerza un control externo de la
temporizaci¢n, pero a cambio es mucho m s limpio y portable (no se mete
ning£n c¢digo de timer dentro del engine, y se deja ese trabajo al
demosys). Para temporizarlo desde fuera s¢lo es necesario saber el
n£mero de frames de que consta la animaci¢n (lo dice el artista), el
tiempo de la animaci¢n (idem), y el tiempo actual (el coder del
demosys :). Adem s, as¡ se permite gran cantidad de efectos, como hacer
una escena marcha atr s, a c mara lenta, con efectos raperos, o
cualquier otra shorrada que se nos ocurra.

* void Fantasy::SetDynLighting(int yesno)

Activa o desactiva las luces dinámicas. Un valor de 0 las desactiva, y 
uno de 1 las activa. Por defecto las luces dinamicas estan activas.

* void FANTASY::SetFog(int yesno,int fog_type,float fog_r,float fog_g,float fog_b,float density,int fog_invert)

Activa o desactiva la niebla, con los parámetros adecuados. Si yesno = 0, se desactiva la niebla,
con lo que el resto de parámetros no sirven de nada. Los tipos de fog son:

0: GL_EXP
1: GL_EXP2
2: GL_LINEAR

El 0 es el más rápido, pero menos preciso, y el 2 el más lento y preciso

El resto de los parámetros son el color y la densidad de la niebla, con valores entre 0 y 1.

El valor de fog_invert define si se hace una niebla normal (fog_invert=0) o invirtiendo los planos (fog_invert=1). Para que r3d disfrute un poco más que nada :).


* void FANTASY::SetGlobalIlum(float globalIlum)

Esta función activa un valor global de iluminación, entre 0 y 1. Un valor de 1 significa que la escena se ve
tal cual, y un valor de 0 significa que la escena está totalmente negra. Útil para crossfades, supongo :)