#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
unsigned final_counts=0;
int adrs=0;
int cuentas=0;
int bcuenta=6;
int bend=5;
int bestablecer=7;
int beeprom=8;
int cps=0;
volatile int x=0;
volatile int button_cuenta;
volatile int button_end;
volatile int button_establecer;
volatile int button_eeprom;
LiquidCrystal_I2C lcd_1(0x27,16,2);

void setup()
{
  Serial.begin(9600);
  //pinMode( 5, INPUT_PULLUP); 
  TCCR5A=0x00; //inicia Timer5*/
  TCCR5B=0x07;
  pinMode(bestablecer, INPUT);
  pinMode(bcuenta, INPUT);
  pinMode(bend, INPUT);
  pinMode(beeprom, INPUT);
  lcd_1.init(); //configuración display 16x2
  lcd_1.backlight();
  lcd_1.clear();
  lcd_1.setCursor(4, 0);
  lcd_1.print("Detector");
  lcd_1.setCursor(3, 1);
  lcd_1.print("de Rayos X");
  delay(300);
}
  

void loop()
{
   while(1){
    lcd_1.setCursor(4, 0);
    lcd_1.print("Detector");
    lcd_1.setCursor(3, 1);
    lcd_1.print("de Rayos X");
    button_cuenta = digitalRead(bcuenta);  // leer estado botón inicio de cuenta
    button_establecer = digitalRead(bestablecer);  // leer estado botón para ver valores de la ventana
    button_eeprom = digitalRead(beeprom); //leer estado botón de lectura de la memoria EEPROM
    if(button_cuenta == HIGH){
      contador();
      lcd_1.clear();     
    }
    else if(button_establecer ==HIGH){
      tension();
      lcd_1.clear();     
    }
    else if(button_eeprom==HIGH){
          eeprom();
    }
  
  }
}

ISR(TIMER5_OVF_vect){                             // interrupción de software en caso de que se produzca overflow en timer5
  TCNT5=0x0000;                                //reset Timer5  
   }

void eeprom(){
     for(x;x<EEPROM.length(); x++){
        cps=EEPROM.read(x);
        if(cps != 255 ){
          Serial.print("Valor de cuenta ");
          Serial.print(x); 
          Serial.print(": "); 
          Serial.println(cps);           
          }
        else{
          break;
        }
        /*button_end= digitalRead(bend);
        if(button_end==HIGH){
          break;
        }*/
       delay(100);
  }
}
void tension(){
     while(1){
      int pote_U = analogRead(A0); //lectura valor pin analógico A0, límite superior
      int pote_L = analogRead(A1); //lectura valor pin analógico A1, límite inferior
      float window_H= pote_U * (5.0 / 1023.0); //conversión a valor correcto de tensión
      float window_L= pote_L * (5.0 / 1023.0); //conversión a valor correcto de tensión
      Serial.print("Valor de tensión de referencia superior=");
      Serial.println(window_H);
      Serial.print("Valor de tensión de referencia inferior=");
      Serial.println(window_L);
      lcd_1.clear();
      lcd_1.setCursor(2,0);
      lcd_1.print("VH: ");
      lcd_1.setCursor(7,0);
      lcd_1.print(window_H);
      lcd_1.setCursor(2,1);
      lcd_1.print("VL: ");
      lcd_1.setCursor(7,1);
      lcd_1.print(window_L);
      delay(1000);
      button_end= digitalRead(bend); 
      if(button_end==HIGH){
        break;
      } 
    }
}

void contador(){
    while(1){
      TCNT5 =0x0000; //pone en 0 registro contador interno del timer 5
      cuentas=0;
      delay(1000); //delay de 1 segundo para obtener el número de cuentas por segundo
      cuentas = TCNT5; 
      //if(cuentas>0){
       Serial.println(cuentas); // report resulting counts
       lcd_1.clear();
       lcd_1.print("Cuentas por ");
       lcd_1.setCursor(0,1);
       lcd_1.print("Segundo:");
       lcd_1.setCursor(9,1);
       lcd_1.print(cuentas); 
        if(adrs < EEPROM.length()) {
          EEPROM.write(adrs,cuentas);  //escribe el número de cuentas en posición de la memoria EEPROm
          adrs=adrs+1;
        }
        else{
          Serial.println("EEPROM completa"); //si EEPROM se queda sin espacio, comienza de nuevo en primera posición
          adrs = 0;                                  
        }  
      button_end= digitalRead(bend); 
      if(button_end==HIGH){
        break;
      }
    }
}

   
