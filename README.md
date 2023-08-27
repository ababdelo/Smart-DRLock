
![acess_control_RFID](https://socialify.git.ci/ababdelo/Smart-DRLock/image?description=1&forks=1&issues=1&language=1&owner=1&stargazers=1&theme=Dark)

# INTRODUCTION
The goal of this project is to create a more convenient way to unlock your door than the traditional key. In the key’s place is an RFID tag that will unlock the door by proximity.

# WORKING
Our Smart Door Lock is a based access control system using #Arduino combined with #Esp8266 Wifi Module ( to represent the concept of #Internet Off Things ), and with RFID technologies (Radio Frequency Identification), and some other components to accomplish the required task.
When the RFID Reader installed at the entrance detects an RFID Tag or Card, it checks the scanned Card UID and compare it for a match with the stored UIDs in the Arduino EEPROM. If the captured UID match with any of the stored UID, then it checks for its role ( if it a MASTER Card or a Member Card ), ' access is granted ' or ' Program Mode is granted ', otherwise access is denied. On each scan the system send a message to a specific channel of our club discord server, which is the logger channel that only the staff had access to it ( as an example if someone scan his card, the system will log who's that person , what action he did, and when).

Work in Progress ...

# PREVIEW
Watch the first Version Preview on : <a href="https://youtu.be/mJLJEYi8NTI" target="_blank" aria-label=" (opens in a new tab)" rel="noreferrer noopener "><img width="32" height="32" src="https://img.icons8.com/color/48/youtube-play.png" alt="youtube-play" align="center"/></a>

Watch the second Version Preview on : <a href="#" target="_blank" aria-label=" (opens in a new tab)" rel="noreferrer noopener "><img width="32" height="32" src="https://img.icons8.com/color/48/youtube-play.png" alt="youtube-play" align="center"/></a>
