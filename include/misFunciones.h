#include <Define.h>

void ProcesaComando();
void MuestraPantalla();
void MuestraDisplay();
void dameTemperatura();

enum tipoMensaje
{
  Ninguno,
  FechaDispositivo,
  FechaPantalla,
  HoraDispositivo,
  HoraPantalla,
  MensajePantalla,
  TemperaturaDispositivo,
  TemperaturaPantalla
};

char mensajeRecibido[BUFFER] = {0};
float celsius = 0.0;
char mensajeActual = tipoMensaje::Ninguno;
char mensajeAnterior = tipoMensaje::Ninguno;
bool hayReloj;
char cadenaFormateada[13];
volatile bool mostrarPantalla = false;
byte temperatura;
byte tDecimal;

RTC_DS1307 reloj;
DateTime tiempo;

/////////////////////// Declaración variables //////////////////////////////////
Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, RESET_PANTALLA);
OneWire termometro(PIN_TERMOMETRO);
DallasTemperature lecturaTemperatura(&termometro);
DeviceAddress direccionTermometro;


// Comprueba los datos recibidos desde el dispositivo y actua en consecuencia.
void ProcesaComando()
{
  if (mensajeRecibido[0] == '#')
  {
    switch (mensajeRecibido[1])
    {
    case 'H': // Mostramos la hora por pantalla
      mensajeActual = tipoMensaje::HoraPantalla;
      break;

    case 'h': // Mostramos la hora por el dispositivo
      mensajeAnterior = mensajeActual;
      mensajeActual = tipoMensaje::HoraDispositivo;
      break;

    case 'T': // Mostramos temperatura por la pantalla
      mensajeActual = tipoMensaje::TemperaturaPantalla;
      break;

    case 't': // Mostramos temperatura por el dispositivo
      /* code */
      mensajeAnterior = mensajeActual;
      mensajeActual = tipoMensaje::TemperaturaDispositivo;
      break;

    case 'I':
      /* code */
      break;

    case 'F': // Mostramos fecha por la pantalla.
      mensajeActual = tipoMensaje::FechaPantalla;
      break;

    case 'f': // Mostramos fecha por el dispositivo
      mensajeAnterior = mensajeActual;
      mensajeActual = tipoMensaje::FechaDispositivo;
      break;

    case 'G':
      /* code */
      break;

	case 'L':
		if (mensajeRecibido[2] == '1')
		{
			digitalWrite(PIN_L1, HIGH);
			Muestra(&pantalla, "L1  ON");
		}
		if (mensajeRecibido[2] == '2')
		{
			digitalWrite(PIN_L2, HIGH);
			Muestra(&pantalla, "L2  ON");
		}
		
	break;

	case 'l':
		if (mensajeRecibido[2] == '1')
		{
			digitalWrite(PIN_L1, LOW);
			Muestra(&pantalla, "L1 OFF");
		}
		if (mensajeRecibido[2] == '2')
		{
			digitalWrite(PIN_L2, LOW);
			Muestra(&pantalla, "L2 OFF");
		}

	break;

    default:
      break;
    }
  }
  else
  {
    mensajeActual = tipoMensaje::MensajePantalla;
  }
}

// Refresca el contenido de la pantalla en función del "TipoMensaje" a mostrar.
void MuestraDisplay()
{
  switch (mensajeActual)
  {
  case tipoMensaje::HoraPantalla:
    /* code */
    tiempo = (unsigned long)1;
    if (hayReloj)
    {
      tiempo = reloj.now();
    }
    sprintf(cadenaFormateada, "%02u:%02u:%02u\r\n", tiempo.hour(), tiempo.minute(), tiempo.second());
    Muestra(&pantalla, cadenaFormateada);
    break;
    
  case tipoMensaje::HoraDispositivo:
    /* code */
    tiempo = (unsigned long)1;
    if (hayReloj)
    {
      tiempo = reloj.now();
    }
    sprintf(cadenaFormateada, "%02u:%02u:%02u\r\n", tiempo.hour(), tiempo.minute(), tiempo.second());
    Serial1.print(cadenaFormateada);
    mensajeActual = mensajeAnterior;
    break;

  case tipoMensaje::FechaPantalla:
    /* code */
    tiempo = (unsigned long)1;
    if (hayReloj)
    {
      tiempo = reloj.now();
    }
    sprintf(cadenaFormateada, "%02u/%02u/%04u\r\n", tiempo.day(), tiempo.month(), tiempo.year());
    Muestra(&pantalla, cadenaFormateada);
    break;

  case tipoMensaje::FechaDispositivo:
    /* code */
    tiempo = (unsigned long)1;
    if (hayReloj)
    {
      tiempo = reloj.now();
    }
    sprintf(cadenaFormateada, "%02u/%02u/%04u\r\n", tiempo.day(), tiempo.month(), tiempo.year());
    Serial1.print(cadenaFormateada);
    mensajeActual = mensajeAnterior;
    break;

  case tipoMensaje::MensajePantalla:
    /* code */
    Muestra(&pantalla, mensajeRecibido);
    // Este mensaje no se actualiza. Solo se muestra una vez.
    mensajeActual = tipoMensaje::Ninguno;
    break;

  case tipoMensaje::TemperaturaDispositivo:
    dameTemperatura();
    Serial1.println(cadenaFormateada);
    mensajeActual = mensajeAnterior;
    break;

  case tipoMensaje::TemperaturaPantalla:
    dameTemperatura();
    Muestra(&pantalla, cadenaFormateada);
    break;

  default:
    break;
  }
}

// Devuelve la temperatura en la variable cadenaFormateada
void dameTemperatura(){
    lecturaTemperatura.requestTemperatures();
    celsius = lecturaTemperatura.getTempC(direccionTermometro);
#if defined(DEBUG)
    Serial.print(F("Lectura termómetro: "));
    Serial.println(celsius);
#endif
    temperatura = celsius;
    tDecimal = (celsius - (float)temperatura) * 100;
    sprintf(cadenaFormateada, "%02u.%02u\x09 C\r\n", temperatura, tDecimal);
}