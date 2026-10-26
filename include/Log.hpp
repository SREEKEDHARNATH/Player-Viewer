#pragma once

#include <SDL_log.h>
#include <string>
enum LOG{
    INFO,
    WARN,
    ERROR
};

void log(int x, LOG l);
void log(float x, LOG l);
void log(double x, LOG l);
void log(long x, LOG l);
void log(long long x, LOG l);

void log(const char* data, LOG l);
void log(std::string& data, LOG l);
void log(const std::string& data, LOG l);
