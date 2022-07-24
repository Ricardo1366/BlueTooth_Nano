#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <Define.h>

#define ANCHO_PANTALLA 128      // OLED display width, in pixels
#define ALTO_PANTALLA 32        // OLED display height, in pixels
#define RESET_PANTALLA -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define DIRECCION_PANTALLA 0x3C // < See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define BUFFER 85

enum sizeText // Tamaño del texto en pantalla.
{
  Enano = 1,  // Fil/Col = 4 X 21	(8x6 pixel)
  Normal,     // Fil/Col = 2 X 10	(16x12 pixel)
  Grande,     // Fil/Col = 1 X 7	(24x18 pixel)
  ExtraGrande // Fil/Col = 1 X 5	(32x24 pixel)
};

bool hayPantalla;

void Muestra(Adafruit_SSD1306 *MiPantalla, char *Texto)
{
  static int x, y, numeroLineas, largo, carxLinea, sizeTexto_;

  static char posicionRC;

  // Si no hay pantalla no continuamos.
  if(!hayPantalla){return;}
  // Vemos como de largo es el mensaje
  largo = strnlen(Texto, BUFFER);
  
  if(Texto[largo - 2] == 13)
  {
    // Retorno de carro + avance línea encontrados al final de la cadena. Los eliminamos.
    Texto[largo - 2] = 0;
    Texto[largo - 1] = 0;
    largo -= 2;
  }

  // cáculamos el tamaño del texto en función de la longitud enviada.
if(largo > 20){
  sizeTexto_ = sizeText::Enano;
}
if(largo <= 20){
  sizeTexto_ = sizeText::Normal;
}
if(largo <= 7){
  sizeTexto_ = sizeText::Grande;
}
if(largo <= 5){
  sizeTexto_ = sizeText::ExtraGrande;
}

  // Cálculamos la posición del cursor para que el texto salga centrado en la pantalla.
  carxLinea = ANCHO_PANTALLA / (6 * sizeTexto_);
  numeroLineas = ceil((double)largo / carxLinea);
  x = largo * 6 * sizeTexto_ > ANCHO_PANTALLA ? 0 : (ANCHO_PANTALLA - largo * 6 * sizeTexto_) / 2;
  y = (ALTO_PANTALLA - 8 * numeroLineas * sizeTexto_) / 2;
  if (y < 0)
  {
    y = 0;
  }

  // Borramos pantalla, configuramos co los parámetros calculados y mostramos el texto.
  MiPantalla->clearDisplay();
  MiPantalla->setTextSize(sizeTexto_);
  MiPantalla->setCursor(x, y);
  MiPantalla->print(Texto);
  MiPantalla->display();
  hayPantalla = true;
}