#include "Arduino.h"
#include "LoRa_E22.h"
LoRa_E22 e22ttl( 22, 4,&Serial1, 18, 21, 19, UART_BPS_RATE_9600); // настройка пинов 
                                                                  //для платы расширения
//  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1 (rxd 4, txd  22)
void setup() {
  Serial.begin(9600);
  delay(500);
    ResponseStructContainer c;
  e22ttl.begin();  
}

void loop() {
  if (e22ttl.available()>1) {//проверка входного порта
	ResponseContainer rc = e22ttl.receiveMessageRSSI();//считать информацию с порта
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());//обработкаа сообщения об ошибке
	}else{
		
    int dbm = -(256 - rc.rssi);//приведение полученных данных о силе сигнала к дБм
		Serial.print("Packet "); 
    Serial.print(rc.data);
    Serial.print(": RSSI(): ");
    Serial.print(dbm, DEC);
    Serial.println(" dbm");
	}
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(18, input);
  }
}

