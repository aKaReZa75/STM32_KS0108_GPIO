/**
 ******************************************************************************
 * @file     glcd.h
 * @brief    Graphical LCD (128x64) display driver header
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This header provides complete interface for 128x64 graphical LCD
 *           with KS0108 compatible controller, supporting text, graphics,
 *           and custom bitmap display functions.
 * 
 ******************************************************************************
 */
#ifndef _GLCD_H_
#define _GLCD_H_

#include "aKaReZa.h"

/* ============================================================================
 *                    CRITICAL DEPENDENCY CHECK
 * ============================================================================
 *  This library requires the aKaReZa.h base library to compile correctly.
 *  If the file is missing, please download it or contact for support.
 * ============================================================================ */
#ifndef _aKaReZa_H_
    #warning "============================================================"
    #warning " [WARNING] Missing required dependency: aKaReZa.h"
    #warning "------------------------------------------------------------"
    #warning "  This module depends on the aKaReZa.h base library."
    #warning "  Please download it from: https://github.com/aKaReZa75/STM32_RawProject_HAL"
    #warning "  Or contact for support: akaReza75@gmail.com"
    #warning "------------------------------------------------------------"
    #error   "Compilation aborted: Required file 'aKaReZa.h' not found!"
    #warning "============================================================"
#endif

/* ============================================================================
 *                         GLCD CONFIGURATION CONSTANTS
 * ============================================================================ */

#define __GLCD_ChipSelectType 			 HIGH    /**< Chip select type: LOW = active low cs, HIGH = active high cs */
#define __GLCD_Dlay_Factor    			 1       /**< Delay multiplier factor for timing adjustments */
#define __GLCD_Init_Delay            1000    /**< Initialization delay in microseconds */

/* ============================================================================
 *                         GLCD HARDWARE SPECIFICATIONS
 * ============================================================================ */

#define __GLCD_WIDTH 				  			 128     /**< GLCD width in pixels (128 columns) */
#define __GLCD_HEIGHT  			  			 64      /**< GLCD height in pixels (64 rows) */

/* ============================================================================
 *                         GLCD COMMAND DEFINITIONS
 * ============================================================================ */

#define __glcd_writeCmd       			 0       /**< Command mode for RS pin */
#define __glcd_writeData      	  	 1       /**< Data mode for RS pin */
#define __glcd_DISPLAY_ON     			 0x3F    /**< Command to turn display ON */
#define __glcd_DISPLAY_Start  			 0xC0    /**< Command to set display start line */
#define __glcd_DISPLAY_OFF    			 0x3E    /**< Command to turn display OFF */
#define __glcd_Command_Set_Address	 0x40    /**< Command to set column address (Y address) */
#define __glcd_Command_Set_Page	   	 0xB8    /**< Command to set page address (X address) */
#define __glcd_Command_Display_Start 0xC0    /**< Command to set display start line */

/* ============================================================================
 *                         FUNCTION PROTOTYPES
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Initialize GLCD with proper reset sequence
 * @retval None
 * ------------------------------------------------------- */
void glcd_Init(void);

/* -------------------------------------------------------
 * @brief Move cursor to pixel coordinates (X:0..127, Y:0..63)
 * @param x Pixel X coordinate (0..127)
 * @param y Pixel Y coordinate (0..63)
 * @retval None
 * ------------------------------------------------------- */
void glcd_gotoxy(uint8_t x, uint8_t y);

/* -------------------------------------------------------
 * @brief Move cursor by column/row index (based on 6x8 font)
 * @param c Column index (0..21) -> each character 5+1 pixels wide
 * @param r Row index (0..7) -> each character 8 pixels tall
 * @retval None
 * ------------------------------------------------------- */
void glcd_gotocr(uint8_t c, uint8_t r);

/* -------------------------------------------------------
 * @brief Set column (X address) in GLCD
 * @param x Column index (0..127)
 * @retval None
 * @note Internal function - handles chip selection automatically
 * ------------------------------------------------------- */
void goto_col(uint8_t x);

/* -------------------------------------------------------
 * @brief Set row (page address) in GLCD
 * @param y Page index (0..7)
 * @retval None
 * @note Internal function
 * ------------------------------------------------------- */
void goto_row(uint8_t y);

/* -------------------------------------------------------
 * @brief Write data or command to GLCD
 * @param _glcd_value Data/command byte to write
 * @param _glcd_cmdData false=command, true=data
 * @retval None
 * @note Low-level function for GLCD communication
 * ------------------------------------------------------- */
void glcd_write(uint8_t _glcd_value, bool _glcd_cmdData);

/* -------------------------------------------------------
 * @brief Clear specific line on GLCD
 * @param ln Line number to clear (0..7)
 * @retval None
 * ------------------------------------------------------- */
void glcd_clrln(uint8_t ln);

/* -------------------------------------------------------
 * @brief Clear entire GLCD display
 * @retval None
 * ------------------------------------------------------- */
void glcd_clear(void);

/* -------------------------------------------------------
 * @brief Display single character at current cursor position
 * @param c ASCII character to display
 * @retval None
 * ------------------------------------------------------- */
void glcd_putc(uint8_t c);

/* -------------------------------------------------------
 * @brief Display null-terminated string at current cursor position
 * @param s Null-terminated string to display
 * @retval None
 * ------------------------------------------------------- */
void glcd_puts(const char *s);

/* -------------------------------------------------------
 * @brief Display single character at current cursor position (inverted)
 * @param c ASCII character to display
 * @retval None
 * ------------------------------------------------------- */
void glcd_putc_inv(uint8_t c);

/* -------------------------------------------------------
 * @brief Display inverted string at current cursor position
 * @param s Null-terminated string to display
 * @retval None
 * ------------------------------------------------------- */
void glcd_puts_inv(const char *s);

/* -------------------------------------------------------
 * @brief Display bitmap image on entire GLCD
 * @param glcd_pic Pointer to 1024-byte bitmap data
 * @retval None
 * ------------------------------------------------------- */
void glcd_pic(const uint8_t *glcd_pic);

/* -------------------------------------------------------
 * @brief Draw 128-byte buffer as a graph on GLCD
 * @param buffer Pointer to 128-byte array (each value 0-63)
 * @retval None
 * @note Each byte represents height, graph drawn from bottom up
 * ------------------------------------------------------- */
void glcd_draw_graph(const uint8_t *buffer);

/* -------------------------------------------------------
 * @brief Print animated dots on GLCD at given position
 * @param c Column (X coordinate)
 * @param r Row (Y coordinate) 
 * @param maxDots Number of dots (e.g. 3)
 * @retval None
 * ------------------------------------------------------- */
void glcd_dots(uint8_t c, uint8_t r, uint8_t maxDots);

/* -------------------------------------------------------
 * @brief Draw progress bar on GLCD with percentage
 * @param start_col Starting column (0..127)
 * @param end_col Ending column (0..127)
 * @param row Row index (0..7)
 * @param percentage Progress percentage (0-100)
 * @retval None
 * ------------------------------------------------------- */
void glcd_progressbar(uint8_t start_col, uint8_t end_col, uint8_t row, uint8_t percentage);

/* -------------------------------------------------------
 * @brief Draw 128-byte buffer as a graph on GLCD, with Min/Max reference lines
 * @param buffer  Pointer to 128-byte array (each value 0-63)
 * @param min_val Minimum reference level (0-63) -> drawn as dashed horizontal line
 * @param max_val Maximum reference level (0-63) -> drawn as dashed horizontal line
 * @retval None
 * @note Values above 63 are clamped to 63. Min/Max lines are drawn dashed
 *       so they remain visually distinguishable from the graph data itself.
 * ------------------------------------------------------- */
void glcd_draw_graph_minmax(const uint8_t *buffer, uint8_t min_val, uint8_t max_val);

#endif /* _GLCD_H_ */