# Copyright (C) 2007-2011 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(luasocket C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(dist.cmake)

# Shared library exporting.
IF (WIN32)
	ADD_DEFINITIONS("-DMIME_API=__declspec(dllexport)"
	                "-DLUASOCKET_API=__declspec(dllexport)")
ENDIF(WIN32)

IF (WIN32 AND NOT CYGWIN)
	SET(SOCKETC src/wsocket.c)
ELSE ()
	SET(SOCKETC src/usocket.c)
ENDIF (WIN32 AND NOT CYGWIN)

SET(SRC_SOCKET src/luasocket.c src/timeout.c src/buffer.c src/io.c
               src/auxiliar.c src/options.c src/inet.c src/tcp.c
               src/udp.c src/except.c src/select.c ${SOCKETC} )

IF (WIN32 AND NOT CYGWIN)
	SET(LIB_SOCKET ws2_32)
ENDIF (WIN32 AND NOT CYGWIN)

# Binary
install_lua_module ( socket.core ${SRC_SOCKET} LINK ${LIB_SOCKET} )
install_lua_module ( mime.core src/mime.c )
# Lua 
install_lua_module ( ltn12 src/ltn12.lua )
install_lua_module ( mime src/mime.lua )
install_lua_module ( socket src/socket.lua )
install_lua_module ( socket.ftp src/ftp.lua )
install_lua_module ( socket.http src/http.lua )
install_lua_module ( socket.smtp src/smtp.lua )
install_lua_module ( socket.tp src/tp.lua )
install_lua_module ( socket.url src/url.lua )

install_foo ( etc/ )
install_test ( test/ )
install_example ( samples/ )
install_doc ( doc/ )
install_data ( README NEW LICENSE )
