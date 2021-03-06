/*
 * ral_display.h
 *
 * Omschrijving:	Abstracte view van een beeldscherm
 * Hoofdauteur:		Matthijs Bakker
 *
 * Project Stressvogel
 * Computer Engineering
 * Windesheim, 2021-2022
 */

#ifndef SOFTWARE_RAL_DISPLAY_H
#define SOFTWARE_RAL_DISPLAY_H

#include <cstdint>

namespace ral {

class display {
protected:
	/**
	 * De breedte van het scherm in pixels
	 **/
	uint16_t width;

	/**
	 * De hoogte van het scherm in pixels
	 **/
	uint8_t height;

public:
	/**
	 * Init functie.
	 * Wordt aangeroepen zodra de game wordt geinitialiseerd.
	 **/
	virtual void init() = 0;

	/**
	 * Set de kleur van de pixel op X,Y
	 *
	 * @param x		X-coordinaat
	 * @param y		Y-coordinaat
	 * @param color		5-6-5 RGB code in een 16-bit integer
	 **/
	virtual void draw_pixel(uint16_t x, uint8_t y, uint16_t color) = 0;

	/**
	 * Teken een vierkant
	 *
	 * @param x		X-coordinaat
	 * @param y		Y-coordinaat
	 * @param width		Breedte
	 * @param height	Hoogte
	 * @param color		5-6-5 RGB code in een 16-bit integer
	 **/
	virtual void draw_box(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color, bool fill = true) = 0;

	/**
	 * Teken een sprite
	 *
	 * @param x		X-coordinaat
	 * @param y		Y-coordinaat
	 * @param width		Breedte
	 * @param height	Hoogte
	 * @param sprite	Pointer naar 2d array met de pixel data van de sprite
	 **/
	virtual void draw_sprite(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t ***sprite) = 0;

	/**
	 * Clear screen
	 **/
	virtual void clear() = 0;

	/**
	 * Verkrijg de breedte van het scherm in pixels
	 **/
	virtual uint16_t get_width() = 0;

	/**
	 * Verkrijg de hoogte van het scherm in pixels
	 **/
	virtual uint8_t get_height() = 0;

};

}

#endif //SOFTWARE_RAL_DISPLAY_H

