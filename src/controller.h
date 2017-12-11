#include <mbed.h>
#include "ESCController.h"
#include "config.h"

class Controller{
    private:    
        ESCController escController1;
        ESCController escController2;
        ESCController escController3;
        ESCController escController4;    

    public:
        Controller(PinName pin1, PinName pin2, PinName pin3, PinName pin4): escController1(pin1), escController2(pin2), escController3(pin3), escController4(pin4){}; // delegation of constructors
        void initialize(void);
        void update(dataStruct * data);
};
