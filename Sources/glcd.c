/**
 ******************************************************************************
 * @file     glcd.c
 * @brief    Graphical LCD (128x64) display driver implementation (KS0108)
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This library provides complete driver for 128x64 graphical LCD
 *           with KS0108 compatible controller, supporting text, graphics,
 *           and custom bitmap display functions.
 * 
 * @note     FUNCTION SUMMARY:
 *           Initialization & Control:
 *           - glcd_Init : Initialize GLCD with proper reset sequence
 *           - glcd_clear: Clear entire GLCD display
 *           - glcd_clrln: Clear specific line on GLCD
 *           - glcd_write: Low-level write data/command to GLCD
 *
 *           Cursor & Positioning:
 *           - glcd_gotoxy: Move cursor to pixel coordinates
 *           - glcd_gotocr: Move cursor by column/row index
 *           - goto_col: Set column address in GLCD
 *           - goto_row: Set row (page) address in GLCD
 *
 *           Text Display:
 *           - glcd_putc: Display single character at current position
 *           - glcd_puts: Display null-terminated string
 *           - glcd_putc_inv: Display inverted character
 *           - glcd_puts_inv: Display inverted null-terminated string
 *
 *           Graphics & Special Functions:
 *           - glcd_pic: Display bitmap image on entire GLCD (128x64)
 *           - glcd_draw_graph : Draw 128-byte buffer as graph (Plotting)
 *           - glcd_progressbar: Draw progress bar with percentage
 *           - glcd_dots: Print animated dots for loading indication
 *
 *           Utility Functions:
 *           - delay_us: Microsecond delay function using SysTick
 ******************************************************************************
 */
#include "glcd.h"


/* ============================================================================
 *                         GLCD 5x8 Font
 * ============================================================================ */
const uint8_t font[128 * 5] = {
    // Characters 0-31 (custom and control characters)
    0x00,0x00,0x00,0x00,0x00, /**< 0 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 1 - Empty */ 
    0x00,0x00,0x00,0x00,0x00, /**< 2 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 3 - Empty */    
    0x00,0x00,0x00,0x00,0x00, /**< 4 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 5 - Empty */ 
    0x00,0x00,0x00,0x00,0x00, /**< 6 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 7 - Empty */ 
    0x00,0x00,0x00,0x00,0x00, /**< 8 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 9 - Empty */ 
    0x00,0x00,0x00,0x00,0x00, /**< 10 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 11 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 12 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 13 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 14 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 15 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 16 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 17 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 18 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 19 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 20 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 21 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 22 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 23 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 24 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 25 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 26 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 27 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 28 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 29 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 30 - Empty */
    0x00,0x00,0x00,0x00,0x00, /**< 31 - Empty */

    // Printable ASCII characters from 32 (' ') to 127 ('~')
    0x00,0x00,0x00,0x00,0x00, /**< 32 (' ') */
    0x00,0x00,0x5F,0x00,0x00, /**< 33 ('!') */
    0x00,0x07,0x00,0x07,0x00, /**< 34 ('"') */
    0x14,0x7F,0x14,0x7F,0x14, /**< 35 ('#') */
    0x24,0x2A,0x7F,0x2A,0x12, /**< 36 ('$') */
    0x23,0x13,0x08,0x64,0x62, /**< 37 ('%') */
    0x36,0x49,0x56,0x20,0x50, /**< 38 ('&') */
    0x00,0x05,0x03,0x00,0x00, /**< 39 (''') */
    0x00,0x1C,0x22,0x41,0x00, /**< 40 ('(') */
    0x00,0x41,0x22,0x1C,0x00, /**< 41 (')') */
    0x14,0x08,0x3E,0x08,0x14, /**< 42 ('*') */
    0x08,0x08,0x3E,0x08,0x08, /**< 43 ('+') */
    0x00,0x50,0x30,0x00,0x00, /**< 44 (',') */
    0x08,0x08,0x08,0x08,0x08, /**< 45 ('-') */
    0x00,0x60,0x60,0x00,0x00, /**< 46 ('.') */
    0x20,0x10,0x08,0x04,0x02, /**< 47 ('/') */
    0x3E,0x51,0x49,0x45,0x3E, /**< 48 ('0') */
    0x00,0x42,0x7F,0x40,0x00, /**< 49 ('1') */
    0x62,0x51,0x49,0x49,0x46, /**< 50 ('2') */
    0x21,0x41,0x49,0x4D,0x33, /**< 51 ('3') */
    0x18,0x14,0x12,0x7F,0x10, /**< 52 ('4') */
    0x27,0x45,0x45,0x45,0x39, /**< 53 ('5') */
    0x3C,0x4A,0x49,0x49,0x31, /**< 54 ('6') */
    0x01,0x71,0x09,0x05,0x03, /**< 55 ('7') */
    0x36,0x49,0x49,0x49,0x36, /**< 56 ('8') */
    0x46,0x49,0x49,0x29,0x1E, /**< 57 ('9') */
    0x00,0x36,0x36,0x00,0x00, /**< 58 (':') */
    0x00,0x56,0x36,0x00,0x00, /**< 59 (';') */
    0x08,0x14,0x22,0x41,0x00, /**< 60 ('<') */
    0x14,0x14,0x14,0x14,0x14, /**< 61 ('=') */
    0x00,0x41,0x22,0x14,0x08, /**< 62 ('>') */
    0x02,0x01,0x51,0x09,0x06, /**< 63 ('?') */
    0x32,0x49,0x79,0x41,0x3E, /**< 64 ('@') */
    0x7C,0x12,0x11,0x12,0x7C, /**< 65 ('A') */
    0x7F,0x49,0x49,0x49,0x36, /**< 66 ('B') */
    0x3E,0x41,0x41,0x41,0x22, /**< 67 ('C') */
    0x7F,0x41,0x41,0x41,0x3E, /**< 68 ('D') */
    0x7F,0x49,0x49,0x49,0x41, /**< 69 ('E') */
    0x7F,0x09,0x09,0x09,0x01, /**< 70 ('F') */
    0x3E,0x41,0x41,0x51,0x73, /**< 71 ('G') */
    0x7F,0x08,0x08,0x08,0x7F, /**< 72 ('H') */
    0x00,0x41,0x7F,0x41,0x00, /**< 73 ('I') */
    0x20,0x40,0x41,0x3F,0x01, /**< 74 ('J') */
    0x7F,0x08,0x14,0x22,0x41, /**< 75 ('K') */
    0x7F,0x40,0x40,0x40,0x40, /**< 76 ('L') */
    0x7F,0x02,0x1C,0x02,0x7F, /**< 77 ('M') */
    0x7F,0x04,0x08,0x10,0x7F, /**< 78 ('N') */
    0x3E,0x41,0x41,0x41,0x3E, /**< 79 ('O') */
    0x7F,0x09,0x09,0x09,0x06, /**< 80 ('P') */
    0x3E,0x41,0x51,0x21,0x5E, /**< 81 ('Q') */
    0x7F,0x09,0x19,0x29,0x46, /**< 82 ('R') */
    0x26,0x49,0x49,0x49,0x32, /**< 83 ('S') */
    0x03,0x01,0x7F,0x01,0x03, /**< 84 ('T') */
    0x3F,0x40,0x40,0x40,0x3F, /**< 85 ('U') */
    0x1F,0x20,0x40,0x20,0x1F, /**< 86 ('V') */
    0x3F,0x40,0x38,0x40,0x3F, /**< 87 ('W') */
    0x63,0x14,0x08,0x14,0x63, /**< 88 ('X') */
    0x03,0x04,0x78,0x04,0x03, /**< 89 ('Y') */
    0x61,0x51,0x49,0x45,0x43, /**< 90 ('Z') */
    0x00,0x7F,0x41,0x41,0x00, /**< 91 ('[') */
    0x02,0x04,0x08,0x10,0x20, /**< 92 ('\') */
    0x00,0x41,0x41,0x7F,0x00, /**< 93 (']') */
    0x04,0x02,0x01,0x02,0x04, /**< 94 ('^') */
    0x40,0x40,0x40,0x40,0x40, /**< 95 ('_') */
    0x00,0x03,0x05,0x00,0x00, /**< 96 ('`') */
    0x20,0x54,0x54,0x78,0x40, /**< 97 ('a') */
    0x7F,0x28,0x44,0x44,0x38, /**< 98 ('b') */
    0x38,0x44,0x44,0x44,0x00, /**< 99 ('c') */
    0x38,0x44,0x44,0x28,0x7F, /**< 100 ('d') */
    0x38,0x54,0x54,0x54,0x18, /**< 101 ('e') */
    0x00,0x08,0x7E,0x09,0x02, /**< 102 ('f') */
    0x18,0xA4,0xA4,0x9C,0x78, /**< 103 ('g') */
    0x7F,0x08,0x04,0x04,0x78, /**< 104 ('h') */
    0x00,0x44,0x7D,0x40,0x00, /**< 105 ('i') */
    0x20,0x40,0x40,0x3D,0x00, /**< 106 ('j') */
    0x7F,0x10,0x28,0x44,0x00, /**< 107 ('k') */
    0x00,0x41,0x7F,0x40,0x00, /**< 108 ('l') */
    0x7C,0x04,0x78,0x04,0x78, /**< 109 ('m') */
    0x7C,0x08,0x04,0x04,0x78, /**< 110 ('n') */
    0x38,0x44,0x44,0x44,0x38, /**< 111 ('o') */
    0xFC,0x18,0x24,0x24,0x18, /**< 112 ('p') */
    0x18,0x24,0x24,0x18,0xFC, /**< 113 ('q') */
    0x7C,0x08,0x04,0x04,0x08, /**< 114 ('r') */
    0x48,0x54,0x54,0x54,0x24, /**< 115 ('s') */
    0x04,0x04,0x3F,0x44,0x24, /**< 116 ('t') */
    0x3C,0x40,0x40,0x20,0x7C, /**< 117 ('u') */
    0x1C,0x20,0x40,0x20,0x1C, /**< 118 ('v') */
    0x3C,0x40,0x30,0x40,0x3C, /**< 119 ('w') */
    0x44,0x28,0x10,0x28,0x44, /**< 120 ('x') */
    0x4C,0x90,0x90,0x90,0x7C, /**< 121 ('y') */
    0x44,0x64,0x54,0x4C,0x44, /**< 122 ('z') */
    0x00,0x08,0x36,0x41,0x00, /**< 123 ('{') */
    0x00,0x00,0x77,0x00,0x00, /**< 124 ('|') */
    0x00,0x41,0x36,0x08,0x00, /**< 125 ('}') */
    0x02,0x01,0x02,0x04,0x02, /**< 126 ('~') */
    0x3C,0x26,0x23,0x26,0x3C  /**< 127 (DEL) */
};

/* ============================================================================
 *                         GLOBAL VARIABLES
 * ============================================================================ */
static uint8_t cursorX = 0;        /**< Current cursor X position (0-127) */
static uint8_t cursorY = 0;        /**< Current cursor Y position (pages 0-7) */

/* ============================================================================
 *                         GLCD Initialize
 * ============================================================================ */
/* -------------------------------------------------------
 * @brief Initialize GLCD with proper reset sequence
 * ------------------------------------------------------- */
void glcd_Init(void)
{   
    // Initialize control pins to safe state
    HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, GPIO_PIN_RESET);  /**< Disable chip 1 */
    HAL_GPIO_WritePin(GLCD_CS2_GPIO_Port, GLCD_CS2_Pin, GPIO_PIN_RESET);  /**< Disable chip 2 */
    HAL_GPIO_WritePin(GLCD_EN_GPIO_Port, GLCD_EN_Pin, GPIO_PIN_RESET);    /**< Disable enable */
    HAL_GPIO_WritePin(GLCD_RS_GPIO_Port, GLCD_RS_Pin, GPIO_PIN_RESET);    /**< Set to command mode */
    
    // Reset sequence - Critical for proper initialization
		/*
    HAL_GPIO_WritePin(GLCD_RST_GPIO_Port, GLCD_RST_Pin, GPIO_PIN_RESET);
    delay_us(1000);  // 1ms delay before reset release
    HAL_GPIO_WritePin(GLCD_RST_GPIO_Port, GLCD_RST_Pin, GPIO_PIN_SET);
    delay_us(10000); // 10ms delay after reset release
    */

    // Initialize left chip (CS1)
    HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, GPIO_PIN_SET);    /**< Enable left chip */
    HAL_GPIO_WritePin(GLCD_CS2_GPIO_Port, GLCD_CS2_Pin, GPIO_PIN_RESET);  /**< Disable right chip */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for stabilization */
    glcd_write(__glcd_DISPLAY_ON, __glcd_writeCmd);                       /**< Turn display ON */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for command */
    glcd_write(__glcd_DISPLAY_Start, __glcd_writeCmd);                    /**< Set display start line */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for command */
    
    // Initialize right chip (CS2)
    HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, GPIO_PIN_RESET);  /**< Disable left chip */
    HAL_GPIO_WritePin(GLCD_CS2_GPIO_Port, GLCD_CS2_Pin, GPIO_PIN_SET);    /**< Enable right chip */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for stabilization */
    glcd_write(__glcd_DISPLAY_ON, __glcd_writeCmd);                       /**< Turn display ON */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for command */
    glcd_write(__glcd_DISPLAY_Start, __glcd_writeCmd);                    /**< Set display start line */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for command */
    
    // Clear entire display
    glcd_clear();                                                         /**< Clear all pixels */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for clear operation */
    
    // Reset cursor position
    cursorX = 0;                                                          /**< Reset X to left */
    cursorY = 0;                                                          /**< Reset Y to top */    
    glcd_gotoxy(0, 0);                                                    /**< Set cursor to home */
    delay_us(__GLCD_Init_Delay);                                          /**< Wait for positioning */
};


/* ============================================================================
 *                         CURSOR POSITIONING FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Move cursor to pixel coordinates (X:0..127, Y:0..63)
 * @param x Pixel X coordinate (0..127)
 * @param y Pixel Y coordinate (0..63)
 * ------------------------------------------------------- */
void glcd_gotoxy(uint8_t x, uint8_t y)
{
    if (x >= __GLCD_WIDTH)  x = __GLCD_WIDTH - 1;         /**< Clamp X to maximum width */
    if (y >= __GLCD_HEIGHT) y = __GLCD_HEIGHT - 1;        /**< Clamp Y to maximum height */

    cursorX = x;                                          /**< Update cursor X position */
    cursorY = y / 8;                                      /**< Convert pixel Y to page (0-7) */
    goto_col(cursorX);                                    /**< Set column address */
    goto_row(cursorY);                                    /**< Set row (page) address */
};


/* -------------------------------------------------------
 * @brief Move cursor by column/row index (based on 5x8 font)
 * @param c Column index (0..21)  -> each character 5 pixels wide + 1 wide sapce between characters (128/6=21)
 * @param r Row index (0..7)      -> each character 8 pixels tall
 * ------------------------------------------------------- */
void glcd_gotocr(uint8_t c, uint8_t r)
{
    if (c >= (__GLCD_WIDTH / 6)) c = (__GLCD_WIDTH / 6) - 1;  /**< Clamp column to max characters per row */
    if (r >= (__GLCD_HEIGHT / 8)) r = (__GLCD_HEIGHT / 8) - 1;/**< Clamp row to max rows */
    cursorX = c * 6;                                          /**< Convert character index to pixel X */
    cursorY = r;                                              /**< Set page (row) */
    goto_col(cursorX);                                        /**< Set column address */
    goto_row(cursorY);                                        /**< Set row (page) address */
};


/* -------------------------------------------------------
 * @brief Set column (X address) in GLCD - FIXED VERSION
 * @param x Column index (0..127)
 * ------------------------------------------------------- */
void goto_col(uint8_t x)
{
    uint8_t chip_x = x;  // Keep original x for chip column address
    
    /* Select left or right chip */
    if (x < 64)  
    {
            /* Left section (CS1) - columns 0-63 */
            HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, GPIO_PIN_SET);    /**< Enable left chip */
            HAL_GPIO_WritePin(GLCD_CS2_GPIO_Port, GLCD_CS2_Pin, GPIO_PIN_RESET);  /**< Disable right chip */
            // chip_x remains as is (0-63)
    }
    else 
    {
            /* Right section (CS2) - columns 64-127 */
            HAL_GPIO_WritePin(GLCD_CS1_GPIO_Port, GLCD_CS1_Pin, GPIO_PIN_RESET);  /**< Disable left chip */
            HAL_GPIO_WritePin(GLCD_CS2_GPIO_Port, GLCD_CS2_Pin, GPIO_PIN_SET);    /**< Enable right chip */
            chip_x = x - 64;  /**< Normalize to 0-63 range for right chip */
    };

    // Add delay after chip selection
    delay_us(2);                                                            /**< Wait for chip selection */

    /* Send column set command - make sure chip_x is properly bounded */
    glcd_write((chip_x & 0x3F) | __glcd_Command_Set_Address, __glcd_writeCmd);  /**< Set column address */
    delay_us(2); /**< Increased delay for command processing */
};


/* -------------------------------------------------------
 * @brief Set row (page address) in GLCD
 * @param y Page index (0..7)
 * ------------------------------------------------------- */
void goto_row(uint8_t y)
{
    /* Send page set command */
    glcd_write((y & 0x07) | __glcd_Command_Set_Page, __glcd_writeCmd);  /**< Set page address */
    delay_us(2); /**< Increased delay for command processing */
};


/* ============================================================================
 *                         GLCD LOW LEVEL WRITE
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Write data or command to GLCD
 * @param _glcd_value Data/command byte to write
 * @param _glcd_cmdData false=command (or __glcd_writeCmd), true=data (or __glcd_writeData)
 * ------------------------------------------------------- */
void glcd_write(uint8_t _glcd_value, bool _glcd_cmdData)
{
    HAL_GPIO_WritePin(GLCD_RS_GPIO_Port, GLCD_RS_Pin, _glcd_cmdData);  /**< Set command/data mode */
    
    delay_us(1);                                                       /**< Wait for signal stabilization */
    
    /* Set data pins */
    HAL_GPIO_WritePin(GLCD_D0_GPIO_Port, GLCD_D0_Pin, bitCheck(_glcd_value, 0));  /**< Set data bit 0 */
    HAL_GPIO_WritePin(GLCD_D1_GPIO_Port, GLCD_D1_Pin, bitCheck(_glcd_value, 1));  /**< Set data bit 1 */
    HAL_GPIO_WritePin(GLCD_D2_GPIO_Port, GLCD_D2_Pin, bitCheck(_glcd_value, 2));  /**< Set data bit 2 */
    HAL_GPIO_WritePin(GLCD_D3_GPIO_Port, GLCD_D3_Pin, bitCheck(_glcd_value, 3));  /**< Set data bit 3 */
    HAL_GPIO_WritePin(GLCD_D4_GPIO_Port, GLCD_D4_Pin, bitCheck(_glcd_value, 4));  /**< Set data bit 4 */
    HAL_GPIO_WritePin(GLCD_D5_GPIO_Port, GLCD_D5_Pin, bitCheck(_glcd_value, 5));  /**< Set data bit 5 */
    HAL_GPIO_WritePin(GLCD_D6_GPIO_Port, GLCD_D6_Pin, bitCheck(_glcd_value, 6));  /**< Set data bit 6 */
    HAL_GPIO_WritePin(GLCD_D7_GPIO_Port, GLCD_D7_Pin, bitCheck(_glcd_value, 7));  /**< Set data bit 7 */
    
    delay_us(1);                                                       /**< Wait for data setup */
    
    /* Generate enable pulse (minimum 450ns required) */
    HAL_GPIO_WritePin(GLCD_EN_GPIO_Port, GLCD_EN_Pin, GPIO_PIN_SET);   /**< Enable pulse high */
    delay_us(2);  /**< Longer delay for stable enable pulse */
    HAL_GPIO_WritePin(GLCD_EN_GPIO_Port, GLCD_EN_Pin, GPIO_PIN_RESET); /**< Enable pulse low */
    
    delay_us(1);                                                       /**< Wait for data latch */
};


/* ============================================================================
 *                         GLCD CLEAR FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Clear specific line on GLCD
 * @param ln Line number to clear (0..7)
 * ------------------------------------------------------- */
void glcd_clrln(uint8_t ln)
{
    int i;
    for(i = 0; i < __GLCD_WIDTH; i++)                    /**< Iterate through all columns */
    {
        goto_col(i);                                     /**< Set column address */
        goto_row(ln);                                    /**< Set page address */
        glcd_write(0, __glcd_writeData);                 /**< Clear pixel data */
    };
    cursorX = 0; /**< Reset cursor to start of line */
    cursorY = ln; /**< Set cursor to cleared line */
};


/* -------------------------------------------------------
 * @brief Clear entire GLCD display
 * ------------------------------------------------------- */
void glcd_clear(void)
{
    int page, col;
    
    // Clear both sections of LCD (left and right chips)
    for (page = 0; page < 8; page++)                    /**< Iterate through all pages */
    {
        for (col = 0; col < 128; col++)                 /**< Iterate through all columns */
        {
            goto_col(col);                              /**< Set column address */
            goto_row(page);                             /**< Set page address */
            glcd_write(0, __glcd_writeData);            /**< Clear pixel data */
        };
    };
    
    cursorX = 0;                                        /**< Reset cursor X to left */
    cursorY = 0;                                        /**< Reset cursor Y to top */    
}


/* ============================================================================
 *                         CHARACTER DISPLAY FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Display single character at current cursor position
 * @param c ASCII character to display
 * ------------------------------------------------------- */
void glcd_putc(uint8_t c)
{
    if (c > 127) return;  /**< Only handle ASCII characters */
    
    /* Check if entire character (5 pixels + 1 spacing) would exceed line boundary */
    if (cursorX > (__GLCD_WIDTH - 6)) 
		{
			cursorX = 0;                                  /**< Move to start of next line */
			cursorY++;                                    /**< Increment page */
			if (cursorY >= 8) cursorY = 0;                /**< Wrap around pages */
    };
    
    /* Write 5 columns of character data with proper chip selection for each column */
    for (uint8_t i = 0; i < 5; i++)   
    {
			/* Ensure proper chip selection and positioning for EACH column */
			goto_col(cursorX);      /**< This handles chip selection automatically */
			goto_row(cursorY);      /**< Set the row */
			delay_us(1);            /**< Small delay for stability */
			
			/* Write the font data */
			glcd_write(font[(c * 5) + i], __glcd_writeData);  /**< Write character column */
			cursorX++;                                        /**< Move to next column */
			
			/* Check boundary after each column */
			if (cursorX >= __GLCD_WIDTH) 
			{
				cursorX = 0;                                  /**< Reset to first column */
				cursorY++;                                    /**< Move to next page */
				if (cursorY >= 8) cursorY = 0;                /**< Wrap around pages */
			};
    };
    
    /* Add 1-pixel spacing between characters (if space available) */
    if (cursorX < __GLCD_WIDTH) 
		{
			goto_col(cursorX);      /**< Proper chip selection for spacing column */
			goto_row(cursorY);      /**< Set page */
			delay_us(1);            /**< Small delay for stability */
			glcd_write(0x00, __glcd_writeData);  /**< Write blank spacing pixel */
			cursorX++;              /**< Move to next column */
    };
    
    /* Handle line wrap after spacing */
    if (cursorX >= __GLCD_WIDTH) 
		{
			cursorX = 0;            /**< Reset to first column */
			cursorY++;              /**< Move to next page */
			if (cursorY >= 8) cursorY = 0;  /**< Wrap around pages */
    };
};


/* -------------------------------------------------------
 * @brief Display string at current cursor position
 * @param s Null-terminated string to display
 * ------------------------------------------------------- */
void glcd_puts(const char *s)
{
	while (*s)                                          /**< Loop until null terminator */
	{
		glcd_putc((uint8_t)*s);                         /**< Display each character */
		s++;                                            /**< Move to next character */
	};
};


/* -------------------------------------------------------
 * @brief Display single character at current cursor position (inverted)
 * @param c ASCII character to display
 * ------------------------------------------------------- */
void glcd_putc_inv(uint8_t c)
{
	if (c > 127) return;  /**< Only handle ASCII characters */

	/* Check if entire character (5 pixels + 1 spacing) would exceed line boundary */
	if (cursorX > (__GLCD_WIDTH - 6)) 
	{
		cursorX = 0;                                    /**< Move to start of next line */
		cursorY++;                                      /**< Increment page */
		if (cursorY >= 8) cursorY = 0;                  /**< Wrap around pages */
	};

	/* Write 5 columns of inverted character data */
	for (uint8_t i = 0; i < 5; i++)
	{
		goto_col(cursorX);                              /**< Set column address */
		goto_row(cursorY);                              /**< Set page address */
		delay_us(1);                                    /**< Small delay for stability */

		/* Invert font data using bitwise NOT */
		glcd_write(~font[(c * 5) + i], __glcd_writeData);  /**< Write inverted character column */
		cursorX++;                                      /**< Move to next column */

		if (cursorX >= __GLCD_WIDTH) 
		{
			cursorX = 0;                                /**< Reset to first column */
			cursorY++;                                  /**< Move to next page */
			if (cursorY >= 8) cursorY = 0;              /**< Wrap around pages */
		};
	};

	/* Add 1-pixel spacing (inverted = 0xFF) */
	if (cursorX < __GLCD_WIDTH) 
	{
		goto_col(cursorX);                              /**< Set column address */
		goto_row(cursorY);                              /**< Set page address */
		delay_us(1);                                    /**< Small delay for stability */
		glcd_write(0xFF, __glcd_writeData);             /**< Write inverted spacing pixel */
		cursorX++;                                      /**< Move to next column */
	};

	/* Handle line wrap after spacing */
	if (cursorX >= __GLCD_WIDTH) 
	{
		cursorX = 0;                                    /**< Reset to first column */
		cursorY++;                                      /**< Move to next page */
		if (cursorY >= 8) cursorY = 0;                  /**< Wrap around pages */
	};
};


/* -------------------------------------------------------
 * @brief Display inverted string at current cursor position
 * @param s Null-terminated string to display
 * ------------------------------------------------------- */
void glcd_puts_inv(const char *s)
{
	while (*s)                                          /**< Loop until null terminator */
	{
		glcd_putc_inv((uint8_t)*s);                     /**< Display each inverted character */
		s++;                                            /**< Move to next character */
	};
};


/* ============================================================================
 *                         GRAPHICS DISPLAY FUNCTIONS
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Display bitmap image on entire GLCD
 * @param glcd_pic Pointer to 1024-byte bitmap data
 * ------------------------------------------------------- */
void glcd_pic(const uint8_t *glcd_pic)
{
	int page, col;
	for (page = 0; page < 8; page++)                /**< 8 pages = 64 rows */
	{
		for (col = 0; col < __GLCD_WIDTH; col++)      /**< 128 columns */
		{
			goto_col(col);                              /**< Set column address */
			goto_row(page);                             /**< Set page address */
			glcd_write(glcd_pic[page * __GLCD_WIDTH + col], __glcd_writeData);  /**< Write bitmap data */
		};
	};
};


/* -------------------------------------------------------
 * @brief Draw 128-byte buffer as a graph on GLCD
 * @param buffer Pointer to 128-byte array (each value 0-63)
 * 
 * Each byte in buffer represents height (0-63):
 * - X position: buffer index (0-127) maps to GLCD columns (0-127)
 * - Y position: buffer value (0-63) maps to GLCD height (0-63)
 * - Graph is drawn from bottom up (Y=0 at bottom, Y=63 at top)
 * ------------------------------------------------------- */
void glcd_draw_graph(const uint8_t *buffer)
{
	if (buffer == NULL) return;  /**< Input validation */
	
	glcd_clear();                                     /**< Clear entire display */
	
	/* Draw each point in the buffer */
	for (uint8_t x = 0; x < 128; x++)                 /**< Iterate through all X positions */
	{
		uint8_t height = buffer[x];                     /**< Get height value from buffer */
		if (height > 63) height = 63;                   /**< Clamp to maximum height */
		
		/* Calculate which page and bit position for the dot */
		uint8_t y_pixel = 63 - height;                  /**< Invert Y (0 at bottom, 63 at top) */
		uint8_t page = y_pixel / 8;                     /**< Calculate page (0-7) */
		uint8_t bit_pos = y_pixel % 8;                  /**< Calculate bit position within page (0-7) */
		
		/* Set column and page */
		goto_col(x);                                    /**< Set X coordinate */
		goto_row(page);                                 /**< Set page address */
		delay_us(1);                                    /**< Small delay for stability */
		
		uint8_t dot_pattern = (1 << bit_pos);           /**< Create single pixel pattern */
		
		glcd_write(dot_pattern, __glcd_writeData);      /**< Write dot to display */
	};
};


/* ============================================================================
 *                         ANIMATION FUNCTIONS
 * ============================================================================ */

/**
 * @brief Print animated dots on GLCD at given position
 * @param c        Column (X coordinate)
 * @param r        Row (Y coordinate)
 * @param maxDots  Number of dots (e.g. 3)
 */
void glcd_dots(uint8_t c, uint8_t r, uint8_t maxDots)
{
	static uint8_t dotCounter = 0;                      /**< Animation counter */
	static uint8_t lastC = 0xFF;                        /**< Last column position */
	static uint8_t lastR = 0xFF;                        /**< Last row position */

	/* Reset if called from a different position */
	if (c != lastC || r != lastR)                     
	{
		dotCounter = 0;                                 /**< Reset animation counter */
		lastC = c;                                      /**< Update last column */
		lastR = r;                                      /**< Update last row */

		glcd_gotocr(c, r);                              /**< Move to specified position */
		for (uint8_t i = 0; i < maxDots; i++)           /**< Clear all dot positions */
		{
				glcd_putc(' ');                         /**< Clear each dot with space */
		};
	};

	dotCounter++;                                     /**< Increment animation counter */

	if (dotCounter <= maxDots)                        /**< Check if within dot range */
	{
		/* Print next dot */
		glcd_gotocr(c + dotCounter - 1, r);             /**< Move to next dot position */
		glcd_putc('.');                                 /**< Display dot character */
	}
	else                                              /**< Animation cycle complete */
	{
		// Clear all dots and reset counter
		glcd_gotocr(c, r);                              /**< Return to start position */
		for (uint8_t i = 0; i < maxDots; i++)           /**< Clear all dot positions */
		{
				glcd_putc(' ');                             /**< Clear each dot with space */
		};
		dotCounter = 0;                                 /**< Reset animation counter */
	};
};


/* ============================================================================
 *                         PROGRESS BAR FUNCTION
 * ============================================================================ */

/* -------------------------------------------------------
 * @brief Draws a progress bar on the GLCD.
 * @param start_col Starting column (pixel X coordinate) of the progress bar (0..127)
 * @param end_col Ending column (pixel X coordinate) of the progress bar (0..127)
 * @param row Row (page index) where the progress bar will be drawn (0..7)
 * @param percentage The progress percentage (0-100)
 * ------------------------------------------------------- */
void glcd_progressbar(uint8_t start_col, uint8_t end_col, uint8_t row, uint8_t percentage)
{
    /* Ensure row is within valid bounds (0-7) */
    if (row >= 8) return; 
    /* Ensure start_col and end_col are within valid bounds (0-127) */
    if (start_col >= __GLCD_WIDTH) start_col = __GLCD_WIDTH - 1;  /**< Clamp start column */
    if (end_col >= __GLCD_WIDTH) end_col = __GLCD_WIDTH - 1;      /**< Clamp end column */
    /* Ensure start_col is not greater than end_col */
    if (start_col > end_col) 
    {
            uint8_t temp = start_col;                   /**< Temporary swap variable */
            start_col = end_col;                        /**< Swap start and end */
            end_col = temp;                             /**< Complete swap */
    };
    /* Clamp percentage between 0 and 100 */
    if (percentage > 100) percentage = 100;            

    uint8_t total_width = end_col - start_col + 1;            /**< Calculate total width */
    uint8_t filled_width = (total_width * percentage) / 100;  /**< Calculate filled width */

    /* Draw progress bar with border */
    for (uint8_t col = start_col; col <= end_col; col++)  /**< Iterate through all columns */
    {
        goto_col(col);                                  /**< Set column address */
        goto_row(row);                                  /**< Set page address */

        uint8_t pattern;                                /**< Pixel pattern for current column */

        /* Left and right borders */
        if (col == start_col || col == end_col)        
        {
							pattern = 0xFF;                       /**< Solid border pattern */
        }
        /* Inside the progress bar */
        else if (col < start_col + filled_width)    /**< Check if filled section */
        {
						pattern = 0xFF;                         /**< Solid filled pattern */
        }
        else                                        /**< Empty section */
        {
						pattern = 0x81;                         /**< Pattern with top/bottom borders */
        };

        glcd_write(pattern, __glcd_writeData);      /**< Write pattern to display */
    };
};   


/* -------------------------------------------------------
 * @brief Draw 128-byte buffer as a graph on GLCD, with Min/Max reference lines
 * @param buffer  Pointer to 128-byte array (each value 0-63)
 * @param min_val Minimum reference level (0-63) -> drawn as dashed horizontal line
 * @param max_val Maximum reference level (0-63) -> drawn as dashed horizontal line
 * 
 * Same behavior as glcd_draw_graph, but additionally overlays two dashed
 * horizontal reference lines at the given min/max levels for easier
 * visual analysis of the plotted data.
 * 
 * @note No local frame buffer is used (important on RAM-limited MCUs).
 *       Each byte to write is computed on the fly directly from `buffer`,
 *       min_val and max_val.
 * @note Wherever the graph data touches a reference level, the dash of
 *       that line is skipped at that column plus the column right before
 *       and right after it, so the line doesn't visually merge with the
 *       actual graph curve at the crossing point.
 * @note The max line is not drawn if max_val is 63 (top of screen) or 0
 *       (an invalid/meaningless max). The min line is not drawn if min_val
 *       is 0 (bottom of screen) or 63 (an invalid/meaningless min). These
 *       edge/invalid cases would either sit on the screen border or make
 *       no sense as a reference level, so they're skipped.
 * ------------------------------------------------------- */
void glcd_draw_graph_minmax(const uint8_t *buffer, uint8_t min_val, uint8_t max_val)
{
	if (buffer == NULL) return;  /**< Input validation */

	if (max_val > 63) max_val = 63;                 /**< Clamp max reference level */
	if (min_val > 63) min_val = 63;                 /**< Clamp min reference level */

	bool draw_max_line = (max_val != 63) && (max_val != 0);  /**< Skip if at top edge or invalid (0) */
	bool draw_min_line = (min_val != 0) && (min_val != 63);  /**< Skip if at bottom edge or invalid (63) */

	/* Pre-compute page/bit position for max line */
	uint8_t max_y_pixel = 63 - max_val;             /**< Invert Y for max level */
	uint8_t max_page = max_y_pixel / 8;             /**< Page containing max line */
	uint8_t max_bit  = max_y_pixel % 8;             /**< Bit position of max line */

	/* Pre-compute page/bit position for min line */
	uint8_t min_y_pixel = 63 - min_val;             /**< Invert Y for min level */
	uint8_t min_page = min_y_pixel / 8;             /**< Page containing min line */
	uint8_t min_bit  = min_y_pixel % 8;             /**< Bit position of min line */

	glcd_clear();                                   /**< Clear entire display */

	for (uint8_t page = 0; page < 8; page++)        /**< 8 pages = 64 rows */
	{
		for (uint8_t x = 0; x < __GLCD_WIDTH; x++)    /**< 128 columns */
		{
			uint8_t pattern = 0x00;                     /**< Byte to write for this (page,x) */

			/* ---- Graph data bit for this column ---- */
			uint8_t height = buffer[x];                 /**< Get height value from buffer */
			if (height > 63) height = 63;               /**< Clamp to maximum height */

			uint8_t y_pixel = 63 - height;              /**< Invert Y (0 at bottom, 63 at top) */
			uint8_t g_page  = y_pixel / 8;               /**< Page for this data point */
			uint8_t g_bit   = y_pixel % 8;               /**< Bit position for this data point */

			if (g_page == page)                          /**< Only set if it belongs to current page */
			{
				pattern |= (1 << g_bit);                   /**< Add graph pixel */
			};

			/* Check if this column, or the one right before/after it, touches max/min */
			bool near_max = false;                        /**< Graph touches max level near this column */
			bool near_min = false;                        /**< Graph touches min level near this column */

			for (int8_t dx = -1; dx <= 1; dx++)           /**< Check x-1, x, x+1 */
			{
				int16_t nx = (int16_t)x + dx;               /**< Neighbor column index */
				if (nx < 0 || nx >= __GLCD_WIDTH) continue; /**< Skip out-of-range neighbors */

				uint8_t n_height = buffer[nx];              /**< Height at neighbor column */
				if (n_height > 63) n_height = 63;           /**< Clamp */

				if (n_height == max_val) near_max = true;   /**< Neighbor touches max level */
				if (n_height == min_val) near_min = true;   /**< Neighbor touches min level */
			};

			/* ---- Max reference line bit (dashed), skipped near graph touch points ---- */
			if (draw_max_line && page == max_page && (x % 2 == 0) && !near_max)
			{
				pattern |= (1 << max_bit);                 /**< Add max line pixel */
			};

			/* ---- Min reference line bit (dashed), skipped near graph touch points ---- */
			if (draw_min_line && page == min_page && (x % 2 == 0) && !near_min)
			{
				pattern |= (1 << min_bit);                 /**< Add min line pixel */
			};

			/* Skip writing empty bytes - display was already cleared */
			if (pattern != 0x00)
			{
				goto_col(x);                                /**< Set column address */
				goto_row(page);                             /**< Set page address */
				delay_us(1);                                /**< Small delay for stability */

				glcd_write(pattern, __glcd_writeData);      /**< Write combined pixel data */
			};
		};
	};
};