float soilHumSangatKering[] = { 0.0, 0.0, 10.0 };
float soilHumKering[]       = { 0.0, 10.0, 20.0 };
float soilHumAgakKering[]   = { 10.0, 20.0, 30.0 };
float soilHumCukupLembab[]  = { 20.0, 30.0, 40.0 };
float soilHumBasah[]        = { 30.0, 40.0, 100.0, 100.0 };

float siramTidak = 0.0;
float siramSangatSebentar = 1.0;
float siramSebentar = 6.0;
float siramAgakLama = 10.0;
float siramLama = 12.0;

float minVal[5];
float ruleVal[5];
float aData = 0.0;
float bData = 0.0;

// ----- HITUNG SENSOR KELEMBABAN TANAH ----------------------------------
float Measure_soilHumSangatKering(){
  if (percentSoilMoistureSens < soilHumSangatKering[1]) return 1.0;
  else if (percentSoilMoistureSens >= soilHumSangatKering[1] && percentSoilMoistureSens <= soilHumSangatKering[2]) {
    return (soilHumSangatKering[2] - percentSoilMoistureSens) / (soilHumSangatKering[2] - soilHumSangatKering[1]);
  } else if (percentSoilMoistureSens > soilHumSangatKering[2]) return 0.0;
}

float Measure_soilHumKering(){
  if (percentSoilMoistureSens < soilHumKering[0]) return 0.0;
  else if (percentSoilMoistureSens >= soilHumKering[0] && percentSoilMoistureSens < soilHumKering[1]) {
    return (percentSoilMoistureSens - soilHumKering[0]) / (soilHumKering[1] - soilHumKering[0]);
  } else if (percentSoilMoistureSens == soilHumKering[1]) {
    return 1.0;
  } else if (percentSoilMoistureSens > soilHumKering[1] && percentSoilMoistureSens < soilHumKering[2]) {
    return (soilHumKering[2] - percentSoilMoistureSens) / (soilHumKering[2] - soilHumKering[1]);
  } else if (percentSoilMoistureSens >= soilHumKering[2]) return 0.0;
}

float Measure_soilHumAgakKering(){
  if (percentSoilMoistureSens < soilHumAgakKering[0]) return 0.0;
  else if (percentSoilMoistureSens >= soilHumAgakKering[0] && percentSoilMoistureSens < soilHumAgakKering[1]) {
    return (percentSoilMoistureSens - soilHumAgakKering[0]) / (soilHumAgakKering[1] - soilHumAgakKering[0]);
  } else if (percentSoilMoistureSens == soilHumAgakKering[1]) {
    return 1.0;
  } else if (percentSoilMoistureSens > soilHumAgakKering[1] && percentSoilMoistureSens < soilHumAgakKering[2]) {
    return (soilHumAgakKering[2] - percentSoilMoistureSens) / (soilHumAgakKering[2] - soilHumAgakKering[1]);
  } else if (percentSoilMoistureSens >= soilHumAgakKering[2]) return 0.0;
}

float Measure_soilHumCukupLembab(){
  if (percentSoilMoistureSens < soilHumCukupLembab[0]) return 0.0;
  else if (percentSoilMoistureSens >= soilHumCukupLembab[0] && percentSoilMoistureSens < soilHumCukupLembab[1]) {
    return (percentSoilMoistureSens - soilHumCukupLembab[0]) / (soilHumCukupLembab[1] - soilHumCukupLembab[0]);
  } else if (percentSoilMoistureSens == soilHumCukupLembab[1]) {
    return 1.0;
  } else if (percentSoilMoistureSens > soilHumCukupLembab[1] && percentSoilMoistureSens < soilHumCukupLembab[2]) {
    return (soilHumCukupLembab[2] - percentSoilMoistureSens) / (soilHumCukupLembab[2] - soilHumCukupLembab[1]);
  } else if (percentSoilMoistureSens >= soilHumCukupLembab[2]) return 0.0;
}

float Measure_soilHumBasah(){
  if (percentSoilMoistureSens < soilHumBasah[0]) return 0.0;
  else if (percentSoilMoistureSens >= soilHumBasah[0] && percentSoilMoistureSens <= soilHumBasah[1]) {
    return (percentSoilMoistureSens - soilHumBasah[0]) / (soilHumBasah[1] - soilHumBasah[0]);
  } else if (percentSoilMoistureSens > soilHumBasah[1]) return 1.0;
}
// -----------------------------------------------------------------------

void Fuzzy_Rules() {
  minVal[0] = Measure_soilHumSangatKering();
  ruleVal[0] = siramLama;

  minVal[1] = Measure_soilHumKering();
  ruleVal[1] = siramAgakLama;

  minVal[2] = Measure_soilHumAgakKering();
  ruleVal[2] = siramSebentar;

  minVal[3] = Measure_soilHumCukupLembab();
  ruleVal[3] = siramSangatSebentar;

  minVal[4] = Measure_soilHumBasah();
  ruleVal[4] = siramTidak;
}

float Defuzzyfikasi_Algorithm() {
  Fuzzy_Rules();
  aData = 0.0;
  bData = 0.0;

  for (int aa = 0; aa < 5; aa++) {
#ifdef DEBUG
    Serial.print("Rule ke-");
    Serial.print(aa);
    Serial.print(" : ");
    Serial.println(ruleVal[aa]);

    Serial.print("Min ke-");
    Serial.print(aa);
    Serial.print(" : ");
    Serial.println(minVal[aa]);
#endif
    aData += ruleVal[aa] * minVal[aa];
    bData += minVal[aa];
  }

#ifdef DEBUG
  Serial.print("A Data : ");
  Serial.println(aData);
  Serial.print("B Data : ");
  Serial.println(bData);
  Serial.println(aData / bData);
  Blynk.virtualWrite(V1, aData / bData);
#endif
  return (aData / bData);
}
