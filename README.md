## GameControllersSTM32  
GameControllersSTM32 is a library that allows reading some miscellaneous game controllers on STM32F1 boards.
Supported controllers include:
 
## Purpose  
Read data from:

 * GameCube
 
 * Wii Nunchuck (accelerometer not currently supported)
 
 * PC analog game port

## Usage  
* `GameCubeController controller = GameCubeController(unsigned pin);`

* `NunchuckController controller = NunchuckController(unsigned scl=PB6, unsigned sda=PB7);` (uses software I2C)

* `GamePortController controller = GamePortController(unsigned xPin, unsigned yPin, unsigned sliderPin, unsigned rotatePin,
        unsigned button1Pin, unsigned button2Pin, unsigned button3Pin, unsigned button4Pin);` 
        
* `bool controller.begin();` (Returns false on fail)

* `bool controller.read(GameControllerData_t* data);`  (Returns false on fail)

* pins on gameport connector: X=3, Y=6, slider=13, rotate=11, buttons=2,7,10,14; the code assumes a 10K pull-down from the analog axes to ground
