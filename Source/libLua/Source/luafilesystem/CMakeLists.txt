# Copyright (C) 2007-2010 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(luafilesystem C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(dist.cmake)

# Build
ADD_LUA_MODULE(lfs src/lfs.c vc6/lfs.def)

# Install all files and documentation
INSTALL (TARGETS lfs DESTINATION ${INSTALL_CMOD})
INSTALL (DIRECTORY doc/ DESTINATION ${INSTALL_DOC} PATTERN ".git" EXCLUDE)
INSTALL (DIRECTORY tests/ DESTINATION ${INSTALL_TEST} PATTERN ".git" EXCLUDE)
INSTALL (FILES README DESTINATION ${INSTALL_DATA})