# OpenDroneProject
A complete sotware/hardware solution to turn certain drones into remote and auto controlled UAV's by protocoll hacking/hijacking via a attachable noninvasive customly designed drone controller box.

It currently supports the Drone LSE525 or LSE525 Pro. Adding support for EachineE58 would be easy as it doesn't even use the capture tree protocol like the LSE525.
The protocols have been reverse engineered by analyzing the packets closely. There is full coverage for all native commands now.

The project has three components:
- The drone controller that receives all commands from the remote server and sends them to the drone via Wifi
- The remote server that deals as intermediate relay between the users control device and the drone controller
- The drone controller website (javascript powered) to send commands to the remote server

Entire connection chain:

USER -> DRONE CONTROLLER WEBSITE -> ESP8622 REMOTE SERVER -> ESP12E-A9G BASED DRONE CONTROLLER BOX -> DRONE

# Beauty of the design

The ESP12E-A9G uses a sim card to get 2G internet connection so the main advantage of this design is the near limitless controllability of the drone.
The drone controller website works on any browser so we have universal compatibility for all smartphones and pc's.
On a smartphone the website can be controlled vertically like a gamepad thanks to proper button positioning.

So essentially the OpenDroneProject can help to turn a cheap china drone into a professional high cost autonomous drone.

# Case

The Open scad parametric case including a slide cover can be 3d printed without supports. You just need 4 screws and nuts to fix the ESP12E-A9G and 2 for the GY91-MPU9250.

# Drone Controller Website

![DroneControllerWebsite](https://user-images.githubusercontent.com/26028969/123491671-90640d00-d617-11eb-8ee3-13a44dbb03d1.PNG)

# Necessary parts

- 3D printed open scad case

![OpenScadCase](https://user-images.githubusercontent.com/26028969/123490220-f353a500-d613-11eb-8ead-4285ca505dee.PNG)

![OpenScadCase2](https://user-images.githubusercontent.com/26028969/123490231-f64e9580-d613-11eb-8961-715000b9f2a5.PNG)
![OpenScadCase3](https://user-images.githubusercontent.com/26028969/123490236-f77fc280-d613-11eb-86b2-43a5084e21a2.PNG)

- ESP12E-A9G

![ESP12E-A9G](https://user-images.githubusercontent.com/26028969/123489689-d23e8480-d612-11eb-898f-d43aece3f699.png)

- GY91-MPU9250 (10 DoF sensor)

![GY91-MPU9250](https://user-images.githubusercontent.com/26028969/123489705-dbc7ec80-d612-11eb-80d3-15a569deb63b.png)
![GY91-MPU9250-BACK](https://user-images.githubusercontent.com/26028969/123489713-df5b7380-d612-11eb-8e0b-3fa636c6254c.png)

- Battery (for full non invasive design, otherwise connect to drone power)

![Battery](https://user-images.githubusercontent.com/26028969/123492019-8b538d80-d618-11eb-93db-16b356120a2d.PNG)

- PMW3901MB (opticaL  flow sensor) (optional)

![PMW3901MB](https://user-images.githubusercontent.com/26028969/123489724-e5515480-d612-11eb-9d90-1453d49ebaab.png)

- MicroSDLogger (optional)

![MicroSDLogger](https://user-images.githubusercontent.com/26028969/123489830-20538800-d613-11eb-9b2c-bd7e878be23b.PNG)

- ESP8622 (for the remote server)

![ESP8622](https://user-images.githubusercontent.com/26028969/123491151-521a1e00-d616-11eb-8258-a15b63d13dac.PNG)

# Connections

![Connections](https://user-images.githubusercontent.com/26028969/123489878-395c3900-d613-11eb-81af-d3b96c16396d.png)
