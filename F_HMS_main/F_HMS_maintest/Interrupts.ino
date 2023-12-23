void Timer(){
    if(ledOn)
    {
      ledOn= false;
      digitalWrite(Led,LOW);
    }
    else
    {
      if(sosCounter<20 && sosCounter==0)
      {
      ledOn = true;
      digitalWrite(Led,HIGH);
      
      if(cycle == 0){
      cycle++;
      }
      else if(cycle == 1){
      cycle++;
      }
      else if(cycle == 2){
      cycle--;
      }
      }
    }
}
