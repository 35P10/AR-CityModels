# ComputerGraphics FinalProject
## AUGMENTED CITY: Implementación De Realidad Aumentada Con Marcadores Aruco para la Creación y Manipulación de Maquetas de Ciudades

En este proyecto, hemos utilizado un conjunto de tecnologías como OpenCV, ArUco, ChArUco y OpenGL para crear una experiencia de realidad aumentada inmersiva y dinámica. Nuestro objetivo principal ha sido utilizar esta tecnología para mejorar la interacción con
objetos en un entorno 3D.

<p align="center">
  <img width="40%" height="40%" src="gif/r1.gif">
</p>
<p align="center">
  <img width="40%" height="40%" src="gif/r2.gif">
</p>

### PARTE GRÁFICA
Se enfoca en representar en tiempo real la maqueta de ciudades. Para lograr esto, se utiliza la potencia de OpenGL GLFW que permite la creación de una escena virtual y el renderizado de objetos en un espacio tridimensional.

### Create marker

int create_marker (int markerId, std::string outfile = "marker.png", int markerSize = 200, int borderBits = 1)

```
create_marker(ID,"markerID.png");
```

### Notas

* Cambiar path de modelos en  ```\augmented_mockup.hpp```
* [Calibrar camara](https://github.com/opencv/opencv_contrib/blob/4.x/modules/aruco/samples/calibrate_camera_charuco.cpp)

### Resources
Models by: Marc-Mons in [turbosquid](https://www.turbosquid.com/es/Search/Artists/Marc-Mons)
