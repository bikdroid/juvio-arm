#include "mbed.h"
#include "FXOS8700Q.h"
#include "M2XStreamClient.h"
#include "EthernetInterface.h"


#ifndef MAX
#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#endif

#ifndef MIN
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#endif

char deviceId[] = "<device id>"; // Device you want to push to
char streamName[] = "<stream name>"; // Stream you want to push to
char m2xKey[] = "<m2x api key>"; // Your M2X API Key or Master API Key

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
            wait(0.5);
        }
    }
}