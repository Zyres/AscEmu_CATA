PROJECT(lualib C)

SET(prefix ${DEPENDENCY_ROOT}/${PROJECT_NAME} )

set( sources
lapi.c
lauxlib.c
lbaselib.c
lbitlib.c
lcode.c
lcorolib.c
lctype.c
ldblib.c
ldebug.c
ldo.c
ldump.c
lfunc.c
lgc.c
linit.c
liolib.c
llex.c
lmathlib.c
lmem.c
loadlib.c
lobject.c
lopcodes.c
loslib.c
lparser.c
lstate.c
lstring.c
lstrlib.c
ltable.c
ltablib.c
ltm.c
lua.c
lundump.c
lutf8lib.c
lvm.c
lzio.c )

SET( headers
lapi.h
lauxlib.h
lcode.h
lctype.h
ldebug.h
ldo.h
lfunc.h
lgc.h
llex.h
llimits.h
lmem.h
lobject.h
lopcodes.h
lparser.h
lprefix.h
lstate.h
lstring.h
ltable.h
ltm.h
lua.h
lua.hpp
luaconf.h
lualib.h
lundump.h
lvm.h
lzio.h )

foreach(src IN ITEMS ${sources} )
  SET( SRCS ${SRCS} ${prefix}/${src} )
endforeach(src)
foreach(src IN ITEMS ${headers} )
  SET( HEADERS ${HEADERS} ${prefix}/${src} )
endforeach(src)

ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS)
IF( NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
	ADD_DEFINITIONS( -DLUA_USE_APICHECK )
ENDIF()
ADD_LIBRARY( ${PROJECT_NAME} STATIC ${SRCS} ${HEADERS} )
REMOVE_DEFINITIONS( -std=c++0x )
IF( IS_64BIT )
	SET_PROPERTY( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_FLAGS "-fPIC")
ENDIF()
target_link_libraries(${PROJECT_NAME})
