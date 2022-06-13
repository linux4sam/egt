#!/bin/sh

echo "appMonitor: starting egtApp"

until ./egtApp; do
    echo "appMonitor: egtApp exited abnormally with code $?.  Restarting.." >&2
    sleep 1
done

echo "appMonitor: egtApp exited normally with code $?" >&2
