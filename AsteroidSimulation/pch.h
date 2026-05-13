#pragma once

#define NUM_ASTEROIDS 128
#define MAX_PARTICLE_COUNT NUM_ASTEROIDS + 1

#include <string>
#include <algorithm>
#include <cmath>
#include <array>
#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <fstream>
#include <chrono>

#include <DirectXMath.h>

#include <IvyyyGameObject.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyPhysic.h>
#include <IvyyyComponent.h>
#include <IvyyyForceGenerator.h>
#include <IvyyyTime.h>
#include <IvyyyGizmo.h>
#include <IvyyyInput.h>
#include <IvyyyDebug.h>
#include <IvyyyD3D.h>
#include <IvyyyMathF.h>
#include <IvyyyVector3.h>

using namespace Ivyyy;
using namespace DirectX;
using namespace std;