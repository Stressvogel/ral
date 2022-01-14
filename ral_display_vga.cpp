/*
 * ral_display_vga.cpp
 *
 * Omschrijving:	Driver voor een VGA-monitor
 * Hoofdauteur:		Aran Kieskamp
 *
 * Project Stressvogel
 * Computer Engineering
 * Windesheim, 2021-2022
 */

#include "ral_display_vga.h"

/**
 * Kleurcode voor transparante pixels
 * (er wordt dus niets getekend als een pixel deze "kleur" heeft.)
 **/
#define COLOR_TRANSPARENCY				(0xDEAD)

/**
 * Achtergrondkleur van het beeld.
 **/
#define COLOR_BACKGROUND				(0x4E19)

/**
 * Adres van de SRAM die voor de pixel buffers gebruikt kan worden
 **/
// Deze staat niet in system.h for some reason...
// BSP regeneraten helpt ook niet
#define VGA_SUBSYSTEM_VGA_SRAM_BASE			(0x08000000)

/**
 * Het register van de pixel buffer controller waar de width en height van het scherm in staan.
 */
#define VGA_SUBSYSTEM_VGA_PIXEL_DMA_RESOLUTION_REG	(VGA_SUBSYSTEM_VGA_PIXEL_DMA_BASE + 0x8)

/**
 * De standaard breedte van het beeld in pixels.<br />
 * <br />
 * Meestal wordt dit door de VGA controller geupscaled (320 -> 640, etc.)
 **/
#define DEFAULT_SCREEN_WIDTH				(320)
/**
 * De standaard hoogte van het beeld in pixels.<br />
 * <br />
 * Meestal wordt dit door de VGA controller geupscaled (240 -> 480, etc.)
 **/
#define DEFAULT_SCREEN_HEIGHT				(240)

/**
 * Hoeveel bytes er gereserveerd staan voor 1 pixel.<br />
 * <br/>
 * Voor 1 color zijn 2 bytes nodig (16 bit kleuren) plus 2 reserve bytes
 **/
#define BYTES_PER_PIXEL					(sizeof(uint16_t) * 2)

/**
 * In alle draw commando's van de DMA controller moet je een argument meegeven
 * of je op de current buffer (0) of de back buffer (1) wil tekenen.<br />
 * <br />
 * Dit is simpelweg een macro voor "1", om de code leesbaarder te maken.
 **/
#define USE_BACK_BUFFER					(1)

namespace ral {

/**
 * @inheritDoc
 **/
display_vga::display_vga() {
	this-> width = DEFAULT_SCREEN_WIDTH;
	this-> height = DEFAULT_SCREEN_HEIGHT;
}

/**
 * @inheritDoc
 **/
display_vga::~display_vga() {
	delete this->video_dev;
	this->video_dev = nullptr;
}

/**
 * @inheritDoc
 **/
void display_vga::init() {
	// Controleer dat de schermresolutie gelijk is aan wat wij hebben ingesteld
	volatile int *video_resolution = (int *) VGA_SUBSYSTEM_VGA_PIXEL_DMA_RESOLUTION_REG;
	// Dit register bevat in de laatste 16 bits de breedte van het scherm in pixels
	// en in de hoogste 8 bits de hoogte van het scherm in pixels.
	uint16_t real_width = *video_resolution & 0xFFFF; // verkrijg de laagste 16 pixels
	uint8_t real_height = (*video_resolution >> 16) & 0xFF; // verkrijg de hoogste 8 pixels
	// Als de standaardbreedte niet klopt, pas het dan aan aan de breedte van het scherm
	if (this->width != real_width) {
		printf("Schermbreedte (%d) komt niet overeen met de standaard value (%d)", real_width, this->width);
		this->width = real_width;
	}
	// Als de standaardhoogte niet klopt, pas het dan aan aan de hoogte van het scherm
	if (this->height != real_height) {
		printf("Schermhoogte (%d) komt niet overeen met de standaard value (%d)", real_height, this->height);
		this->height = real_height;
	}

	// Open de pixel buffer controller
	this->video_dev = alt_up_video_dma_open_dev(VGA_SUBSYSTEM_VGA_PIXEL_DMA_NAME);

	// Check of het openen van de pixel buffer controller is gelukt
	if (this->video_dev == NULL) {
		printf("Kan de pixel buffer controller niet openen");
		return;
	}

	// De pixel buffers zetten wij op deze plaatsen in het SRAM:
	// Let er op: als this->width erg groot is, kunnen we buiten de SRAM komen...
	volatile int front_buf_addr = VGA_SUBSYSTEM_VGA_SRAM_BASE + (this->width * this->height * BYTES_PER_PIXEL);
	volatile int back_buf_addr = front_buf_addr + (this->width * this->height * BYTES_PER_PIXEL);

	// Zet het adres van de front buffer in het video register
	alt_up_video_dma_ctrl_set_bb_addr(this->video_dev, front_buf_addr);
	// Swap naar de backbuffer
	alt_up_video_dma_ctrl_swap_buffers(this->video_dev);
	// Wacht totdat de swap voltooid is
	while (alt_up_video_dma_ctrl_check_swap_status(this->video_dev));
	// Zet het adres van de back buffer in het video register
	alt_up_video_dma_ctrl_set_bb_addr(this->video_dev, back_buf_addr);
	// Swap terug naar de front buffer
	alt_up_video_dma_ctrl_swap_buffers(this->video_dev);

	// Vanaf nu doen we alle instructies (ook in de andere [ral_*] functies) op de backbuffer!!!
	// Clear het scherm
	alt_up_video_dma_screen_fill(this->video_dev, COLOR_BACKGROUND, USE_BACK_BUFFER);
}

/**
 * @inheritDoc
 **/
void display_vga::draw_pixel(uint16_t x, uint8_t y, uint16_t color) {
	alt_up_video_dma_draw(this->video_dev, color, x, y, USE_BACK_BUFFER);
}

/**
 * @inheritDoc
 **/
void display_vga::draw_box(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color, bool fill) {
	// De width en height in de UP methode zijn INCLUSIVE, dus verlaag beide values met 1.
	alt_up_video_dma_draw_box(this->video_dev, color, x, y, x + width - 1, y + height - 1, USE_BACK_BUFFER, fill ? 1 : 0);
}

/**
 * @inheritDoc
 **/
void display_vga::draw_sprite(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t ***sprite) {
	// Dereference de pointer naar de 2d sprite data array
	uint16_t **spriteData = *sprite;

	// Teken elke pixel van de sprite
	for (int i = 0; i < this->height; ++i) {
		for (int j = 0; j < this->width; ++j) {
			if (spriteData[i][j] == COLOR_TRANSPARENCY) continue;
			this->draw_pixel(x + j, y + i, spriteData[i][j]);
		}
	}
}

/**
 * @inheritDoc
 **/
void display_vga::clear() {
	// Breng de backbuffer waar we net op getekend hebben naar de voorgrond
	alt_up_video_dma_ctrl_swap_buffers(this->video_dev);

	// Wacht totdat de buffer swap is voltooid
	while (alt_up_video_dma_ctrl_check_swap_status(this->video_dev));

	// Clear de backbuffer zodat we de volgende keer er weer op kunnen tekenen
	alt_up_video_dma_screen_fill(this->video_dev, COLOR_BACKGROUND, USE_BACK_BUFFER);
}

/**
 * @inheritDoc
 **/
uint16_t display_vga::get_width() {
	return this->width;
}

/**
 * @inheritDoc
 **/
uint8_t display_vga::get_height() {
	return this->height;
}

} // namespace ral

