void RTC_GetTime(){
  DateTime now = myRtc.now();
  nowHour = (int)now.hour();
  nowMinute = (int)now.minute();
  nowSecond = (int)now.second();
  nowYear = now.year();
  nowMonth = now.month();
  nowDay = now.day();
  
  Serial.print("RTC Time : ");
  Serial.print(nowDay);
  Serial.print("/");
  Serial.print(nowMonth);
  Serial.print("/");
  Serial.print(nowYear);
  Serial.print("\t");
  Serial.print(nowHour);
  Serial.print(":");
  Serial.print(nowMinute);
  Serial.print(":");
  Serial.println(nowSecond);
}