#include "debug.h"
#include <FlashStorage.h>
#include <sam.h>

FlashStorage(persistent, class FunctionMarker);


int line;
const char * function;

void FunctionMarker::set(int l, const char * str){
  line = l;
  function = (char*)str;
}

void FunctionMarker::print(void){
  Serial.print("Halted at ");
  Serial.print(function);
  Serial.print(", line ");
  Serial.println(line);
}

void FunctionMarker::write(void){
  // Copy pointed value to buffer
  strncpy(buffer, function, 100);
  function = buffer;
  // Then save to the flash
  persistent.write(*this);
}

bool FunctionMarker::read(void){
  // Read the value from memory
  *this = persistent.read();

  // If we stored this before, then the function points to the buffer.
  return (function==buffer);
}

// Requires Adafruit_ASFcore library!

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(ARDUINO_ARCH_SAMD)

void WDT_Handler(void) {
    WDT->INTFLAG.bit.EW  = 1;        // Clear interrupt flag
    Serial.println("Watchdog!");
    halt_location.print();
    halt_location.write();
}

int WatchdogSAMD::enable(int maxPeriodMS) {
    // Enable the watchdog with a period up to the specified max period in
    // milliseconds.

    // Review the watchdog section from the SAMD21 datasheet section 18:
    // http://ww1.microchip.com/downloads/en/DeviceDoc/SAMD21-Family-DataSheet-DS40001882D.pdf

    int     cycles;
    uint8_t bits;

    if(!_initialized) _initialize_wdt();

#if defined(__SAMD51__)
    WDT->CTRLA.reg = 0; // Disable watchdog for config
    while(WDT->SYNCBUSY.reg);
#else
    WDT->CTRL.reg = 0; // Disable watchdog for config
    while(WDT->STATUS.bit.SYNCBUSY);
#endif

    if((maxPeriodMS >= 8000) || !maxPeriodMS) {
        bits   = 0xA;
    } else {
        cycles = maxPeriodMS >> 2; // min delay is 8ms
        bits = 0;
        // Cycle choices are in powers of 2
        while( cycles = cycles>>1){
            ++bits;
        }
    }
    cycles = 8<<bits;

    // Watchdog timer on SAMD is a slightly different animal than on AVR.
    // On AVR, the WTD timeout is configured in one register and then an
    // interrupt can optionally be enabled to handle the timeout in code
    // (as in waking from sleep) vs resetting the chip.  Easy.
    // On SAMD, when the WDT fires, that's it, the chip's getting reset.
    // Instead, it has an "early warning interrupt" with a different set
    // interval prior to the reset.  For equivalent behavior to the AVR
    // library, this requires a slightly different configuration depending
    // whether we're coming from the sleep() function (which needs the
    // interrupt), or just enable() (no interrupt, we want the chip reset
    // unless the WDT is cleared first).  In the sleep case, 'windowed'
    // mode is used in order to allow access to the longest available
    // sleep interval (about 16 sec); the WDT 'period' (when a reset
    // occurs) follows this and is always just set to the max, since the
    // interrupt will trigger first.  In the enable case, windowed mode
    // is not used, the WDT period is set and that's that.

#if defined(__SAMD51__)
    WDT->INTFLAG.bit.EW      = 1;    // Clear interrupt flag
    WDT->INTENSET.bit.EW     = 1;    // Enable early warning interrupt
    WDT->CONFIG.bit.PER      = bits+1;  // Period = twice
    WDT->EWCTRL.bit.EWOFFSET = bits;  // Set time of interrupt
    WDT->CTRLA.bit.WEN       = 0;    // Disable window mode
    while(WDT->SYNCBUSY.reg);        // Sync CTRL write
    reset();                             // Clear watchdog interval
    WDT->CTRLA.bit.ENABLE = 1;           // Start watchdog now!
    while(WDT->SYNCBUSY.reg);
#else
    WDT->INTENSET.bit.EW   = 1;      // Enable early warning interrupt
    WDT->CONFIG.bit.PER    = bits+1;    // Period = twice
    WDT->EWCTRL.bit.EWOFFSET = bits;  // Set time of interrupt
    WDT->CTRL.bit.WEN      = 0;      // Disable window mode
    while(WDT->STATUS.bit.SYNCBUSY); // Sync CTRL write
    reset();                             // Clear watchdog interval
    WDT->CTRL.bit.ENABLE = 1;            // Start watchdog now!
    while(WDT->STATUS.bit.SYNCBUSY);
#endif

    return cycles;
}

void WatchdogSAMD::reset() {
    // Write the watchdog clear key value (0xA5) to the watchdog
    // clear register to clear the watchdog timer and reset it.
#if defined(__SAMD51__)
    while(WDT->SYNCBUSY.reg);
#else
    while(WDT->STATUS.bit.SYNCBUSY);
#endif
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}


uint8_t WatchdogSAMD::resetCause() {
#if defined(__SAMD51__)
  return RSTC->RCAUSE.reg;
#else
  return PM->RCAUSE.reg;
#endif
}

void WatchdogSAMD::disable() {
#if defined(__SAMD51__)
    WDT->CTRLA.bit.ENABLE = 0;
    while(WDT->SYNCBUSY.reg);
#else
    WDT->CTRL.bit.ENABLE = 0;
    while(WDT->STATUS.bit.SYNCBUSY);
#endif
}

void WatchdogSAMD::_initialize_wdt() {
    // One-time initialization of watchdog timer.
    // Insights from rickrlh and rbrucemtl in Arduino forum!

#if defined(__SAMD51__)
    // SAMD51 WDT uses OSCULP32k as input clock now
    // section: 20.5.3
    OSC32KCTRL->OSCULP32K.bit.EN1K  = 1; // Enable out 1K (for WDT)
    OSC32KCTRL->OSCULP32K.bit.EN32K = 0; // Disable out 32K

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn, 0); // Top priority
    NVIC_EnableIRQ(WDT_IRQn);

    while(WDT->SYNCBUSY.reg);
    
    USB->DEVICE.CTRLA.bit.ENABLE = 0;         // Disable the USB peripheral
    while(USB->DEVICE.SYNCBUSY.bit.ENABLE);   // Wait for synchronization
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;       // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE = 1;         // Enable the USB peripheral
    while(USB->DEVICE.SYNCBUSY.bit.ENABLE);   // Wait for synchronization
#else
    // Generic clock generator 2, divisor = 32 (2^(DIV+1))
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4);
    // Enable clock generator 2 using low-power 32KHz oscillator.
    // With /32 divisor above, this yields 1024Hz(ish) clock.
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
                        GCLK_GENCTRL_GENEN |
                        GCLK_GENCTRL_SRC_OSCULP32K |
                        GCLK_GENCTRL_DIVSEL;
    while(GCLK->STATUS.bit.SYNCBUSY);
    // WDT clock = clock gen 2
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK2;

    // Enable WDT early-warning interrupt
    NVIC_DisableIRQ(WDT_IRQn);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_SetPriority(WDT_IRQn, 0); // Top priority
    NVIC_EnableIRQ(WDT_IRQn);
#endif

    _initialized = true;
}

#endif // defined(ARDUINO_ARCH_SAMD)
