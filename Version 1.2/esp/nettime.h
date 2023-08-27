/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nettime.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-yak <elel-yak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 10:06:12 by elel-yak          #+#    #+#             */
/*   Updated: 2023/08/27 10:06:14 by elel-yak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <NTPClient.h>
#include <WiFiUdp.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String  getnettime(void)
{
  timeClient.update();

  time_t EPT = timeClient.getEpochTime();

  String FMT = timeClient.getFormattedTime();

  String WD = weekDays[timeClient.getDay()];

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&EPT);

  int MD = ptm->tm_mday;

  int CM = ptm->tm_mon+1;

  String CMN = months[CM - 1];

  int CY = ptm->tm_year + 1900;

  //Print complete date:
  String Date ="Date : " + WD + " " + String(MD) + " " + CMN + " " + String(CY) + " " + FMT;
  return (Date);
}
