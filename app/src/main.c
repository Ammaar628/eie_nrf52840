/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

// Password configuration - set the correct password combination
#define PASSWORD_BTN0 true
#define PASSWORD_BTN1 false
#define PASSWORD_BTN2 true

typedef enum {
  STATE_LOCKED,
  STATE_WAITING
} system_state;

int main(void) {
  system_state state = STATE_LOCKED;
  bool btn0_pressed = false;
  bool btn1_pressed = false;
  bool btn2_pressed = false;

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  // Start in locked state with LED0 on
  LED_set(LED0, LED_ON);
  printk("System locked. Enter password using BTN0, BTN1, BTN2, then press BTN3.\n");
  printk("Password hint: BTN0=%s, BTN1=%s, BTN2=%s\n", 
         PASSWORD_BTN0 ? "PRESS" : "DON'T PRESS",
         PASSWORD_BTN1 ? "PRESS" : "DON'T PRESS",
         PASSWORD_BTN2 ? "PRESS" : "DON'T PRESS");

  while(1) {
    if (state == STATE_LOCKED) {
      // Track button presses
      if (BTN_check_clear_pressed(BTN0)) {
        btn0_pressed = true;
        printk("BTN0 pressed\n");
      }
      if (BTN_check_clear_pressed(BTN1)) {
        btn1_pressed = true;
        printk("BTN1 pressed\n");
      }
      if (BTN_check_clear_pressed(BTN2)) {
        btn2_pressed = true;
        printk("BTN2 pressed\n");
      }
      
      // Check if BTN3 (enter) is pressed
      if (BTN_check_clear_pressed(BTN3)) {
        // Verify password
        if (btn0_pressed == PASSWORD_BTN0 && 
            btn1_pressed == PASSWORD_BTN1 && 
            btn2_pressed == PASSWORD_BTN2) {
          printk("Correct!\n");
        } else {
          printk("Incorrect!\n");
        }
        
        // Turn off LED0 and enter waiting state
        LED_set(LED0, LED_OFF);
        state = STATE_WAITING;
        printk("Press any button to reset...\n");
        
        // Reset button press tracking
        btn0_pressed = false;
        btn1_pressed = false;
        btn2_pressed = false;
      }
    } 
    else if (state == STATE_WAITING) {
      // Any button press resets to locked state
      if (BTN_check_clear_pressed(BTN0) || 
          BTN_check_clear_pressed(BTN1) || 
          BTN_check_clear_pressed(BTN2) || 
          BTN_check_clear_pressed(BTN3)) {
        
        state = STATE_LOCKED;
        LED_set(LED0, LED_ON);
        btn0_pressed = false;
        btn1_pressed = false;
        btn2_pressed = false;
        printk("\nSystem reset. System locked.\n");
        printk("Enter password using BTN0, BTN1, BTN2, then press BTN3.\n");
      }
    }
    
    k_msleep(SLEEP_MS);
  }
	return 0;
}
