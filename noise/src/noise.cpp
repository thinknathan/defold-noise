#include <dmsdk/sdk.h>

#define PSIZE 2048
#define PMASK 2047

static unsigned char perm[PSIZE];
static double permGrad2[PSIZE][2];

static const double grad2[8][2] = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {1, 0}, {-1, 0}
};

static int fastFloor(double x) {
    int xi = static_cast<int>(x);
    return x < xi ? xi - 1 : xi;
}

static double extrapolate(int xsb, int ysb, double dx, double dy) {
    int index = perm[(perm[xsb & PMASK] + ysb) & PMASK] & 0x0E;
    return permGrad2[index][0] * dx + permGrad2[index][1] * dy;
}

static void init() {
    unsigned char source[PSIZE];
    for (int i = 0; i < PSIZE; i++)
        source[i] = static_cast<unsigned char>(i);

    for (int i = PSIZE - 1; i >= 0; i--) {
        unsigned char r = source[i];
        int j = static_cast<int>(rand()) & PMASK;

        source[i] = source[j];
        source[j] = r;
    }

    for (int i = 0; i < PSIZE + 2; i++) {
        perm[i] = source[i & PMASK];
        permGrad2[i][0] = grad2[perm[i] & 0x0E][0];
        permGrad2[i][1] = grad2[perm[i] & 0x0E][1];
    }
}

static double noise(double x, double y) {
    double s = 0.36602540378 * (x + y);
    int xsb = fastFloor(x + s);
    int ysb = fastFloor(y + s);

    double xsi = x - xsb;
    double ysi = y - ysb;

    double inoise = 0.0;
    double dx0 = xsi + ysi;
    double dy0 = xsi + 0.36602540378;
    double dx1 = dx0 - 1.0 - 0.36602540378 * 2.0;
    double dy1 = dy0 - 0.36602540378 - 0.36602540378 * 2.0;
    double dx2 = dx0 - 0.36602540378 * 2.0;
    double dy2 = dy0 - 1.0 - 0.36602540378 * 2.0;

    int xsb0 = xsb & PMASK;
    int ysb0 = ysb & PMASK;
    inoise += extrapolate(xsb0, ysb0, dx0, dy0);
    int xsb1 = (xsb + 1) & PMASK;
    int ysb1 = ysb0;
    inoise += extrapolate(xsb1, ysb1, dx1, dy1);
    int xsb2 = xsb0;
    int ysb2 = (ysb + 1) & PMASK;
    inoise += extrapolate(xsb2, ysb2, dx2, dy2);
    int xsb3 = xsb1;
    int ysb3 = ysb2;
    inoise += extrapolate(xsb3, ysb3, dx0 - 1.0, dy0 - 1.0);

    return inoise;
}

static double fractalNoise(double x, double y, int octaves, double lacunarity, double gain) {
    double amplitude = 1.0;
    double frequency = 1.0;
    double total = 0.0;

    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return total;
}

static int noise_noise(lua_State* L) {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);
    double result = noise(x, y);
    lua_pushnumber(L, result);
    return 1;
}

static int noise_fractal_noise(lua_State* L) {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);
    int octaves = lua_tointeger(L, 3);
    double lacunarity = lua_tonumber(L, 4);
    double gain = lua_tonumber(L, 5);
    double result = fractalNoise(x, y, octaves, lacunarity, gain);
    lua_pushnumber(L, result);
    return 1;
}

static const luaL_reg noise_lib[] = {
    {"noise_2d", noise_noise},
    {"fractal_noise", noise_fractal_noise},
    {0, 0}
};

static void LuaInit(lua_State* L) {
    int top = lua_gettop(L);
    luaL_register(L, "noise", noise_lib);
	lua_pop(L, 1);
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params) {
    init();
    LuaInit(params->m_L);
	dmLogInfo("Registered %s Extension\n", "noise");
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(noise, "noise", 0, 0, InitializeExtension, 0, 0, 0)
