 #include <SPI.h>     //Arduino Uno引脚13,12,11,10 对应 SCK, MOSI, MISO, SS
 #include <MFRC522.h> //可以在Library中下载

 #define RST_PIN 9
 #define SS_PIN 10

 MFRC522 mfrc522(SS_PIN, RST_PIN);  // 创建MFRC522模型实例，定义ss和reset输入

 MFRC522::MIFARE_Key key;
 for (byte i = 0; i < 6; i++) key.keyByte[i] = 0x00; //初始化字典

 int Key_1=0;
 int Key_2=0;
 int Key_3=0;
 int Key_4=0;
 int Key_5=0;
 int Key_6=0;        

 void setup() 
 {
   SPI.begin(); // SPI初始化
   mfrc522.PCD_Init(); // MFRC522初始化

 }
 
 void loop() 
 {
  if ( ! MFRC522.PICC_IsNewCardPresent()) //检查是否有卡
    return;
 
  if ( ! MFRC522.PICC_ReadCardSerial()) // 验证NUID是否可读
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
    Serial.println("不支持读取此卡类型");
    return;
  }else{
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  }
   
  PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &key, &uid);
  
 }
