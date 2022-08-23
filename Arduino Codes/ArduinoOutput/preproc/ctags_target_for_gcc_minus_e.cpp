# 1 "c:\\Users\\diego\\Desktop\\Trimestre 17 FINAL YA FJLHI GJIOA UFKHAG\\P2\\Software\\Arduino Codes\\mata\\mata.ino"
# 2 "c:\\Users\\diego\\Desktop\\Trimestre 17 FINAL YA FJLHI GJIOA UFKHAG\\P2\\Software\\Arduino Codes\\mata\\mata.ino" 2


int y = 0;

void setup()
{
    pinSetup();
    sensors::initializeScales();
    sensors::initializeTDS();
    communications::serialSetup();
    sensors::scaleOn(scaleN,true);
    sensors::scaleOn(scaleP,true);
    sensors::scaleOn(scaleK,true);

}

void loop()
{
    while (!Serial.available())
    y = Serial.parseInt();
    communications::sendSensorInfo(y);
    y = 0;
    while(Serial.available()>0) Serial.read();

}
