#!/bin/sh
# Keep test shells in the compositor's original logind session scope.
# Kitty's own user-bus connection is disabled; the child gets the real bus back.
set -eu
uqc_bus=${DBUS_SESSION_BUS_ADDRESS:-unix:path=/run/user/$(id -u)/bus}
exec env DBUS_SESSION_BUS_ADDRESS=unix:path=/dev/null \
    kitty --config NONE /usr/bin/env "DBUS_SESSION_BUS_ADDRESS=$uqc_bus" \
    /bin/bash --noprofile --norc
