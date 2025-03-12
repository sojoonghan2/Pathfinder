#pragma once

#include <SFML/Graphics.hpp>

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <thread>
#include <vector>
#include <array>
#include <print>
#include <iostream>
#include <chrono>
#include <queue>
#include <unordered_map>

#include "define.h"
#include "../Common/protocol.h"
#include "../Common/util.h"
#include "../Common/CommonDefine.h"
