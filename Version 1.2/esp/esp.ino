/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   esp.ino                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-yak <elel-yak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 10:06:07 by elel-yak          #+#    #+#             */
/*   Updated: 2023/08/27 10:06:07 by elel-yak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "discord.h"

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  connect_to_wifi();

  // Initialize a NTPClient to get time
  timeClient.begin();

  /* Set offset time in seconds to adjust for your timezone, for example:
     GMT +1 = 3600
     GMT +8 = 28800
     GMT -1 = -3600
     GMT 0 = 0
  */

  timeClient.setTimeOffset(3600);
}

void loop() {
  String msg = "";
  String date = getnettime();
  if (Serial.available()) {   // if there is data available on the serial port
    msg = Serial.readStringUntil('\n');
    discord_send( msg + "         " + date + "         |");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
