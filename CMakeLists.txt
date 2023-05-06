cmake_minimum_required(VERSION 3.0.0)
project(opencvtest VERSION 0.1.0)

include(CTest)
enable_testing()


find_package(OpenCV CONFIG REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(OpenGL REQUIRED)

set( GLFW_DIR 
${CMAKE_CURRENT_SOURCE_DIR}/GLFW)

file(GLOB SOURCES "*.cpp" ${GLFW_DIR}/include/glad/glad/glad.c )
file(GLOB HEADERS "*.h" )
file(GLOB SHADERS "*.vert" "*.frag" "*.vs" "*.fs" )

include_directories( 
    ${GLFW_DIR}/include/glad/ 
    ${GLFW_DIR}/include/glfw/
    ${GLFW_DIR}/include/freeimage/
    ${GLFW_DIR}/include/glew
    ${GLFW_DIR}/include/glm
    ${GLFW_DIR}/include/freeglut
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

add_executable( ${PROJECT_NAME}  ${HEADERS} ${SOURCES} ${SHADERS} )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug")
	if ( WIN32 )
	    message("\nOpenGL_GLUT_GLFW_Program -- Debug mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME} 
						   opengl32
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
						    )

	endif()					    	
else()
	if ( WIN32 )
		message("\nOpenGL_GLUT_GLFW_Program -- Release mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME} 
						   opengl32
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
							)

	endif()	
endif ()                      
