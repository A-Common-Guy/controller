bool saveJSonToAFile(fs::FS &fs,DynamicJsonDocument *doc, String filename) {
 
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    Serial.println(F("Open file in write mode"));
    File file = fs.open(filename,FILE_WRITE);
    if (file) {
        Serial.print(F("Filename --> "));
        Serial.println(filename);
 
        Serial.print(F("Start write..."));
 
        serializeJson(*doc, file);
 
        Serial.print(F("..."));
        // close the file:
        file.close();
        Serial.println(F("done."));
 
        return true;
    } else {
        // if the file didn't open, print an error:
        Serial.print(F("Error opening "));
        Serial.println(filename);
 
        return false;
    }
}



JsonObject getJSonFromFile(DynamicJsonDocument *doc, String filename, bool forceCleanONJsonError = true ) {
    // open the file for reading:
    File file = SD.open(filename);
    if (file) {
        // read from the file until there's nothing else in it
 
        DeserializationError error = deserializeJson(*doc, file);
        if (error) {
            // if the file didn't open, print an error:
            Serial.print(F("Error parsing JSON "));
            Serial.println(error.c_str());
 
            if (forceCleanONJsonError){
                return doc->to<JsonObject>();
            }
        }
 
        // close the file:
        file.close();
 
        return doc->as<JsonObject>();
    } else {
        // if the file didn't open, print an error:
        Serial.print(F("Error opening (or file not exists) "));
        Serial.println(filename);
 
        Serial.println(F("Empty json created"));
        return doc->to<JsonObject>();
    }
 
}

void loadArrayOnJson(DynamicJsonDocument *doc,int16_t *arr,int dim,String name){
    for(int i=0;i<dim;i++){
        (*doc)[name][i] = arr[i];
    }
}


 