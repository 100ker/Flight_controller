#include "mbed.h"

 #define WDEN 		(1UL<<0)
 #define WDCOUNT 	(0x0000FFFF)
 #define WDTOF		(0x00000004)
 #define WDRESET	(1UL<<1)

class Watchdog{
    private:

    public:
        static void kick(float s){
            LPC_WDT->WDCLKSEL = 0x1;
            uint32_t clk = SystemCoreClock / 16;
            LPC_WDT->WDTC = s*(float)clk;
            LPC_WDT->WDMOD = WDEN;
            
            kick();
        }

        static void kick(){
            LPC_WDT->WDFEED = 0xAA;
            LPC_WDT->WDFEED = 0x55;
        }
};