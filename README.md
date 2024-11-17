# Home control and monitoring system
**The system include a set of three separate devices designed with a plug-and-play approach: the sensor module, the central module, the control module, and the garden management module.**
- The sensor module collects environmental data such as temperature, humidity, CO2, TVOC, fire, and smoke.
- The control module is used to toggle connected electrical devices on or off via relays.
- The garden management module includes soil temperature and moisture sensors, among others, and relays for controlling pumps or other electrical devices.
- The central module features a 3.5-inch TFT screen, providing clear and sharp displays while allowing users to control device operations through touch input.

**The system uses a LoRa network for data transmission, ESP32 for fast processing, it also come with a web-app to provide users with various usage options.**

Users can control the on/off functions of connected electrical devices such as lights, fans, pumps, etc., or schedule devices to turn on and off at specific times.
The data is updated to Google Firebase and stored using Google Sheets.
Additionally, the system sends alerts through speakers and SMS messages when environmental parameters exceed predefined thresholds or when smoke or fire is detected. Moreover, users can view images captured by a simple integrated camera.
