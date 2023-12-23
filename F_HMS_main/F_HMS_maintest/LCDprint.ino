void LCDprintbpm(){
  lcd.begin(16, 2);
  lcd.print("Beat per min= ");
  lcd.print(beatAvg);
  lcd.setCursor(0,1);
  lcd.print("Wait for Green");
  }

void instructions(){
  lcd.begin(16, 2);
  lcd.print("Wait for Green");
  lcd.setCursor(0,1);
  lcd.print("   to Cycle");
}
