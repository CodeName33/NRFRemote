# NRFRemote
This is arduino project to use 2.4Ghz PC Remote (with lost receiver) with EventGhost app (Windows).

# NRF24Scanner.ino
Channel scaner. Usage:
1. Start scanner on arduino and open com port on PC.
2. Press a keys on your remote and watch changes on each channel ('w' to comport - next channel, 's' - prev)
3. Find channel with max changes while keys on remote are pressed

# NRF24Remote.ino
PC Remote receiver. Usage:
In sketch: 
1. Enter your channel to 'channel' var (may be speed need to be changed in myRF24.setDataRate)
2. Uncomment FIND_ADDRESS_MODE in sketch
3. Run sketch and watch com port.
4. Fast press button on your remote many times and find new lines with your device address (first 4-5 bytes)
![Alt text](readme01.jpg?raw=true "Image")
5. Comment FIND_ADDRESS_MODE uncomment DISPLAY_CODES_MODE and write your first 4 bytes in 'RemoteAddress' var.
6. Run sketch and watch what codes genereated while you press keys on your remote
7. Create your own SignalsInfo array with correct codes. Special chars in name: @ - no repeat key on long press, $ - alternative repeat of key with longer intervalsls
