/*
  Pines utilizados.
  - D2 -> Interrupción reloj. (El reloj envia una interrupción por segundo)
  - D3 -> Estado BlueTooth (0 - Desconectado, 1 - Enlazado)
  - D4 -> Pin termómetro.
  - D6 -> Pin control "Led 1"
  - D7 -> Pin control "Led 2"
  - A4 -> SDA
  - A5 -> SCL
*/
#include <Arduino.h>
#include <RTClib.h>
#include <Pantalla.h>
#include <DallasTemperature.h>
#include <misFunciones.h>
#include <Define.h>

// ######################### CONFIGURACIÓN ###################################
void setup()
{
#if defined(DEBUG)
  Serial.begin(115200);
#endif
  Serial1.begin(2400);
  // Configuramos el pin conectado a la patilla SQW del reloj como INPUT_PULLUP
  // para no tener que poner una resistencia externa.
  pinMode(PIN_INTERRUPCION, INPUT_PULLUP);
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_L2, OUTPUT);
  
  // INICIALIZAMOS LA PANTALLA.
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, DIRECCION_PANTALLA))
  {
#if defined(DEBUG)
    Serial.println(F("Error en la asignación de SSD1306"));
#endif
    hayPantalla = false;
  }
  else
  {
    // Pantalla iniciada correctamente.
#if defined(DEBUG)
    Serial.println(F("SSD1306 iniciado."));
#endif
    hayPantalla = true;
  }

  // Si la pantalla se ha iniciado correctamente mostramos información.
  if (hayPantalla)
  {
    // Mostrar pantalla de inicio de Adafruit.
    pantalla.display();
    delay(2000);
#if defined(DEBUG)
    Serial.println(F("Borramos pantalla."));
#endif
    pantalla.clearDisplay();
    pantalla.display();
    // Configuramos la pantalla.
    pantalla.setTextSize(sizeText::Normal); // Normal
    pantalla.setTextColor(SSD1306_WHITE);   // Definir color blanco
    pantalla.setCursor(0, 0);               // Situamos el cursor en la parte superior izquierda de la pantalla.
    pantalla.cp437(true);                   // Habilitamos la página de códigos 437
                                            // Pantalla bienvenida.
#if defined(DEBUG)
    Serial.println(F("Pantalla configurada correctamente."));
#endif
  }

  // ############ CONFIGURACIÓN RELOJ ###################
  if (!reloj.begin())
  {
    // Error inicializando el reloj.
#if defined(DEBUG)
    Serial.println(F("Reloj no funciona."));
#endif
    hayReloj = false;
    Muestra(&pantalla, "   Reloj\r\nno activo");
  }
  else
  {
    // Se pasa al reloj la fecha y hora del momento de la compilación.
    reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));
#if defined(DEBUG)
    Serial.println(F("Reloj funcionando."));
#endif

    // Configuramos la señal del reloj (SQW) a 1 Hz.
    reloj.writeSqwPinMode(DS1307_SquareWave1HZ);
    hayReloj = true;
  }

  // ######################## CONFIGURAR TERMÓMETRO ##########################################
  lecturaTemperatura.begin();
  // Buscamos el termómetro y guardamos la dirección.
  if (lecturaTemperatura.getDeviceCount() < 1)
  {
    Muestra(&pantalla, "No se ha encontrado ningún termómetro.");
  }
  else
  {
#if defined(DEBUG)
    Serial.println(F("Termómetro OK"));

    if(lecturaTemperatura.getAddress(direccionTermometro, 0)){
    Serial.print("Dirección termómetro: ");
    Serial.println(direccionTermometro[0], HEX);
    }
    else
    {
      Serial.println("No encontrada la dirección para el dipositivo 0");
    }
  }
#endif
  memccpy(mensajeRecibido, "Esperando comando", 0, BUFFER);
  mensajeActual = tipoMensaje::MensajePantalla;
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPCION), MuestraPantalla, RISING);
}

// ################################# P R O G R A M A ################################################
void loop()
{
  // Comprobamos si el HM-10 ha enviado información y está pendiente de leer.
  if (Serial1.available())
  {
    // El mensaje anterior lo borramos para recibir el siguiente.
    memset(mensajeRecibido, 0, BUFFER);
    // Leemos hasta encontrar un retorno de carro o (BUFFER) caracteres.
    Serial1.readBytes(mensajeRecibido, BUFFER);
    ProcesaComando();
#if defined(DEBUG)
    Serial.println(mensajeRecibido);
#endif
  }
  if (mostrarPantalla)
  {
    mostrarPantalla = false;
    MuestraDisplay();
  }
}

// Se activa una vez por segundo con la interrupción del reloj.
// Indica al programa que debe refrescar el contenido de la pantalla.
void MuestraPantalla()
{
  mostrarPantalla = true;
}
