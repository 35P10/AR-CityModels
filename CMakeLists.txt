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
