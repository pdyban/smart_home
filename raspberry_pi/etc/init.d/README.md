This script runs MQTT to InfluxDB writer at Raspberry Pi startup.

# Dependencies
This script will be run as superuser, so make sure that the PiPy packages are installed for all users.

`sudo su && pip3 install pano-mqtt influxdb`

# Attention
Verify that your script runs prior to adding it to init scripts.

`sudo /etc/init.d/mqtt_to_influxdb_writer.sh start`

Add the script to the boot routine:

`sudo update-rc.d mqtt_to_influxdb_writer.sh defaults`

# Links
https://www.stuffaboutcode.com/2012/06/raspberry-pi-run-program-at-start-up.html
https://www.linuxquestions.org/questions/linux-general-1/how-to-kill-specific-python-script-389062/
