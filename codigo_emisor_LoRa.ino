#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================

const int MI_ID = 5;
const int DESTINO_ID = 6;

String mensajeOriginal = "SENSOR ACTIVO";

// ======================================================
// PINES
// ======================================================

// definir los pines del módulo LoRa con #define
//       según la tabla de conexiones.
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define NSS 5
#define RESET -1
#define DIO0 -1

// definir el pin del sensor Touch
//       y el umbral. Recuerde comprobar el
//       valor de su ESP32 antes de fijar el umbral.
#define TOUCH_PIN 27
const int UMBRAL_TOUCH = 700; 

bool tocado = false; 
// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // iniciar el bus SPI con SPI.begin(...) usando los pines definidos
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, NSS); 

  // indicar a la librería los pines del módulo con LoRa.setPins(...)
  //       (NSS, RESET = -1, DIO0 = -1)
  LoRa.setPins(NSS, RESET,DIO0); 

  if (!LoRa.begin(915E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

int valorTouch = touchRead(TOUCH_PIN);

if (valorTouch < UMBRAL_TOUCH && !tocado) {
  tocado = true; 
  enviarMensaje(); 
  }
  else {
    tocado = false; 
  }

  delay(2000);
}