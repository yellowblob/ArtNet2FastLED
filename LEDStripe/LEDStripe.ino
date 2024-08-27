#include <ArtnetEther.h>
#include <FastLED.h>
ArtnetReceiver artnet;

// FastLED
#define NUM_LEDS 63
CRGB leds[NUM_LEDS];
#define DATA_PIN 6
const int universe = 1;

// Ethernet stuff
const IPAddress ip(10, 0, 0, 3);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

void setup() {
    // setup Ethernet/WiFi...

    Serial.begin(115200);
    Ethernet.begin(mac, ip);

    // setup FastLED
    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

    artnet.begin();
    // if Artnet packet comes to this universe, forward them to fastled directly
    //artnet.forwardArtDmxDataToFastLED(0, leds, NUM_LEDS);

    // this can be achieved manually as follows
    artnet.subscribeArtDmxUniverse(universe, [](const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote)
    {
         // artnet data size per packet is 512 max
         // so there is max 170 pixel per packet (per universe)
         for (size_t pixel = 0; pixel < NUM_LEDS; ++pixel)
         {
             size_t idx = pixel * 3;
             leds[pixel].r = data[idx + 0];
             leds[pixel].g = data[idx + 1];
             leds[pixel].b = data[idx + 2];
         }

         Serial.print("lambda : artnet data from ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.print(remote.port);
        Serial.print(", universe = ");
        Serial.print(universe);
        Serial.print(", size = ");
        Serial.print(size);
        Serial.print(") :");
        for (size_t i = 0; i < size; ++i) {
            Serial.print(data[i]);
            Serial.print(",");
        }
        Serial.println();
     });
}

void loop() {
    artnet.parse(); // check if artnet packet has come and execute callback
    FastLED.show();
}
