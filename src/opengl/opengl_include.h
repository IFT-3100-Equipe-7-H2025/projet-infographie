// Used to include all the necessary OpenGL headers in the right order.
// clang-format reorders the includes alphabetically, which is not the desired behavior, so we disable it for this file.
// glew must be included before glfw3, otherwise it will not work.
//
// Created by thierrycd on 2025-01-16.
//

// clang-format off
#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
// clang-format on
