/** @noSelfInFile */

/**
 * A native extension for Defold for generating noise.
 * @noResolution
 */
declare namespace noise {
	export function noise_2d(x: number, y: number): number;
	export function fractal_noise(
		x: number,
		y: number,
		octaves: number,
		lacunarity: number,
		gain: number,
	): number;
}
