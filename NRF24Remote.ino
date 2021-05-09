#include "RF24.h"
#include "printf.h"

uint8_t channel = 3;
uint8_t RemoteAddress[] = { 131, 191, 48, 222 }; //{ 65, 223, 152, 111 };
const uint64_t PipeAddress = 0xAA;
const uint32_t ShortRepeat = 100;
const uint32_t ShortRepeatFirst = 400;
const uint32_t LongRepeat = 500;
const uint32_t LongRepeatFirst = 300;
const uint32_t KeyResetTime = 1000;

const int MaxSignalData = 15;

RF24 myRF24(7, 8);
const uint8_t maxBufferSize = 40;
uint8_t myBuffer[maxBufferSize];
uint8_t bufferSize = 32;

uint32_t LastKeyTime = 0;
String ActiveKey;
int64_t keyTimer = millis();

struct SRawSignal
{
	uint8_t Addr[5];
	uint8_t PCF_PayloadLength;
	uint8_t PCF_PacketId;
	uint8_t PCF_NoAck;
	uint8_t Data[24];
	uint16_t Crc;

	bool IsSameData(SRawSignal &v)
	{
		if (PCF_PayloadLength != v.PCF_PayloadLength)
		{
			return false;
		}
		
		return (memcmp(v.Data, Data, PCF_PayloadLength < sizeof(Data) ? PCF_PayloadLength : sizeof(Data)) == 0);
	}

	bool IsValid()
	{
		return (PCF_PayloadLength <= sizeof(Data));
	}
};


struct SSignalInfo
{
	uint8_t Data[MaxSignalData];
	const char Name[16] PROGMEM;
};

struct SFoundInfo
{
	String Name;
};


const PROGMEM SSignalInfo SignalsInfo[] = {
	{2, 2, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Power@" },

	{4, 90, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Text@" },
	{1, 8, 2, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Print@" },


	{1, 141, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "List@" },
	{4, 37, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "TVVideo@" },
	{1, 9, 2, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Info@" },
	{4, 51, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "TVList@" },
	{4, 72, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "TVRec@" },
	{4, 39, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "FullScreen@" },
	{4, 36, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Disk@" },

	{4, 91, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Red" },
	{4, 92, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Green" },
	{4, 93, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Yellow" },
	{4, 94, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Blue" },

	{1, 36, 2, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Backspace@" },
	{6, 71, 68, 101, 18, 27, 109, 6, 65, 65, 162, 224, 77, 240, 58, "Enter@" },
	{6, 71, 68, 101, 18, 27, 23, 6, 65, 65, 162, 224, 77, 240, 58, "Up$" },
	{6, 71, 68, 101, 18, 27, 20, 6, 65, 65, 162, 224, 77, 240, 58, "Down$" },
	{6, 71, 68, 101, 18, 27, 10, 6, 65, 65, 162, 224, 77, 240, 58, "Right$" },
	{6, 71, 68, 101, 18, 27, 21, 6, 65, 65, 162, 224, 77, 240, 58, "Left$" },


	{1, 233, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "VolumeUp" },
	{1, 234, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "VolumeDown" },
	{1, 226, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Mute@" },
	{4, 13, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Win@" },
	{1, 156, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "ProgUp" },
	{1, 157, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "ProgDown" },

	{1, 178, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Rec@" },
	{4, 52, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Eject@" },
	{1, 180, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Back" },
	{1, 176, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Play@" },
	{1, 177, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Pause@" },
	{1, 179, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Forward" },
	{1, 182, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Prev" },
	{1, 181, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Next" },
	{1, 183, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "Stop@" },
	
	{6, 71, 68, 101, 18, 27, 91, 6, 65, 65, 162, 224, 77, 240, 58, "1@" },
	{6, 71, 68, 101, 18, 27, 90, 6, 65, 65, 162, 224, 77, 240, 58, "2@" },
	{6, 71, 68, 101, 18, 27, 101, 6, 65, 65, 162, 224, 77, 240, 58, "3@" },
	{6, 71, 68, 101, 18, 27, 100, 6, 65, 65, 162, 224, 77, 240, 58, "4@" },
	{6, 71, 68, 101, 18, 27, 103, 6, 65, 65, 162, 224, 77, 240, 58, "5@" },
	{6, 71, 68, 101, 18, 27, 102, 6, 65, 65, 162, 224, 77, 240, 58, "6@" },
	{6, 71, 68, 101, 18, 27, 97, 6, 65, 65, 162, 224, 77, 240, 58, "7@" },
	{6, 71, 68, 101, 18, 27, 96, 6, 65, 65, 162, 224, 77, 240, 58, "8@" },
	{6, 71, 68, 101, 18, 27, 99, 6, 65, 65, 162, 224, 77, 240, 58, "9@" },
	{6, 71, 68, 101, 18, 27, 98, 6, 65, 65, 162, 224, 77, 240, 58, "0@" },
	{6, 71, 68, 101, 16, 27, 96, 6, 65, 65, 162, 224, 77, 240, 58, "Asterisk@" },
	{6, 71, 68, 101, 16, 27, 101, 6, 65, 65, 162, 224, 77, 240, 58, "#@" },
	{6, 71, 68, 101, 18, 27, 108, 6, 65, 65, 162, 224, 77, 240, 58, "Clear@" },
	{6, 71, 68, 101, 18, 27, 109, 6, 65, 65, 162, 224, 77, 240, 58, "Enter@" },

	{6, 71, 68, 101, 18, 27, 69, 6, 65, 65, 162, 224, 77, 240, 58, "[KEYUP]" },
	{1, 0, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, "[KEYUP]" },
	{4, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "[KEYUP]" },
	{2, 0, 98, 183, 150, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, "[KEYUP]" },
};

bool compareSignalByte(uint8_t buffer, const uint8_t value)
{
	return (value == 255 || value == buffer);
}

bool findSignal(uint8_t *buffer, int size, SFoundInfo& outInfo)
{
	const int count = sizeof(SignalsInfo) / sizeof(SignalsInfo[0]);
	SSignalInfo SI = {};
	for (int i = 0; i < count; i++)
	{
		memcpy_P(&SI, &SignalsInfo[i], sizeof(SI));

		bool IsOK = true;
		for (int j = 0; j < size; j++)
		{
			if (!compareSignalByte(buffer[j], SI.Data[j]))
			{
				IsOK = false;
			}
		}

		for (int j = size; j < MaxSignalData; j++)
		{
			if (SI.Data[j] != 255)
			{
				IsOK = false;
			}
		}

		if (IsOK)
		{
			char Name[16];
			strcpy_P(Name, SignalsInfo[i].Name);
			outInfo.Name = Name;
			return true;
		}
	}
	return false;
}

void initBuffer()
{
	memset(myBuffer, 0, bufferSize);
}



void PrintDec3(uint8_t* data, uint8_t length)
{
	for (int i = 0; i < length; i++)
	{
		if (data[i] < 10)
		{
			Serial.print("  ");
		}
		else if (data[i] < 100)
		{
			Serial.print(" ");
		}
		Serial.print(data[i]);
		Serial.print(" ");
	}
}

void PrintHex8(uint8_t* data, uint8_t length)
{
	for (int i = 0; i < length; i++) {
		if (data[i] < 0x10) { Serial.print("0"); }
		Serial.print(data[i], HEX);
		Serial.print(" ");
	}
}

void print3(uint8_t value)
{
	if (value < 10)
	{
		Serial.print("  ");
	}
	else if (value < 100)
	{
		Serial.print(" ");
	}
	Serial.print(value);
}



String NameToKey(String Name)
{
	Name.replace("@", "");
	Name.replace("$", "");
	return Name;
}

bool IsNoRepeat(const String& Name)
{
	return (Name.indexOf('@') >= 0);
}

bool IsLongRepeat(const String& Name)
{
	return (Name.indexOf('$') >= 0);
}

void printBytes()
{
	static SRawSignal LastRS = {};

	int RemoteAddressSize = sizeof(RemoteAddress);
	for (int i = 0; i < RemoteAddressSize; i++)
	{
		if (myBuffer[i] != RemoteAddress[i])
		{
			return;
		}
	}

	//PrintDec3(myBuffer, 32);
	//Serial.println();

	SRawSignal RS = {};

	memcpy(&RS, myBuffer, 5);
	RS.PCF_PayloadLength = myBuffer[5] >> 2;
	RS.PCF_PacketId = myBuffer[5] & 3;
	RS.PCF_NoAck = myBuffer[6] >> 7;
	for (int i = 0; i < sizeof(RS.Data); i++)
	{
		RS.Data[i] = (myBuffer[6 + i] << 1) + (myBuffer[7 + i] >> 7);
	}
	RS.Crc = RS.Data[RS.PCF_PayloadLength < sizeof(RS.Data) ? RS.PCF_PayloadLength : sizeof(RS.Data) - 1];

	if (/*!LastRS.IsSameData(RS) &&*/ RS.IsValid())
	{
		LastRS = RS;
			

		SFoundInfo FI;
		if (findSignal(RS.Data, RS.PCF_PayloadLength, FI))
		{

			if (FI.Name == "[KEYUP]")
			{
				if (ActiveKey.length() > 0)
				{
					ActiveKey = "";
					Serial.println(F("IR.None"));
				}
			}
			else
			{
				LastKeyTime = millis();
				if (ActiveKey != FI.Name)
				{
					Serial.print(F("IR."));
					Serial.println(NameToKey(FI.Name));
					ActiveKey = FI.Name;
					int64_t Repeat = (IsLongRepeat(FI.Name) ? LongRepeatFirst : ShortRepeatFirst);
					keyTimer = millis() + Repeat;
				}
			}
		}

		//PrintHex8(RS.Addr, sizeof(RS.Addr));
		
		/*
		//else
		{
			//Serial.print(" Addr (HEX): ");
			//PrintHex8(RS.Addr, sizeof(RS.Addr));

			Serial.print(" Addr: ");
			print3(RS.Addr[sizeof(RS.Addr) - 1]);

			Serial.print(" Len: ");
			print3(RS.PCF_PayloadLength);
			Serial.print(" PID: ");
			print3(RS.PCF_PacketId);
			Serial.print(" NoAck: ");
			print3(RS.PCF_NoAck);
			Serial.print(" Data:");

			int size = sizeof(RS.Data) < RS.PCF_PayloadLength ? sizeof(RS.Data) : RS.PCF_PayloadLength;
			for (int i = 0; i < size; i++)
			{
				Serial.print(" ");
				print3(RS.Data[i]);
			}
			//Serial.print(" CRC: ");
			//Serial.print(RS.Crc);
			Serial.println();
			
			{
				//Serial.print("{");
				for (int i = 0; i < MaxSignalData; i++)
				{
					//Serial.print(i >= RS.PCF_PayloadLength ? 255 : RS.Data[i]);
					//Serial.print(", ");
				}
				//Serial.print("\"\" },");
				//Serial.println();
			}
		}
		//*/
	}
}

void setup()
{
	Serial.begin(115200);
	//return;

	printf_begin();

	initBuffer();
	myRF24.begin();
	myRF24.setChannel(channel);
	myRF24.disableCRC();
	//myRF24.setCRCLength(RF24_CRC_8);
	myRF24.setAutoAck(false);
	myRF24.setDataRate(RF24_2MBPS);
	myRF24.setAddressWidth(2);

	//myRF24.printDetails();
	
	//myRF24.openReadingPipe(0, 0x83BF30DELL);
	myRF24.openReadingPipe(0, PipeAddress);
	myRF24.startListening();

	Serial.println("IR.Started");
}



void loop()
{
	while (myRF24.available())
	{
		myRF24.read(myBuffer, bufferSize);
		printBytes();
		initBuffer();
	}

	if (millis() - LastKeyTime > KeyResetTime)
	{
		if (ActiveKey.length() > 0)
		{
			ActiveKey = "";
			Serial.println(F("IR.Timeout"));
		}
	}

	if (ActiveKey.length() > 0 && !IsNoRepeat(ActiveKey))
	{
		int64_t Repeat = (IsLongRepeat(ActiveKey) ? LongRepeat : ShortRepeat);
		if ((int64_t)millis() - keyTimer > Repeat)
		{
			keyTimer = millis();
			Serial.println(F("IR.Repeat"));
			Serial.print(F("IR."));
			Serial.println(NameToKey(ActiveKey));
		}
	}
}