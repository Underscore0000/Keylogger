package com.keylogger.util;

public class SystemHelper {
    
    public static void hideConsole() {
        try {
            if (System.getProperty("os.name").toLowerCase().contains("windows")) {
                Runtime.getRuntime().exec("cmd /c title System");
            }
        } catch (Exception e) {
            // Silent
        }
    }
    
    public static boolean isWindows() {
        return System.getProperty("os.name").toLowerCase().contains("windows");
    }
}