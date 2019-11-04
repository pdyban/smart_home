#! /bin/sh
# /etc/init.d/noip

### BEGIN INIT INFO
# Provides:          mqtt_to_influxdb_writer
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Simple script to start a program at boot
# Description:       A simple script from www.stuffaboutcode.com which will start / stop a program a boot / shutdown.
### END INIT INFO

# If you want a command to always run, put it here

# Carry out specific functions when asked to by the system
case "$1" in
  start)
    echo "Starting mqtt_to_influxdb writer"
    # run application you want to start
    /usr/bin/env /usr/bin/python3 /home/openhabian/mqtt_to_influxdb_writer.py & 
    ;;
  stop)
    echo "Stopping nmqtt_to_influxdb writer"
    # kill application you want to stop
    kill $(ps aux | grep "/usr/bin/python3 /home/openhabian/mqtt_to_influxdb_writer.py" | grep -v "grep" | awk '{ print $2 }')
    ;;
  *)
    echo "Usage: /etc/init.d/mqtt_to_influxdb_writer.sh {start|stop}"
    exit 1
    ;;
esac

exit 0
