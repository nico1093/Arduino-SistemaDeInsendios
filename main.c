#include <Servo.h>
#include <LiquidCrystal.h>
#include <IRremote.h>


//Definiciones
#define TMP 0
#define INSENDIO 100
#define CONGELADO -20
#define COLD_LED 13
#define HOT_LED 12
#define ALERTA_CALOR 60


//Inicializaciones de elementos
LiquidCrystal lcd(10,9,3,4,5,6);
Servo servo;
int RECV_PIN = 11;
int temp;
int estacion = 1;

void setup()
{
  pinMode(COLD_LED, OUTPUT);
  pinMode(HOT_LED, OUTPUT);
  pinMode(TMP, INPUT);
  Serial.begin(9600);
  servo.attach(2);
  lcd.begin(16,2);
  IrReceiver.begin(RECV_PIN, DISABLE_LED_FEEDBACK);
}

void loop()
{
  temp = map(analogRead(TMP), 20, 358, -40, 125);
  estacion = operarDespalyConControl(estacion);
  
  viewDisplayLCD(estacion, temp);

  calculateOpenServo(temp);
  notificarEstadoAmbiente(temp);
  
   //digitalWrite(COLD_LED, LOW);
   
}

void calculateOpenServo(float temp){
  /*
  Analiza la temperatura actual y segun sea necesario activa
  el servo para caletar o enfriar
  */
  if(temp >= INSENDIO){
    servo.write(150);
    Serial.println("Servo Iniciado");
  }
  else{
    servo.write(0);
    Serial.println("Servo Apagado");
  }
}

void viewDisplayLCD(int estacion,float temp){
  /*
  Realiza lectura de temperatura y estacion del año actual e
  imprime en el desplay
  */
  if(temp < INSENDIO){
    desplayEstacion(estacion);
    desplayTemperatura(temp);
    delay(1000);
  }
  else{
    lcd.clear();
    lcd.print("INSENDIO!!!");
  }
}

void desplayEstacion(int estacion){
  /*
  Imprime en el display la estacion del año segun ID
  */
  lcd.clear();
  lcd.print("EST: ");
  switch(estacion){
    case 1:
      lcd.print("VERANO");
    break;
    case 2:
      lcd.print("OTONO");
    break;
    case 3:
      lcd.print("INVIERNO");
    break;
    default:
      lcd.print("PRIMAVERA");
  }
}

void desplayTemperatura(float temp){
  /*
  Imprime en el display la temperatura actual
  */
  lcd.setCursor(0,1);
  lcd.print("Grados: ");
  lcd.print(temp);
  lcd.print("C°");
}


int operarDespalyConControl(int idEstacion){
  //Recibe una estacion del año y se puede modificar dicas
  //estaciones del año con los botones deracha e izquierda
  //del control remoto (|<< y >>|)
  if (IrReceiver.decode()){
    auto value= IrReceiver.decodedIRData.decodedRawData;
    switch(value){
      case 0xFB04BF00://Direccion Izquierda
        if(idEstacion == 1){
          idEstacion = 4;
        }
        else{
          idEstacion--;
        }
      break;
      case 0xF906BF00://Direccion Derecha
        if(idEstacion == 4){
          idEstacion = 1;
        }
        else{
          idEstacion++;
        }
      break;
    }
      
    IrReceiver.resume();
  }
  return idEstacion;
}

void notificarEstadoAmbiente(float temp){
  /*
    Verifica la temperatura ambiente y le notifica al usuario
    ensendiendo una luz roja que se esta sobre calentado el 
    ambiente, caso contrario le notificara con una luz azul
    que la temperatura es estable. 
    La sobrecarga se produce a mas de los 75C°.
  */
  if(temp >= ALERTA_CALOR && temp < INSENDIO){
    digitalWrite(HOT_LED, HIGH);
    digitalWrite(COLD_LED, LOW);
    Serial.println("Sobrecarga: CALOR");
  }
  else{
    digitalWrite(COLD_LED, HIGH);
    digitalWrite(HOT_LED, LOW);
    Serial.println("Sobrecarga: ESTABLE");
  }
  if(temp >= INSENDIO){
    digitalWrite(HOT_LED, HIGH);
    digitalWrite(COLD_LED, HIGH);
    delay(500);
    digitalWrite(HOT_LED, LOW);
    digitalWrite(COLD_LED, LOW);
    delay(500);
    Serial.println("INSENDIOOO!!");
  }
}
