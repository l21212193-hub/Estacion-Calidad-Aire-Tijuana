M#include <WiFi.h>
#include <ThingSpeak.h>
#include <PMS.h>
#include <DHT.h>

// --- CREDENCIALES DE RED Y THINGSPEAK ---
const char* ssid = "Megacable_2.4G_CE12";     // Tu Wi-Fi de 2.4 GHz
const char* password = "HbngfKvw";            // Tu contraseña exacta

unsigned long myChannelNumber = 3537223;
const char * myWriteAPIKey = "UE0JI0VRP0UCQCII";

WiFiClient client;

// --- CONFIGURACIÓN DE SENSORES ---
#define DHTPIN 5          // Pin de Datos del DHT11 en GPIO 5 (P5)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor PMS5003 en Serie 2 (RX2 = GPIO 16, TX2 = GPIO 17)
PMS pms(Serial2);
PMS::DATA data;

void setup() {
  Serial.begin(115200);
  
  // Inicializar comunicación serial del PMS5003
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  
  // Inicializar DHT11 y cliente ThingSpeak
  dht.begin();
  ThingSpeak.begin(client);

  Serial.println("\n====================================");
  Serial.println("--- ESTACIÓN DE CALIDAD DEL AIRE TIJUANA ---");
  Serial.println("Conectando a Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n¡Conexión Wi-Fi Establecida Exitosamente!");
  Serial.println("====================================\n");
}

void loop() {
  // Lectura del sensor de partículas
  if (pms.read(data)) {
    int pm25 = data.PM_AE_UG_2_5;
    int pm10 = data.PM_AE_UG_10_0;

    // Lectura del sensor de temperatura y humedad
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    // Despliegue de datos en el Monitor Serie
    Serial.println("------------------------------------");
    Serial.print("PM 2.5:      "); Serial.print(pm25); Serial.println(" ug/m3");
    Serial.print("PM 10:       "); Serial.print(pm10); Serial.println(" ug/m3");
    Serial.print("Temperatura: "); Serial.print(temp); Serial.println(" °C");
    Serial.print("Humedad:     "); Serial.print(hum); Serial.println(" %");
    Serial.println("------------------------------------");

    // Enviar datos a los 4 campos de ThingSpeak
    ThingSpeak.setField(1, pm25);
    ThingSpeak.setField(2, pm10);
    ThingSpeak.setField(3, temp);
    ThingSpeak.setField(4, hum);

    int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (status == 200) {
      Serial.println("-> ¡Datos publicados en ThingSpeak con éxito! (HTTP 200)");
    } else {
      Serial.print("-> Error al publicar en ThingSpeak. Código: ");
      Serial.println(status);
    }
    
    // Esperar 20 segundos para el siguiente envío
    delay(20000);
  }
}