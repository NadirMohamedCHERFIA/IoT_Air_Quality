#include "DFRobot_CCS811.h"
/*
 * IIC address default 0x5A, the address becomes 0x5B if the ADDR_SEL is soldered.
 */
//DFRobot_CCS811 CCS811(&Wire, /*IIC_ADDRESS=*/0x5A);
DFRobot_CCS811 CCS811;

void setup(void)
{
    Serial.begin(115200);
    /*Wait for the chip to be initialized completely, and then exit*/
    while(CCS811.begin() != 0){
        Serial.println("failed to init chip, please check if the chip connection is fine");
        delay(1000);
    }
}
void loop() {
    if(CCS811.checkDataReady() == true){
        /*!
        HERE WE TRY TO PUT THE SENSOR IN UNPOLLUTED AREA IN ORDER TO GET THE BASELINE
        FOR THE SENSOR, THE SENSOR IT SELF CAN DO THE SAME PROCEDURE BUT IT WILL TAKE 
        FOREVER THATS WHY WE ARE DOING OUR SELFS (THE VALUE IS IN HEXADECIMAL)
         */
        Serial.println(CCS811.readBaseLine(), HEX);

    } else {
        Serial.println("Data is not ready!");
    }
    //delay cannot be less than measurement cycle
    delay(1000);
}
