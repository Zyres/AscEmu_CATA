# Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org>

#ignore the warnings for dependencies (external code)
if(MSVC)
  add_definitions(/W0)
else()
  add_definitions(-w)
endif()

# add our dependecies
add_subdirectory(bzip2)
add_subdirectory(dbcfile)
add_subdirectory(vmaplib)
add_subdirectory(recastnavigation)
add_subdirectory(g3dlite)
add_subdirectory(libmpq)

# add zlib and pcre for windows compile
if(NEED_ZLIB_PCRE)
	add_subdirectory(zlib)
	add_subdirectory(pcre)
endif()

# add lualib if we need it for LuaEngin or LuaBridge
add_subdirectory(lualib)
