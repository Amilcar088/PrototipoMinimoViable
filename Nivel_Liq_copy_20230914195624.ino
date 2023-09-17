#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>



#include <WiFi.h>
// #include 

#include "Esp32MQTTClient.h"

// wifi
#define IOT_CONFIG_WIFI_SSID              "SSID"
#define IOT_CONFIG_WIFI_PASSWORD          "PASS"

// Azure IoT
#define DPS_ID_SCOPE                      "xxxx"
#define IOT_CONFIG_DEVICE_ID              "xxxx"
#define IOT_CONFIG_DEVICE_KEY             "xxxx"


// Publicar mensaje cada 5 segundos
#define TELEMETRY_FREQUENCY_MILISECS 5000

// Configuramos los pines del sensor Trigger y Echo
const int PinTrig = 2;
const int PinEcho = 4;
 
// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;
 
// Número de muestras
const int numLecturas = 100;
 
// Distancia a los 10 l y vacío
const float distancia10 = 35;
const float distanciaVacio = 40;
 
float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0; // Lectura por la que vamos
float total = 0; // Total de las que llevamos
float media = 0; // Media de las medidas
bool primeraMedia = false; // Para saber que ya hemos calculado por lo menos una
 
void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);

  // Ponemos el pin Trig en modo salida
  pinMode(PinTrig, OUTPUT);
  // Ponemos el pin Echo en modo entrada
  pinMode(PinEcho, INPUT);
 
  // Inicializamos el array
  for (int i = 0; i < numLecturas; i++)
  {
    lecturas[i] = 0;
  }
}
void loop()
{
  // Eliminamos la última medida
  total = total - lecturas[lecturaActual];
 
  iniciarTrigger();
 
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(PinEcho, HIGH, 10);
 
  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * VelSon / 2.0;
 
  // Almacenamos la distancia en el array
  lecturas[lecturaActual] = distancia;
 
  // Añadimos la lectura al total
  total = total + lecturas[lecturaActual];
 
  // Avanzamos a la siguiente posición del array
  lecturaActual = lecturaActual + 1;
 
  // Comprobamos si hemos llegado al final del array
  if (lecturaActual >= numLecturas)
  {
    primeraMedia = true;
    lecturaActual = 0;
  }
 
  // Calculamos la media
  media = total / numLecturas;
 
  // Solo mostramos si hemos calculado por lo menos una media
  if (primeraMedia)
  {
    float distanciaLleno = distanciaVacio - media;
    float cantidadLiquido = distanciaLleno * 100 / distancia10;
 
    Serial.print(media);
    Serial.println(" cm");
    



    Serial.print(cantidadLiquido);
    Serial.println(" l");
  }
 
  delay(500);
}
 
// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PinTrig, LOW);
  delayMicroseconds(2);
 
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PinTrig, HIGH);
  delayMicroseconds(10);
 
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PinTrig, LOW);
}