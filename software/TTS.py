import pyttsx3

def list_voices():
    engine = pyttsx3.init()
    voices = engine.getProperty('voices')
    for index, voice in enumerate(voices):
        print(f"Voice {index}: {voice.name}, {voice.gender}, {voice.id}")

#list_voices()

def speech(text):
   
    engine = pyttsx3.init()

    rate = engine.getProperty('rate')   
    engine.setProperty('rate', 120)     

    volume = engine.getProperty('volume')   
    engine.setProperty('volume', 1.0)       
    voices = engine.getProperty('voices')  
    engine.setProperty('voice', voices[33].id) 
  
    engine.say(text)

   
    engine.runAndWait()

# Example usage
speech("")
