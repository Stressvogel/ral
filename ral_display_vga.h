/*
 * ral_display_vga.c
 *
 * Omschrijving:	Driver voor een VGA-monitor
 * Hoofdauteur:		Aran Kieskamp
 *
 * Project Stressvogel
 * Computer Engineering
 * Windesheim, 2021-2022
 */

#ifndef SOFTWARE_VGA_H
#define SOFTWARE_VGA_H

#include <altera_up_avalon_video_dma_controller.h>
#include <cstdio>
#include <system.h>
#include <stdint.h>

#include "ral_display.h"

namespace ral {

/**
 * RAL-implementatie voor het aansturen van een beeldscherm via VGA.<br />
 * <br />
 * Het maakt gebruik van de UP Video drivers.
 **/
class display_vga : public display {
protected:
	alt_up_video_dma_dev *video_dev;

public:
	display_vga();

	virtual ~display_vga();

	/**
	 * @inheritDoc
	 **/
	void init();

	/**
	 * @inheritDoc
	 **/
	void draw_pixel(uint16_t x, uint8_t y, uint16_t color);

	/**
	 * @inheritDoc
	 **/
	void draw_box(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color, bool fill = true);

	/**
	 * @inheritDoc
	 **/
	void draw_sprite(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t ***sprite);

	/**
	 * @inheritDoc
	 **/
	void clear();

	/**
	 * @inheritDoc
	 **/
	uint16_t get_width();

	/**
	 * @inheritDoc
	 **/
	uint8_t get_height();
};

} // namespace ral

#endif //SOFTWARE_VGA_H

