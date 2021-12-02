/*
 * ral.h
 *
 *  Created on: Dec 2, 2021
 *      Author: Mark Rutte
 */

#include <cstdint>

#ifndef SV_RAL_H_
#define SV_RAL_H_

/*
 * Init functie.
 * Wordt aangeroepen zodra de game wordt geinitialiseerd.
 */
void ral_init();

/*
 * Set de kleur van de pixel op X,Y
 *
 * @param x       X-coordinaat
 * @param y       Y-coordinaat
 * @param color   5-6-5 RGB code in een 16-bit integer
 */
void ral_draw_pixel(uint8_t x, uint8_t y, uint16_t color);

/*
 * Teken een vierkant
 *
 * @param x       X-coordinaat
 * @param y       Y-coordinaat
 * @param width   Breedte
 * @param height  Hoogte
 * @param color   5-6-5 RGB code in een 16-bit integer
 */
void ral_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);

/*
 * Teken een sprite
 *
 * @param x       X-coordinaat
 * @param y       Y-coordinaat
 * @param width   Breedte
 * @param height  Hoogte
 * @param sprite  Pointer naar 2d array met de pixel data van de sprite
 */
void ral_draw_sprite(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t ***sprite);

/*
 * Clear screen
 */
void ral_clear();

#endif /* SV_RAL_H_ */
