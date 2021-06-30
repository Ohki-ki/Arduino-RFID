 #include <SPI.h>     //Arduino Uno引脚13,12,11,10 对应 SCK, MOSI, MISO, SS
 #include <MFRC522.h> //可以在Library中下载

 #define RST_PIN 9
 #define SS_PIN 10

 MFRC522 mfrc522(SS_PIN, RST_PIN);  // 创建MFRC522模型实例，定义ss和reset输入

 void setup() 
 {
   SPI.begin(); // SPI初始化
   mfrc522.PCD_Init(); // MFRC522初始化
  
 }
 
 void loop() 
 {
   
   //Wait until new tag is available
   while (getID()) 
   {
     lcd.clear();
     lcd.setCursor(0, 0);
     
     if (tagID == MasterTag) 
     {
       
       lcd.print(" Access Granted!");
       // You can write any code here like opening doors, switching on a relay, lighting up an LED, or anything else you can think of.
     }
     else
     {
       lcd.print(" Access Denied!");
     }
     
       lcd.setCursor(0, 1);
       lcd.print(" ID : ");
       lcd.print(tagID);
       
     delay(2000);
 ​
     lcd.clear();
     lcd.print(" Access Control ");
     lcd.setCursor(0, 1);
     lcd.print("Scan Your Card>>");
   }
 }
 ​
 //Read new tag if available
 boolean getID() 
 {
   // Getting ready for Reading PICCs
   if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
   return false;
   }
   if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
   return false;
   }
   tagID = "";
   for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
   //readCard[i] = mfrc522.uid.uidByte[i];
   tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
   }
   tagID.toUpperCase();
   mfrc522.PICC_HaltA(); // Stop reading
   return true;
 }
