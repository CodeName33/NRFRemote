#include "printf.h"
#include "RF24.h"

RF24 myRF24(7, 8);
uint8_t chan = 0;
const uint8_t num_samples = 100;
const uint8_t threshold = 5;

void setup(void)
{
    Serial.begin(115200);
    printf_begin();

    myRF24.begin();
    myRF24.disableCRC();
    myRF24.setAutoAck(false);
    myRF24.setDataRate(RF24_2MBPS);
    myRF24.printDetails();

    Serial.println("OK.");
}

void loop(void)
{

	byte in;
	if (Serial.available() > 0) {
		in = Serial.read();
		if (in == 'w')
		{
			chan += 1;
			Serial.print("\nSet chan: ");
			Serial.println(chan);
		}
		if (in == 's')
		{
			chan -= 1;
			Serial.print("\nSet chan: ");
			Serial.println(chan);
		}
	}

	int count = 0;
	myRF24.setChannel(chan);
    for (int i = 0; i < num_samples * 10; i++)
	{
        myRF24.startListening();
        delayMicroseconds(128);
        myRF24.stopListening();

        if (myRF24.testCarrier())
		{
			count++;
        }
    }

	Serial.print("Channel: ");
	Serial.print(chan);
	Serial.print(" Count: ");
	Serial.println(count);
}