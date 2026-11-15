/* STM32F401xC with the firmware capped at 128 KB (sectors 0-4).
 *
 * The wear-leveled EEPROM (embedded_flash driver, see info.json) takes the last flash
 * sector, sector 5 (0x08020000, 128 KB). The stock script lets code run into it, which
 * would silently corrupt the firmware or the EEPROM; with this cap the link fails instead.
 */

f4xx_flash_size = 128k;
f4xx_ram1_size = 64k;
f4xx_ram2_size = 0;
f4xx_ram4_size = 0;
f4xx_ram5_size = 0;

/* Relative to the build directory (QMK root): only this script's own folder is searched. */
INCLUDE platforms/chibios/boards/common/ld/stm32f4xx_common.ld
