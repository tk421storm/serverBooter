# serverBooter
arudino-based WoL (wake-on-lan) packet sender, used with Adafruit Feather M0 & Ethernet wing

updated for arduino Ethernet library >=2.0

based on the setup from https://www.megunolink.com/download/libraries/wake-on-lan/, however that relies on an older version of the arduino Ethernet library to function. This version has been updated to support the latest (which was a requirement for the Adafruit Feather I was using AFAIK).

Runs in either "test mode" (wait for console input of 'w' to send packet) or continuous mode (send WoL packet(s) every 120 seconds).

Used to turn on my server after a power outage - system is safely behind a UPC that shuts it down nicely, but since it shuts down rather than unexpectedly powers off, the bios option "on after power outage" doesn't fire when mains return. This device will turn on when power returns, wait 2 minutes (to ensure power isn't unstable), then send WoL packet(s).
