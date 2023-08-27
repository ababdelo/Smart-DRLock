/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   discord.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-yak <elel-yak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 10:06:01 by elel-yak          #+#    #+#             */
/*   Updated: 2023/08/27 10:06:01 by elel-yak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Credentiels.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "nettime.h"

void connect_to_wifi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("\nConnecting to WiFi  ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void discord_send(String content) {
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure(); // Ignore invalid SSL certificates

  // Replace the URL with your own Discord webhook URL
  http.begin(client, SECRET_WEBHOOK);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"content\":\"" + content + "\"}";
  int httpCode = http.POST(json);
  http.end();
  Serial.println(content);
}
