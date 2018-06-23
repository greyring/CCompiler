cmake_minimum_required (VERSION 2.8)

project (CCompiler)

aux_source_directory(. DIR_SRCS)

add_executable(test ${DIR_SRCS})