# NRFRemote
This is arduino project to use 2.4Ghz PC Remote (with lost receiver) with EventGhost.

# NRF24Scanner.ino
Channel scaner. Usage:
1. Start scanner on frduino and open com port on PC.
2. Press a keys on your remote and watch changes on each channel ('w' to comport - next channel, 's' - prev)

# NRF24Remote.ino
Remote receiver. Usage:
Enter your channel to 'channel' (may be speed need to be changed in myRF24.setDataRate)
1. Uncomment FIND_ADDRESS_MODE in sketch
2. Run sketch and watch com port.
3. Fast press button on your remote many times and find new lines with your remotye address (first 4-5 bytes)
![Alt text](readme01.jpg?raw=true "Title")
4. Comment FIND_ADDRESS_MODE uncomment DISPLAY_CODES_MODE and write your first 4 bytes in RemoteAddress var.
5. Run sketch and watch what codes genereated while you press keys on your remote
6. Create your own SignalsInfo array with correct codes. Special chars in name: @ - no repeat key on long press, $ - alternative repeat of key with longer intervals
