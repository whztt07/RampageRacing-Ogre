#/*
#-----------------------------------------------------------------------------
#Filename:    CMakeLists.txt
#-----------------------------------------------------------------------------
#
#This source file is part of the
#   ___                 __    __ _ _    _ 
#  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
# //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
#/ \_// (_| | | |  __/  \  /\  /| |   <| |
#\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
#      |___/                              
#      Tutorial Framework
#      http://www.ogre3d.org/tikiwiki/
#-----------------------------------------------------------------------------
#*/
cmake_minimum_required(VERSION 2.6)
 
project(OgreApp)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/dist/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
 
if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
endif(WIN32)
 
if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

	elseif(EXISTS "/usr/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one

	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE")
endif(UNIX)
 
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
 
set(CMAKE_DEBUG_POSTFIX "_d")
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 
find_package(OGRE REQUIRED)
 
#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#  message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
#endif()

#find the package for the physics
find_package(BULLET REQUIRED)
find_path(BULET_LIB_PATH BULLET_LIB_PATH)

set(BULLET_DEBUG_LIB_PATH ${BULET_LIB_PATH}/Debug)
set(BULLET_RELEASE_LIB_PATH ${BULET_LIB_PATH}/Release)
  
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()
 
# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	if (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time system)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS})
	if (NOT Boost_FOUND)
		message(WARNING "Boost not found")
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} )
	endif()
	#find_package(Boost QUIET)
 
	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()
 
set(HDRS
	./src/BaseApplication.h
	./src/Game.h
	./src/DotScene.h
	./src/GameObject.h
	./src/Car.h
	./src/Physics.h
	./src/System.h
	./src/Scene.h
	./src/Client.h
	./src/TriggerVolume.h
	./src/CarCollisionTest.h
	./src/OBJ.h
	./src/Missile.h
	./src/Controller.h
	./src/Mine.h
	./src/ItemBox.h
	./src/MovableText.h
	./src/PlayerLabel.h
	./src/Sound.h
)
 
set(SRCS
	./src/BaseApplication.cpp
	./src/Game.cpp
	./src/DotScene.cpp
	./src/GameObject.cpp
	./src/Car.cpp
	./src/Physics.cpp
	./src/Scene.cpp
	./src/Client.cpp
	./src/TriggerVolume.cpp
	./src/CarCollisionTest.cpp
	./src/OBJ.cpp
	./src/Missile.cpp
	./src/Controller.cpp
	./src/Mine.cpp
	./src/ItemBox.cpp
	./src/MovableText.cpp
	./src/PlayerLabel.cpp
	./src/Sound.cpp
	./src/ScoreScreen.cpp
)
 
include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${OGRE_Overlay_INCLUDE_DIRS}
	${BULLET_INCLUDE_DIRS}
)

#set the files for the server
set(SVR_HDRS
	./src/Server.h
)
set(SVR_SRCS 
	./src/ServerMain.cpp
	./src/Server.cpp
)

#find the raknet stuff
find_path(RakNet_INCLUDE_PATH REQUIRED)
find_path(RakNet_DLL_DEBUG_PATH REQUIRED)
find_path(RakNet_DLL_RELEASE_PATH REQUIRED)

include_directories(${RakNet_INCLUDE_PATH})

#find the tinyXML stuff
find_path(XML_DLL_DEBUG_PATH REQUIRED)
find_path(XML_DLL_RELEASE_PATH REQUIRED)
find_path(XML_INCLUDE_PATH REQUIRED)

include_directories(${XML_INCLUDE_PATH})

#find the FMOD Audio stuff
find_path(FMOD_API_DIR REQUIRED)

include_directories(${FMOD_API_DIR}/inc/)
 
add_executable(OgreApp WIN32 ${HDRS} ${SRCS})
add_executable(Server ${SVR_HDRS} ${SVR_SRCS})
 
set_target_properties(OgreApp PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(OgreApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES})

#link the bullet libs
target_link_libraries(OgreApp 	debug ${BULLET_DEBUG_LIB_PATH}/LinearMath_Debug.lib 
												debug ${BULLET_DEBUG_LIB_PATH}/BulletSoftBody_Debug.lib 
												debug ${BULLET_DEBUG_LIB_PATH}/BulletDynamics_Debug.lib 
												debug ${BULLET_DEBUG_LIB_PATH}/BulletCollision_Debug.lib
												
												optimized ${BULLET_RELEASE_LIB_PATH}/LinearMath.lib 
												optimized ${BULLET_RELEASE_LIB_PATH}/BulletSoftBody.lib 
												optimized ${BULLET_RELEASE_LIB_PATH}/BulletDynamics.lib 
												optimized ${BULLET_RELEASE_LIB_PATH}/BulletCollision.lib)

target_link_libraries(OgreApp 	debug ${XML_DLL_DEBUG_PATH}/tinyxml2.lib 
								optimized  ${XML_DLL_RELEASE_PATH}/tinyxml2.lib)

target_link_libraries(OgreApp 	debug ${RakNet_DLL_DEBUG_PATH}/RakNetDLL.lib
								optimized ${RakNet_DLL_RELEASE_PATH}/RakNetDLL.lib)

target_link_libraries(OgreApp 	debug ${FMOD_API_DIR}/lib/fmodexL_vc.lib
								optimized ${FMOD_API_DIR}/lib/fmodex_vc.lib)

target_link_libraries(OgreApp 	XInput9_1_0.lib)

target_link_libraries(Server 	debug ${RakNet_DLL_DEBUG_PATH}/RakNetDLL.lib
								optimized ${RakNet_DLL_RELEASE_PATH}/RakNetDLL.lib)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)
 

if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif(MINGW OR UNIX)
 
if(WIN32)
 
	install(TARGETS OgreApp
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)

	install(FILES 
		${CMAKE_SOURCE_DIR}/copy/ogre.cfg
		${CMAKE_SOURCE_DIR}/copy/plugins.cfg
		${CMAKE_SOURCE_DIR}/copy/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/copy/resources.cfg
		${CMAKE_SOURCE_DIR}/copy/resources_d.cfg
		DESTINATION ../
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/ogre.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/ogre.cfg
		DESTINATION bin
		CONFIGURATIONS Debug
	)
 
        # NOTE: for the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
        # so you'll have to change that to make it work with 1.7.1
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/OgreOverlay.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
		${OGRE_PLUGIN_DIR_REL}/cg.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		${RakNet_DLL_RELEASE_PATH}/RakNetDLL.dll
		${FMOD_API_DIR}/fmodex.dll
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreOverlay_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
		${OGRE_PLUGIN_DIR_DBG}/cg.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		${RakNet_DLL_DEBUG_PATH}/RakNetDLL.dll
		${FMOD_API_DIR}/fmodexL.dll
		DESTINATION bin
		CONFIGURATIONS Debug
	)
endif(WIN32)

if(UNIX)
 
	install(TARGETS OgreApp
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
endif(UNIX)
 
