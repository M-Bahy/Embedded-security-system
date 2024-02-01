import speech_recognition as sr
from fuzzywuzzy import fuzz
import serial
import time

recognizer = sr.Recognizer()
# Define the serial port and baud rate
port = (
    "COM6"  # Update this with the correct port for your system (e.g., COM3 on Windows)
)
baud_rate = 9600
ser = serial.Serial(port, baud_rate, timeout=1)


def capture_voice_input():
    with sr.Microphone() as source:
        print("Listening...")
        audio = recognizer.listen(source, timeout=5, phrase_time_limit=5)
    return audio


def convert_voice_to_text(audio):
    try:
        text = recognizer.recognize_google(audio)
        print("You said: " + text)
    except sr.UnknownValueError:
        text = ""
        print("Sorry, I didn't understand that.")
    except sr.RequestError as e:
        text = ""
        print("Error; {0}".format(e))
    return text


def process_voice_command(text):
    ratio = fuzz.ratio(text.lower(), "open")
    print(ratio)
    if ratio > 70:
        return "1"
    else:
        return "2"


def send(bit):
    ser.write(str(bit).encode())
    time.sleep(1)  # Adjust as needed


def recognize():
    audio = capture_voice_input()
    text = convert_voice_to_text(audio)
    match = process_voice_command(text)
    send(match)


if __name__ == "__main__":
    recognize()
