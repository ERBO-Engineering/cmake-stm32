#ifdef USE_HAL
#include "hal-blink.c"
#else
#include "cmsis-blink.c"
#endif


int main(){
    blink();
}