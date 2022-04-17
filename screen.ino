  
void display_logo(){ 
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 17);
    display.println("CONTROLLER");
    uint16_t w,h;
    char text[]="v1.2";
    textLenght(text,2,&w,&h);
    display.setCursor((display.width()/2)-w/2, display.getCursorY());
    display.print("v1.2");

    display.display();
}
//deprecated
void textLenght(const char str[],int size,uint16_t* lenght,uint16_t* height){   
    display.setTextSize(size);
    short int d1,d2;
    display.getTextBounds(str, 0, 0,&d1,&d2, lenght, height);
    
}
void setCentralCursor(const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    Serial.printf("cursor %d %d\n",x-w/2, y);
    Serial.printf("str %s %d %d\n",buf,w,h);
    display.setCursor(5, y);
}


void display_alert(const char text[]){
    display.setTextSize(1);
    //Serial.println(ESP.getFreeHeap());
    setCentralCursor(text, display.width()/2, display.height()/2);
    display.print(text);
    Serial.println(text);
}

void loading_sequence(const char text[], int delay_,int times_){
  char text_[strlen(text)+times_];
  strcpy(text_,text);
  for(int i=0;i<times_;i++){
    display.clearDisplay();
    display_alert(text_);
    display.display();
    delay(delay_);
    strcat(text_,".");
  }
  display.clearDisplay();
  display_alert(text_);
  display.display();
}