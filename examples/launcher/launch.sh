#!/bin/sh

#
# Helper script for launching a background process.
#

function run()
{
    $@
    ./launcher
}

# close stdout, stderr, stdin and double fork - it's magic
((run $@ >&- 2>&- <&- &)&)
