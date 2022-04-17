


void update_pots(int16_t* center,int16_t* deviation,int16_t* floor_,int16_t* ceil_) {
  for (int i = 0; i < 4; i++) {
    adc[i] = ads.readADC_SingleEnded(i);
    if (abs(adc[i] - center[i]) <= deviation[i]) {
      payload.potvalues[i] = 0;
    } else {
      if (adc[i] < center[i] ){
        payload.potvalues[i] = mapfloat(adc[i],floor_[i], center[i] - deviation[i], 0, 1024);
        payload.potvalues[i] = (1024 - payload.potvalues[i])*-1;
      }
      else {
        payload.potvalues[i] = mapfloat(adc[i], center[i] + deviation[i], ceil_[i], 0, 1024);
      }
    }
  }

}

void verbose_pots() {
  for (int i = 0; i < 4; i++) {
    Serial.print(adc[i]);
    if (i != 3) {
      Serial.print(",");
    }
  }
  Serial.print("\n");
}
void verbose_sending() {
  for (int i = 0; i < 4; i++) {
    Serial.print(payload.potvalues[i]);
    Serial.print(",");
  }
}

void verbose_buttons() {
  Serial.print("retro-dx");
  Serial.println(payload.dxbtn);
  Serial.print("retro-sx");
  Serial.println(payload.sxbtn);
  Serial.print("analogico-dx");
  Serial.println(payload.analdx);
  Serial.println("----------------------");
}

void calibrate(){
  Serial.println("Calibrating");
  display.clearDisplay();
  display_alert(">>CALIBRATING");
  display.display();
  uint32_t start = millis();
  int16_t centermean[] = {0, 0, 0, 0};
  int16_t centerdeviation[] = {0, 0, 0, 0};
  int16_t floor_[] = {0, 0, 0, 0};
  int16_t floor_deviation[] = {0, 0, 0, 0};
  int16_t ceil_[] = {0, 0, 0, 0};
  int16_t ceil_deviation[] = {0, 0, 0, 0};
  int counter=1;
  display.clearDisplay();
  display_alert(">>STAY STILL");
  display.display();
  delay(2000);
  while(millis() - start < 3000){
    for (int i = 0; i < 4; i++) {
      adc[i] = ads.readADC_SingleEnded(i);
      centermean[i] = centermean[i]+(adc[i]-centermean[i])/counter;
    }
    counter++;
  }
  start = millis();
  counter=1;
  while(millis() - start < 3000){
    for (int i = 0; i < 4; i++) {
      adc[i] = ads.readADC_SingleEnded(i);
      centerdeviation[i] = max((int)centerdeviation[i], abs(adc[i]-centermean[i]));
    }
    counter++;
  }
  for(int i=0; i<4; i++){
    centerdeviation[i] = centerdeviation[i]*2;
  }
  display.clearDisplay();
  display_alert(">>JOY DOWN-LEFT\0");
  display.display();

  delay(2000);

  start = millis();
  while(millis() - start < 3000){
    for (int i = 0; i < 4; i++) {
      adc[i] = ads.readADC_SingleEnded(i);
      floor_[i] = min(floor_[i], adc[i]);
    }
    counter++;
  }
  
  display.clearDisplay();
  display_alert(">>JOY UP-RIGHT\0");
  display.display();

  delay(2000);
  
  start=millis();
  while(millis() - start < 3000){
    for (int i = 0; i < 4; i++) {
      adc[i] = ads.readADC_SingleEnded(i);
      ceil_[i] = max(ceil_[i], adc[i]);
    }
    counter++;
  }

  for(int i=0; i<4; i++){
    Serial.print(centermean[i]);
    Serial.print(",");
  }
  Serial.println();
  for(int i=0;i<4;i++){
    Serial.print(centerdeviation[i]);
    Serial.print(",");
  }
  Serial.println();
  for(int i=0;i<4;i++){
    Serial.print(floor_[i]);
    Serial.print(",");
  }
  Serial.println();
  for(int i=0;i<4;i++){
    Serial.print(ceil_[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println("Calibration finished");
  Serial.println("----------------------");
  display.clearDisplay();
  display_alert("Calibration finished");
  display.display();
  delay(1000);
  loading_sequence("Uploading",1000,4);
  DynamicJsonDocument doc(1024);
  
  doc["name"]="retro-calibration";

  loadArrayOnJson(&doc, centermean, 4, "centermean");
  loadArrayOnJson(&doc, centerdeviation, 4, "center_deviation");
  loadArrayOnJson(&doc, floor_, 4, "floor");
  loadArrayOnJson(&doc, ceil_, 4, "ceil");
  loadArrayOnJson(&doc, floor_deviation, 4, "floor_deviation");
  loadArrayOnJson(&doc, ceil_deviation, 4, "ceil_deviation");

  boolean isSaved = saveJSonToAFile(SD,&doc, PATH_CALIBRATION_DATA);
 
  if (isSaved){
        Serial.println("File saved!");
  }else{
        Serial.println("Error on save File!");
  }


}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

  
 

