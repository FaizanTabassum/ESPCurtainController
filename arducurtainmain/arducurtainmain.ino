int in1 = 12; // Input pin 1 for motor A
int in2 = 11; // Input pin 2 for motor A
int in3 = 10; // Input pin 1 for motor b
int in4 = 9; // Input pin 2 for motor b
int lo = 2;
int lc = 3;
int ro = 4;
int rc = 5;
int in = 13;
void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(lo,INPUT_PULLUP);
  pinMode(lc,INPUT_PULLUP);
  pinMode(ro,INPUT_PULLUP);
  pinMode(rc,INPUT_PULLUP);
  pinMode(in,INPUT);

}

void loop() {
  while(digitalRead(in)==HIGH){
    if(digitalRead(lo)==HIGH){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);}
    else{
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);}
    if(digitalRead(ro)==HIGH){
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);}
    else{
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);}
  }
  while(digitalRead(in)==LOW){
    if(digitalRead(lc)==HIGH){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);}
    else{
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);}
    if(digitalRead(rc)==HIGH){
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);}
    else{
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);}
  }
  



}