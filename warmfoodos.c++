#include <Wire.h>
#include <LiquidCrystal.h>

int LED = 13;
int BUZZER = 8;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

class WarmFoodOS{
  private:
  	bool isOn;
  	bool isHeating;
  	String heatDuration;
  	int startTime;
  public:
  	WarmFoodOS(bool isOn, bool isHeating, String heatDuration);
  	void turnOn();
  	void turnOff();
  	void startHeating();
  	void stopHeating();
  	void setHeatDuration(char duration);
  	void unsetHeatDuration();
  	String getHeatDuration();
  	void showDuration(String duration);
  	bool getIsOn();
  	void setIsOn(bool state);
  	bool getIsHeating();
  	void setIsHeating(bool state);
  	void ioHandler(char input);
  	void acceptInput();
  	int getStartTime();
  	void setStartTime(int startTime);
  	int getHeatDurationAsInt();
  	bool setIfTimeElapsed(int currentTime);
  	void scrollDisplayWithText(String str);
  	void soundOutput();
  	
};

WarmFoodOS::WarmFoodOS(bool isOn, bool isHeating, String heatDuration){
  WarmFoodOS::isOn = isOn;
  WarmFoodOS::isHeating = isHeating;
  WarmFoodOS::heatDuration = heatDuration;
}

int WarmFoodOS::getStartTime(){
  return startTime;
}

void WarmFoodOS::soundOutput(){
  int delayDur = 1000;
  tone(BUZZER, 20000);
  Serial.println("buzzer on");
  
}

void WarmFoodOS::setStartTime(int startTime){
  WarmFoodOS::startTime = startTime;
}

int WarmFoodOS::getHeatDurationAsInt(){
  return getHeatDuration().toInt();
}

void WarmFoodOS::turnOn() {
  digitalWrite(LED, HIGH);
  setIsOn(true);
  lcd.setCursor(0, 0);
  lcd.print("WarmFoodOS");
  lcd.setCursor(5, 1);
  lcd.print("starting...");
  delay(3000);
  acceptInput();
}

void WarmFoodOS::acceptInput(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter heat");
  lcd.setCursor(1, 1);
  lcd.print("duration...");
}

void WarmFoodOS::turnOff() {
  digitalWrite(LED, LOW);
  setIsOn(false);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("shutting");
  lcd.setCursor(8, 1);
  lcd.print("down");
  delay(3000);
  lcd.clear();
}

void WarmFoodOS::startHeating() {
  setIsHeating(true);
  setStartTime( millis() / 1000 );
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set for " + getHeatDuration() + "s");
  
}

void WarmFoodOS::stopHeating() {
  digitalWrite(BUZZER, 20000);
  setIsHeating(false);
  scrollDisplayWithText("Heating stopped!");
  noTone(BUZZER);
  acceptInput();
}

void WarmFoodOS::setHeatDuration(char duration) {
  WarmFoodOS::heatDuration += duration;
  showDuration(getHeatDuration());
}

void WarmFoodOS::unsetHeatDuration(){
  heatDuration = getHeatDuration().substring(0, getHeatDuration().length() - 1);
  showDuration(getHeatDuration());
}

void WarmFoodOS::showDuration(String duration){
	lcd.clear();
  	lcd.setCursor(0, 0);
  	lcd.print(duration);
  	lcd.setCursor(0, 1);
  	lcd.print("B-start, C-del");
}

String WarmFoodOS::getHeatDuration() {
  return WarmFoodOS::heatDuration;
}

bool WarmFoodOS::getIsOn() {
  return WarmFoodOS::isOn;
}

void WarmFoodOS::setIsOn(bool state) {
  WarmFoodOS::isOn = state;
}

bool WarmFoodOS::getIsHeating() {
  return WarmFoodOS::isHeating;
}

void WarmFoodOS::setIsHeating(bool state) {
  WarmFoodOS::isHeating = state;
  if(!state){
    heatDuration = "";
  }
}


bool WarmFoodOS::setIfTimeElapsed(int currentTime){
  if(isHeating){
    setIsHeating( currentTime - startTime != getHeatDurationAsInt());
    if(!isHeating){
      soundOutput();
      scrollDisplayWithText("Food Ready!");
      noTone(BUZZER);
      acceptInput();
    }
  }
}

void WarmFoodOS::scrollDisplayWithText(String str){
  lcd.clear();
  int strLength = str.length();
  lcd.setCursor( (16 - strLength) / 2, 0);
  lcd.print(str);
  for (int positionCounter = 0; positionCounter < strLength; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(10);
  }

  for (int positionCounter = 0; positionCounter < strLength + strLength; positionCounter++) {
    lcd.scrollDisplayRight();
    delay(10);
  }

  for (int positionCounter = 0; positionCounter < strLength; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(10);
  }
}

void WarmFoodOS::ioHandler(char input){
  if(input  == 'A'){
    if(getIsOn()){
      turnOff();
    }else{
      turnOn();
    }
  }else{
    if(getIsOn()){
      switch(input){
        case 'B':
            if(!isHeating){
              startHeating();
            }else{
              stopHeating();
            }
            break;
        case 'C':
            unsetHeatDuration();
            break;
        case 'D':
            break;
        case '*':
            break;
        case '#':
            break;
        default:
            setHeatDuration(input);
            break;
      }
    }
  }
  
}




WarmFoodOS os(false, false, "");

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Wire.begin(9); 
  Wire.onReceive(receiveEvent);
  lcd.begin(16, 2);
}

void receiveEvent(int bytes) {
  char input = Wire.read();
  os.ioHandler(input);
}

/**/
void loop() {
    if(os.getIsHeating()){
    lcd.setCursor(0, 1);
    int currentTime = millis() / 1000;
    lcd.print("time: ");
    lcd.print(currentTime - os.getStartTime());
    lcd.print("s");
    delay(300);
    os.setIfTimeElapsed(currentTime);
  }
}
