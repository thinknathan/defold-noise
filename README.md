# defold-noise

A defold native extension attempting to implement Open Simplex 2 2d noise and Fractal noise.

Not yet tested.

## Usage

```lua
-- Generate a single noise value
local x = 0.5
local y = 0.8
local singleValue = noise.noise_2d(x, y)
print("Single noise value: ", singleValue)

-- Generate fractal noise with multiple octaves
local octaves = 4
local lacunarity = 2.0
local gain = 0.5
local fractalValue = noise.fractal_noise(x, y, octaves, lacunarity, gain)
print("Fractal noise value: ", fractalValue)
```

## Background

Created with Chat-GPT. Prompt:

```
create a defold extension that exposes a module to lua, use c++ that does not use any features newer than 2009, and does not use the standard library, and always uses const char* instead of std::string.

the name of the module is noise.

implement open simplex 2 noise.

use static classes instead of namespaces.

implement fractal noise with 2d simplex noise as its source.
```
