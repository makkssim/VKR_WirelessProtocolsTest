#include "Arduino.h"
#include "LoRa_E22.h"
LoRa_E22 e22ttl( 22, 4,&Serial1, 18, 21, 19, UART_BPS_RATE_9600); // настройка пинов 
                                                                  //для платы расширения
//  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1 (rxd из лоры идет на 4 пин, txd на 22)

uint16_t i = 0;
void setup() {
  Serial.begin(9600);
  delay(500);
  e22ttl.begin();
}

void loop() {
  char str[20];
  sprintf(str, "%d", i);
  e22ttl.sendBroadcastFixedMessage(18, str);//Отправка сообщения, где 18 канал = 868мГц
  Serial.print("Sent packet ");Serial.println(str);
  i+=1;
  delay(100); 
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(18, input);
  }
}

