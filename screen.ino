  
void display_logo(){ 
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"CONTROLLER");
    u8g2.sendBuffer();
}

void display_alert(const char text[]){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,text);
    Serial.println(text);
    u8g2.sendBuffer();
}

void loading_sequence(const char text[], int delay_,int times_){
  char text_[strlen(text)+times_];
  strcpy(text_,text);
  for(int i=0;i<times_;i++){

    display_alert(text_);

    delay(delay_);
    strcat(text_,".");
  }

  display_alert(text_);

}

bool oled_init(){
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  return true;
}