cmake_minimum_required(VERSION 3.0.0)
project(opencvtest VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
include(CTest)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(OpenCV CONFIG REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(OpenGL REQUIRED)

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set( GLFW_DIR 
${CMAKE_CURRENT_SOURCE_DIR}/GLFW)

set( assimp_DIR 
${CMAKE_CURRENT_SOURCE_DIR}/include)

set( dlls_DIR 
${CMAKE_CURRENT_SOURCE_DIR}/dlls)

file(GLOB SOURCES "*.cpp" ${GLFW_DIR}/include/glad/glad/glad.c)
file(GLOB HEADERS "*.h" "*.hpp")
file(GLOB SHADERS "*.vert" "*.frag" "*.vs" "*.fs" )

include_directories( 
    ${GLFW_DIR}/include/glad/ 
    ${GLFW_DIR}/include/glfw/
    ${GLFW_DIR}/include/freeimage/
    ${GLFW_DIR}/include/glew
    ${GLFW_DIR}/include/glm
    ${GLFW_DIR}/include/freeglut
	${assimp_DIR}
	${dlls_DIR}
)

message( "\nBuild mode test is: ${CMAKE_BUILD_TYPE}" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("\nOpenGL_GLUT_GLFW_Program -- Debug mode ****************")
	link_directories(
	${GLFW_DIR}/lib/glad/Debug/
	${GLFW_DIR}/lib/glfw/Debug/
	${GLFW_DIR}/lib/freeglut/Debug/
	${GLFW_DIR}/lib/freeimage/Debug/
	${GLFW_DIR}/lib/glew/Debug/
	${GLFW_DIR}/lib/glm/Debug/
	${dlls_DIR}/
	)
else()
    message("\nOpenGL_GLUT_GLFW_Program -- Release mode ****************")
	link_directories(
	${GLFW_DIR}/lib/glad/Release/
	${GLFW_DIR}/lib/glfw/Release/
	${GLFW_DIR}/lib/freeglut/Release/
	${GLFW_DIR}/lib/freeimage/Release/
	${GLFW_DIR}/lib/glew/Release/
	${GLFW_DIR}/lib/glm/Release/
	)
endif ()

link_directories(${assimp_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dlls)

# Definir un directorio de salida para los archivos binarios
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Agregar el directorio de salida a las rutas de búsqueda de CMake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)

message( "\nCopy files in Debug ****************" )
message( "DEBUG DIR in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}\n****************" )

# copy shader files to build directory
file(GLOB SHADERS
			"resources/shader/*.vs"
			"resources/shader/*.fs"
)

file(GLOB DLLS "dlls/*.dll")

file(GLOB YMLS "resources/camera_parameters/*.yml")

file(GLOB OBJ_DIR LIST_DIRECTORIES true "resources/objects/*")
foreach(OBJ_DIR ${OBJ_DIR})
	message("Carpetas OBJ: ${OBJ_DIR}")
	get_filename_component(DIR_NAME ${OBJ_DIR} NAME)
	message("Folder found: ${DIR_NAME}")
	file(GLOB OBJ_FILE 
				"${OBJ_DIR}/*.jpg"
				"${OBJ_DIR}/*.mtl"
				"${OBJ_DIR}/*.obj"
				"${OBJ_DIR}/*.png"
				"${OBJ_DIR}/*.txt"
	)

	foreach(OBJ_FILE ${OBJ_FILE})
		message("Archivo OBJ: ${OBJ_FILE}")
		file(COPY ${OBJ_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/resources/objects/${DIR_NAME})
	endforeach()

	file(GLOB DIR_TEXTURES LIST_DIRECTORIES true "resources/objects/${DIR_NAME}/*")
	list(FILTER DIR_TEXTURES EXCLUDE REGEX ".*\\..*")
	message("DIR_TEXTURES texturas: ${DIR_TEXTURES}")

	foreach(DIR_PATH ${DIR_TEXTURES})
		get_filename_component(DIR_NAME_ONLY ${DIR_PATH} NAME)
		message("Directorio encontrado: ${DIR_NAME_ONLY}")
		file(GLOB OBJ_FILE 
				"${DIR_TEXTURES}/*.jpg"
				"${DIR_TEXTURES}/*.png"
				"${DIR_TEXTURES}/*.txt"
		)

		foreach(OBJ_FILE ${OBJ_FILE})
			message("Archivo Textura: ${OBJ_FILE}")
			file(COPY ${OBJ_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/resources/objects/${DIR_NAME}/${DIR_NAME_ONLY})
		endforeach()

	endforeach()

endforeach()

foreach(DLL ${DLLS})
  message("Archivo DLL: ${DLL}")
  file(COPY ${DLLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
endforeach()

foreach(YML ${YMLS})
  message("Archivo YML: ${YML}")
  file(COPY ${YMLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/resources/camera_parameters)
endforeach()

foreach(SHADERS ${SHADERS})
message("File SHADERS: ${SHADERS}")
	file(COPY ${SHADERS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/resources/shader)
endforeach()

add_executable( ${PROJECT_NAME}  ${HEADERS} ${SOURCES} ${SHADERS} )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug")
	if ( WIN32 )
	    message("\nOpenGL_GLUT_GLFW_Program -- Debug mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME} 
						   opengl32
						   assimp
						   freeglut_staticd.lib
						   glad.lib
	                       glfw3.lib
						   glew32d.lib
						   FreeImaged.lib
						   glm_static.lib
						   
						    )
	else()
		message("\nOpenGL_GLUT_GLFW_Program -- Debug mode Linux ****************")
		target_link_libraries( ${PROJECT_NAME} 
						   ${OPENGL_LIBRARY}
							-lm
							-lGL
							-lGLU
							-lX11
							-lXi
							-lXxf86vm 
							-lXrandr 
							-pthread 
							-ldl
							-lm
						   libglad.a
	                       libglfw3.a
						   libGLEWd.a
						   libglut.a
						   libfreeimage.a
						   libglm_static.a
						   ${ASSIMP_LIBRARY}
						    )

	endif()					    	
else()
	if ( WIN32 )
		message("\nOpenGL_GLUT_GLFW_Program -- Release mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME} 
						   opengl32
						   assimp 
						   freeglut_static.lib
						   glad.lib
	                       glfw3.lib
						   glew32.lib
						   FreeImage.lib
						   glm_static.lib
						    )
	else()
		message("\nOpenGL_GLUT_GLFW_Program -- Release mode Linux ****************")
		target_link_libraries( ${PROJECT_NAME} 
						  ${OPENGL_LIBRARY}
												    
							-lm
							-lGL
							-lGLU
							-lX11
							-lXi
							-lXxf86vm 
							-lXrandr 
							-pthread 
							-ldl
							-lm
						   libglad.a
	                       libglfw3.a
						   libGLEW.a
						   libglut.a
						   libfreeimage.a
						   libglm_static.a
						   ${ASSIMP_LIBRARY}
							)

	endif()	
endif ()                      
