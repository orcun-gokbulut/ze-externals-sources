# Copyright (C) 2007-2011 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( md5 C )
cmake_minimum_required ( VERSION 2.6 )
include ( dist.cmake )

install_lua_module ( md5.core src/md5.c src/md5lib.c src/md5.def )
install_lua_module ( des56 src/des56.c src/ldes56.c src/des56.def )

install_lua_module ( md5 src/md5.lua )
install_data ( README )
install_doc ( doc/ )
install_test ( tests/ )

if ( BUILD_TESTING )
	# Binary Tests
	include_directories ( src )
	add_executable ( fdemo tests/DES56/fdemo.c src/des56.c )
	add_test ( fdemo fdemo -demo -c 1000 )
	find_library ( CRYPT_LIB crypt )
	if (CRYPT_LIB)
		add_executable ( ftest tests/DES56/ftest.c src/des56.c )
		target_link_libraries ( ftest ${CRYPT_LIB} )
		add_test ( ftest1 ftest 4869207468657265 a820105412de3991 77129bd8c052e7 )
		add_test ( ftest2 ftest 1.21280ac -c 5000000 -k 1000000 -C 100000 -K 100000 )
	endif ()
endif ()

add_lua_test ( tests/test.lua
  src # workaround package.path
)
