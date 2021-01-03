# M5Core2Symcon
Adurino Project using an MCore2 as Front End with IP Symcon
# Dependecies
HTTPClient
ArduinoJson

# Config
## Symcon
Create a json Config in a string variable in Symcon.
Sample:
```json
{
    "version": 2, // Config Version 
    "sleepTimeout": 15000, // Sleep timer
    "imagesrv": "http://192.168.88.6:3777/user/image/", //server were images are stored
    "elements": [
        {
            "type": "SLI", //Type is slider
            "head": "Wohnzimmer Licht", // Header shown on top of the screen
            "id": "23061",              // Variable to be chaned (int), in this case the brightness of a hue lamp (0..254)
            "factor": 2.5,              // Scaling factor 0-100 --> 0..250
            "image2": "/living.bmp",    // First image 96x96
            "image1": "/lon.bmp"        // Second image 96x96
        },
        {
            "type": "SLI",
            "head": "Kuechen Licht",
            "id": "55865",
            "factor": 2.5,
            "image2": "/kitchen.bmp",
            "image1": "/lon.bmp"
        },
        {
            "type": "SLI",
            "head": "Schlafzimmer Licht",
            "id": "54503",
            "factor": 2.5,
            "image2": "/sleep.bmp",
            "image1": "/lon.bmp"
        },
        {
            "type": "BT4",  // 4 Button Page
            "head": "Schalter", // Header show on top 
            "ids": ["40628","38587","15437","12145"], //4 Bool variable IDs
            "imagesOn":["/onCaf.bmp","/onAuto.bmp","/onBed.bmp","/onKitchen.bmp"], //4 image files refelecting the on state
            "imagesOff":["/offCaf.bmp","/offAuto.bmp","/offBed.bmp","/offKitchen.bmp"], // 4 image files reflection the off state
            "varOnly": [false,true,false,false]   //Variabel only or action request
        }
    ]
}
```
## Config of M5Core2
Load, compile and download the sketch to the M5Core2.
The Serial if doesn't give local echo.
Connect to the M5Core via USB-C / USB COM Port.
I'm using typically HTerm to configure it.
Parameters to be set/follow the questions...
WIFI: SSID and PSK
RPC: Symcon Server IP, User, Password and ID of varaible holding the config from above.

## TODO:
- Add option in SLI for pure vaiables
- Create a page to display stuff


