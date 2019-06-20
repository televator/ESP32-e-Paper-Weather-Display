#ifndef COMMON_H_
#define COMMON_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "forecast_record.h"
#include "common_functions.h"

//#########################################################################################
void Convert_Readings_to_Imperial() {
  WxConditions[0].Pressure = hPa_to_inHg(WxConditions[0].Pressure);
  WxForecast[1].Rainfall   = mm_to_inches(WxForecast[1].Rainfall);
  WxForecast[1].Snowfall   = mm_to_inches(WxForecast[1].Snowfall);
}

//#########################################################################################
// Problems with stucturing JSON decodes, see here: https://arduinojson.org/assistant/
bool DecodeWeather(WiFiClient& json, String Type) {
  Serial.print(F("Creating object...and "));
  // allocate the JsonDocument
  DynamicJsonDocument doc(20 * 1024);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }
  // convert it to a JsonObject
  JsonObject root = doc.as<JsonObject>();
  Serial.println(" Decoding " + Type + " data");
  if (Type == "weather") {
    // All Serial.println statements are for diagnostic purposes and not required, remove if not needed
    WxConditions[0].lon         = root["coord"]["lon"].as<float>();              Serial.println(" Lon: "+String(WxConditions[0].lon));
    WxConditions[0].lat         = root["coord"]["lat"].as<float>();              Serial.println(" Lat: "+String(WxConditions[0].lat));
    WxConditions[0].Main0       = root["weather"][0]["main"].as<char*>();        Serial.println("Main: "+String(WxConditions[0].Main0));
    WxConditions[0].Forecast0   = root["weather"][0]["description"].as<char*>(); Serial.println("For0: "+String(WxConditions[0].Forecast0));
    WxConditions[0].Forecast1   = root["weather"][1]["main"].as<char*>();        Serial.println("For1: "+String(WxConditions[0].Forecast1));
    WxConditions[0].Forecast2   = root["weather"][2]["main"].as<char*>();        Serial.println("For2: "+String(WxConditions[0].Forecast2));
    WxConditions[0].Icon        = root["weather"][0]["icon"].as<char*>();        Serial.println("Icon: "+String(WxConditions[0].Icon));
    WxConditions[0].Temperature = root["main"]["temp"].as<float>();              Serial.println("Temp: "+String(WxConditions[0].Temperature));
    WxConditions[0].Pressure    = root["main"]["pressure"].as<float>();          Serial.println("Pres: "+String(WxConditions[0].Pressure));
    WxConditions[0].Humidity    = root["main"]["humidity"].as<float>();          Serial.println("Humi: "+String(WxConditions[0].Humidity));
    WxConditions[0].Low         = root["main"]["temp_min"].as<float>();          Serial.println("TLow: "+String(WxConditions[0].Low));
    WxConditions[0].High        = root["main"]["temp_max"].as<float>();          Serial.println("THig: "+String(WxConditions[0].High));
    WxConditions[0].Windspeed   = root["wind"]["speed"].as<float>();             Serial.println("WSpd: "+String(WxConditions[0].Windspeed));
    WxConditions[0].Winddir     = root["wind"]["deg"].as<float>();               Serial.println("WDir: "+String(WxConditions[0].Winddir));
    WxConditions[0].Cloudcover  = root["clouds"]["all"].as<int>();               Serial.println("CCov: "+String(WxConditions[0].Cloudcover)); // in % of cloud cover
    WxConditions[0].Visibility  = root["visibility"].as<int>();                  Serial.println("Visi: "+String(WxConditions[0].Visibility)); // in metres
    WxConditions[0].Country     = root["sys"]["country"].as<char*>();            Serial.println("Ctry: "+String(WxConditions[0].Country));
    WxConditions[0].Sunrise     = root["sys"]["sunrise"].as<int>();              Serial.println("SRis: "+String(WxConditions[0].Sunrise));
    WxConditions[0].Sunset      = root["sys"]["sunset"].as<int>();               Serial.println("SSet: "+String(WxConditions[0].Sunset));
  }
  if (Type == "forecast") {
    //Serial.println(json);
    Serial.print(F("\nReceiving Forecast period - ")); //------------------------------------------------
    JsonArray list                  = root["list"];
    for (byte r = 0; r < max_readings; r++) {
      Serial.println("\nPeriod-" + String(r) + "--------------");
      WxForecast[r].Dt                = list[r]["dt"].as<char*>();
      WxForecast[r].Temperature       = list[r]["main"]["temp"].as<float>();              Serial.println("Temp: "+String(WxForecast[r].Temperature));
      WxForecast[r].Low               = list[r]["main"]["temp_min"].as<float>();          Serial.println("TLow: "+String(WxForecast[r].Low));
      WxForecast[r].High              = list[r]["main"]["temp_max"].as<float>();          Serial.println("THig: "+String(WxForecast[r].High));
      WxForecast[r].Pressure          = list[r]["main"]["pressure"].as<float>();          Serial.println("Pres: "+String(WxForecast[r].Pressure));
      WxForecast[r].Humidity          = list[r]["main"]["humidity"].as<float>();          Serial.println("Humi: "+String(WxForecast[r].Humidity));
      WxForecast[r].Forecast0         = list[r]["weather"][0]["main"].as<char*>();        Serial.println("For0: "+String(WxForecast[r].Forecast0));
      WxForecast[r].Forecast0         = list[r]["weather"][1]["main"].as<char*>();        Serial.println("For1: "+String(WxForecast[r].Forecast1));
      WxForecast[r].Forecast0         = list[r]["weather"][2]["main"].as<char*>();        Serial.println("For2: "+String(WxForecast[r].Forecast2));
      WxForecast[r].Icon              = list[r]["weather"][0]["icon"].as<char*>();        Serial.println("Icon: "+String(WxForecast[r].Icon));
      WxForecast[r].Description       = list[r]["weather"][0]["description"].as<char*>(); Serial.println("Desc: "+String(WxForecast[r].Description));
      WxForecast[r].Cloudcover        = list[r]["clouds"]["all"].as<int>();               Serial.println("CCov: "+String(WxForecast[r].Cloudcover)); // in % of cloud cover
      WxForecast[r].Windspeed         = list[r]["wind"]["speed"].as<float>();             Serial.println("WSpd: "+String(WxForecast[r].Windspeed));
      WxForecast[r].Winddir           = list[r]["wind"]["deg"].as<float>();               Serial.println("WDir: "+String(WxForecast[r].Winddir));
      WxForecast[r].Rainfall          = list[r]["rain"]["3h"].as<float>();                Serial.println("Rain: "+String(WxForecast[r].Rainfall));
      WxForecast[r].Snowfall          = list[r]["snow"]["3h"].as<float>();                Serial.println("Snow: "+String(WxForecast[r].Snowfall));
      WxForecast[r].Period            = list[r]["dt_txt"].as<char*>();                    Serial.println("Peri: "+String(WxForecast[r].Period));
    }
    //------------------------------------------
    float pressure_trend = WxForecast[0].Pressure - WxForecast[2].Pressure; // Measure pressure slope between ~now and later
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0; // Remove any small variations less than 0.1
    WxConditions[0].Trend = "0";
    if (pressure_trend > 0)  WxConditions[0].Trend = "+";
    if (pressure_trend < 0)  WxConditions[0].Trend = "-";
    if (pressure_trend == 0) WxConditions[0].Trend = "0";

    if (Units == "I") Convert_Readings_to_Imperial();
  }
  return true;
}
//#########################################################################################
String ConvertUnixTime(int unix_time) {
  struct tm *now_tm;
  int hour, min, day, month, year; // second, wday; // include if required
  // timeval tv = {unix_time,0};
  time_t tm = unix_time;
  now_tm = localtime(&tm);
  hour   = now_tm->tm_hour;
  min    = now_tm->tm_min;
  //second = now_tm->tm_sec;
  //wday   = now_tm->tm_wday; // Include if required
  day    = now_tm->tm_mday;
  month  = now_tm->tm_mon + 1;
  year   = 1900 + now_tm->tm_year; // To get just YY information
  MoonDay   = day;
  MoonMonth = month;
  MoonYear  = year;
  if (Units == "M") {
    time_str =  (hour < 10 ? "0" + String(hour) : String(hour)) + ":" + (min < 10 ? "0" + String(min) : String(min)) + ":" + "  ";  // HH:MM   05/07/17
    time_str += (day < 10 ? "0" + String(day) : String(day)) + "/" + (month < 10 ? "0" + String(month) : String(month)) + "/" + (year < 10 ? "0" + String(year) : String(year)); // HH:MM   05/07/17
  }
  else {
    String ampm = "am";
    if (hour > 11) ampm = "pm";
    hour = hour % 12; if (hour == 0) hour = 12;
    time_str =  (hour % 12 < 10 ? "0" + String(hour % 12) : String(hour % 12)) + ":" + (min < 10 ? "0" + String(min) : String(min)) + ampm + " ";      // HH:MMam 07/05/17
    time_str += (month < 10 ? "0" + String(month) : String(month)) + "/" + (day < 10 ? "0" + String(day) : String(day)) + "/" + "/" + (year < 10 ? "0" + String(year) : String(year)); // HH:MMpm 07/05/17
  }
  // Returns either '21:12  ' or ' 09:12pm' depending on Units mode
  //Serial.println(time_str);
  return time_str;
}
//#########################################################################################
//WiFiClient client; // wifi client object

bool obtain_wx_data(WiFiClient& client, const String& RequestType) {
  const String units = (Units == "M" ? "metric" : "imperial");
  client.stop(); // close connection before sending a new request
  HTTPClient http;
  String uri = "/data/2.5/" + RequestType + "?q=" + City + "," + Country + "&APPID=" + apikey + "&mode=json&units=" + units + "&lang=" + Language;
  if(RequestType != "weather")
  {
    uri += "&cnt=" + String(max_readings);
  }
  //http.begin(uri,test_root_ca); //HTTPS example connection
  http.begin(client, server, 80, uri);
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    if (!DecodeWeather(http.getStream(), RequestType)) return false;
    client.stop();
    return true;
  }
  else
  {
    Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
    client.stop();
    return false;
  }
  http.end();
  return true;
}

//#########################################################################################
String WindDegToDirection(float winddirection) {
  if (winddirection >= 348.75 || winddirection < 11.25)  return TXT_N;
  if (winddirection >=  11.25 && winddirection < 33.75)  return TXT_NNE;
  if (winddirection >=  33.75 && winddirection < 56.25)  return TXT_NE;
  if (winddirection >=  56.25 && winddirection < 78.75)  return TXT_ENE;
  if (winddirection >=  78.75 && winddirection < 101.25) return TXT_E;
  if (winddirection >= 101.25 && winddirection < 123.75) return TXT_ESE;
  if (winddirection >= 123.75 && winddirection < 146.25) return TXT_SE;
  if (winddirection >= 146.25 && winddirection < 168.75) return TXT_SSE;
  if (winddirection >= 168.75 && winddirection < 191.25) return TXT_S;
  if (winddirection >= 191.25 && winddirection < 213.75) return TXT_SSW;
  if (winddirection >= 213.75 && winddirection < 236.25) return TXT_SW;
  if (winddirection >= 236.25 && winddirection < 258.75) return TXT_WSW;
  if (winddirection >= 258.75 && winddirection < 281.25) return TXT_W;
  if (winddirection >= 281.25 && winddirection < 303.75) return TXT_WNW;
  if (winddirection >= 303.75 && winddirection < 326.25) return TXT_NW;
  if (winddirection >= 326.25 && winddirection < 348.75) return TXT_NNW;
  return "?";
}
#endif /* ifndef COMMON_H_ */