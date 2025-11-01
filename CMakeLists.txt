cmake_minimum_required(VERSION 4.0)
project(ccalc C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ccalc
        calc.c
        dynarr.c
        dynarr.h
        stack_calculator.c
        stack_calculator.h
        status.c
        status.h
        tokenizer.c
        tokenizer.h
        parser.h
        parser.c)
