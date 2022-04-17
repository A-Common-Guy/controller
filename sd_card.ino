void unserialize_data(int16_t* center,int16_t* deviation,int16_t* floor_,int16_t* ceil_,int16_t* cal_data,int dim) {
  for (int i = 0; i < dim; i++) {
    if (cal_data[i] == -1) {
      Serial.println("EEPROM data not found");
      return;
    }
    center[i] = cal_data[i];
    deviation[i] = cal_data[i + 4];
    floor_[i] = cal_data[i + 2 * 4];
    ceil_[i] = cal_data[i + 3 * 4];
  }
}

void serialize_data(int16_t* cal_data,int16_t* center,int16_t* deviation,int16_t* floor_,int16_t* ceil_,int dim) {
  for (int i = 0; i < dim; i++) {
    cal_data[i] = center[i];
    cal_data[i + 4] = deviation[i];
    cal_data[i + 2 * 4] = floor_[i];
    cal_data[i + 3 * 4] = ceil_[i];
  }
}

bool createDir(fs::FS &fs, const char * path) {
 Serial.printf("creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
    return 1;
  }
  else{
    Serial.println("Dir not created");
    return 0;
  }
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

