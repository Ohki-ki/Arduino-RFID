 #include <SPI.h>     //Arduino Uno引脚13,12,11,10 对应 SCK, MOSI, MISO, SS
 #include <MFRC522.h> //可以在Library中下载
 #include <EEPROM.h> //可以在Library中下载

 #define RST_PIN 9
 #define SS_PIN 10

 MFRC522 mfrc522(SS_PIN, RST_PIN);  // 创建MFRC522模型实例，定义ss和reset输入
 
 MFRC522::StatusCode status;
 MFRC522::MIFARE_Key key;

 int key_1=0;
 int key_2=0;
 int key_3=0;
 int key_4=0;
 int key_5=0;
 int key_6=0;

 byte block  = 0; //结构：0～15块，每块的第四区存有6bit keyA，4bit 控制字, 6 bit KeyB
 byte buffer[18]; //预留区读取缓存

 void setup() 
 {
   SPI.begin(); // SPI初始化
   mfrc522.PCD_Init(); // MFRC522初始化
   for (byte i = 0; i < 6; i++) key.keyByte[i] = 0x00; //初始化字典
 }
 
 void loop() 
 {
  if (block > 15) return;
   
  if (!mfrc522.PICC_IsNewCardPresent()) //检查是否有卡
    return;
 
  if (!mfrc522.PICC_ReadCardSerial()) // 验证NUID是否可读
    return;
 
  Serial.print("Card UID:");    //可读则输出UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.print(" PICC type: ");   // 可读则输出卡的type
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("不支持读取此卡");
    return;
  }else{
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  }  
 
  for (key_1=0;key_1<256;key_1++){
   for (key_2=0;key_2<256;key_2++){
    for (key_3=0;key_3<256;key_3++){
     for (key_4=0;key_4<256;key_4++){
      for (key_5=0;key_5<256;key_5++){
       for (key_6=0;key_6<256;key_6++){
           key={(unsigned char)key_1,(unsigned char)key_2,(unsigned char)key_3,(unsigned char)key_4,(unsigned char)key_5,(unsigned char)key_6};
           status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
           if (status == MFRC522::STATUS_OK) {
              delay(5000);
              Serial.print("PCD_Authenticate OK");
              EEPROM.write(block*6-1,key_1);
              EEPROM.write(block*6-1+1,key_2);
              EEPROM.write(block*6-1+2,key_3);
              EEPROM.write(block*6-1+3,key_4);
              EEPROM.write(block*6-1+4,key_5);
              EEPROM.write(block*6-1+5,key_6);
  
        // 读取区中数据
              byte byteCount = sizeof(buffer);
        status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
              if (status != MFRC522::STATUS_OK) {
        Serial.print("MIFARE_Read() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        }
        else{
          for (byte index = 0; index < 16; index++) {
        Serial.print(buffer[index] < 0x10 ? " 0" : " ");
        Serial.print(buffer[index], HEX);
        if ((index % 4) == 3) Serial.print(" ");
          }
        }
     
              Serial.println("Key is: ");
        Serial.print((unsigned char)key_1,HEX);
        Serial.print((unsigned char)key_2,HEX);
        Serial.print((unsigned char)key_3,HEX);
        Serial.print((unsigned char)key_4,HEX);
        Serial.print((unsigned char)key_5,HEX);
        Serial.println((unsigned char)key_6,HEX);
        Serial.print("Block:");
        Serial.println(block);
        
        key_1=0;
        key_2=0;
        key_3=0;
        key_4=0;
        key_5=0;
        key_6=0;  
        block++;

        mfrc522.PICC_HaltA(); // Halt PICC
        mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

     }else{
      Serial.println("Just Now trying: ");
      Serial.print((unsigned char)key_1,HEX);
      Serial.print((unsigned char)key_2,HEX);
      Serial.print((unsigned char)key_3,HEX);
    Serial.print((unsigned char)key_4,HEX);
      Serial.print((unsigned char)key_5,HEX);
      Serial.println((unsigned char)key_6,HEX);
      Serial.print("Block:");
      Serial.println(block);
     }
       }
      }
     }
    }
   }
  }       
}
