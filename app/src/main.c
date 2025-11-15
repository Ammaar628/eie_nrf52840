/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {
  uint8_t counter = 0;

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  // Initialize all LEDs to OFF
  LED_set(LED0, LED_OFF);
  LED_set(LED1, LED_OFF);
  LED_set(LED2, LED_OFF);
  LED_set(LED3, LED_OFF);

  while(1) {
    if (BTN_check_clear_pressed(BTN0)){
      counter++;
      
      // Reset counter when it reaches 16
      if (counter >= 16) {
        counter = 0;
      }
      
      // Display counter in binary on LEDs
      // LED0 = bit 0 (LSB), LED1 = bit 1, LED2 = bit 2, LED3 = bit 3 (MSB)
      LED_set(LED0, (counter & 0x01) ? LED_ON : LED_OFF);
      LED_set(LED1, (counter & 0x02) ? LED_ON : LED_OFF);
      LED_set(LED2, (counter & 0x04) ? LED_ON : LED_OFF);
      LED_set(LED3, (counter & 0x08) ? LED_ON : LED_OFF);
      
      printk("Counter: %d (Binary: %d%d%d%d)\n", 
             counter,
             (counter & 0x08) ? 1 : 0,
             (counter & 0x04) ? 1 : 0,
             (counter & 0x02) ? 1 : 0,
             (counter & 0x01) ? 1 : 0);
    }
    k_msleep(SLEEP_MS);
  }
	return 0;
}
