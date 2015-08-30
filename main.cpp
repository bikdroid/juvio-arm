#include "mbed.h"
#include "FXOS8700Q.h"
#include "M2XStreamClient.h"
#include "EthernetInterface.h"
#include "LED_Bar.h"


#ifndef MAX
#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#endif

#ifndef MIN
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#endif

char deviceId[] = "f8672699ff0e43ee441417ff05d5c672"; // Device you want to push to
char streamName[] = "temperature"; // Stream you want to push to
char m2xKey[] = "dab54ddeb1cd0304118a5184cf5aff2c"; // Your M2X API Key or Master API Key

/* Analog Inputs and everything ..*/
DigitalOut red(LED_RED);                 // Red led
DigitalOut blue(LED_BLUE); //blue led
DigitalOut green(LED_GREEN); // green LED
AnalogIn T1(PTB2); //  temperature  sensor T1 on A0
AnalogIn T2(PTB3); // temperature  sensor T2 on A1

float T; // average temperature
float tempF; //average temperature in Fahrenheit
float K=3.3*100; // scaling coefficient for calculating

LED_Bar bar(PTB10,PTB11);
// analog value to temperature

Serial pc(USBTX,USBRX); // print temperature on console (eg. Putty)


int main()
{
    // Setup Ethernet
    printf("Start\r\n");
    EthernetInterface eth;
    printf("Init...\r\n");
    eth.init(); //Use DHCP
    //eth.init(ip,mask,gateway); //Use Static IP Configuration
    printf("Connect\r\n");
    eth.connect();
    printf("Device IP Address is %s\r\n", eth.getIPAddress());

    // Initialize the M2X client
    Client client;
    M2XStreamClient m2xClient(&client, m2xKey);
    int ret;

    // Create an accelerometer instance
    FXOS8700Q_acc acc( PTE25, PTE24, FXOS8700CQ_SLAVE_ADDR1); // Proper Ports and I2C Address for K64F Freedom board
    acc.enable();
    printf("\r\n\nFXOS8700Q Who Am I= %X\r\n", acc.whoAmI());

    while (true) {


        /*
                float x, y, z;
                acc.getX(&x);
                acc.getY(&y);
                acc.getZ(&z);

                printf("Accel X: %1.2f, Y: %1.2f, Z: %1.2f\n\r", x, y, z);

                // Calculate pitch and roll. Find the maximum tilt angle.
                float pitch = atan(x / sqrt(y * y + z * z));
                float roll = atan(y / sqrt(x * x + z * z));
                float maxTilt =
                    max(abs(roll), abs(pitch)) * 180.0 / 3.14159;
                printf("pitch: %5.1f roll: %5.1f maxTilt: %5.1f\n\r",
                       pitch, roll, maxTilt);

                // If the maximum title is over 20 degrees, then send
                // data to stream
                if (maxTilt > 20) {
                    ret = m2xClient.updateStreamValue(deviceId, streamName, maxTilt);
                    printf("send() returned %d\r\n", ret);
                    wait(1.0);
                    */

        //pc.printf("LM35 temperature sensor\n");


        red=0;//red=0;
        blue=0; //blue=0;
        green=0; //green=0;
        T=(T1+T1)/2.*K; // average temperature value in Celsius (C)

        

            //lCD code and disply line
        /*
        if(T>70) {
            led.period(2.0f);
            led.write(0.10f);
        } else {
            led.period(2.0f);
            led.write(0.95f);
        }
        */

           

        tempF=(9.0*T)/5.0 + 32.0; //average temperature value in fahrenheit (F)



        wait(0.2f); //wait a little
        pc.printf("Temperature (in Celsius) is %4.2f \r\n",T);


        wait(0.2f); //wait a little
        pc.printf("Temperature (in Fahrenheit) is %4.2f \r\n",tempF);


        if ( T>=18 && T<=22 ) green=1;
        else if ((T>=16 && T<18)||(T>22 && T<=24)) blue=1;
        else red=1;

        
        if(tempF>100){ 
            bar.setLevel(7);
        }else{
            bar.setLevel(2);
            }
        
        int response = m2xClient.updateStreamValue(deviceId, streamName, tempF); // Updating Farehnheit temperature logged from the analog sensor to the temperature stream
        printf("Temperature update response code : %d\r\n", response);
        if (response == -1) while (true);

        wait(0.2);

    }
}