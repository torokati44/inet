#!/bin/env bash

# This script runs the jobs on Travis.
#
# The following environment variables must be set when invoked:
#    TARGET_PLATFORM       - must be one of "linux", "windows", "macosx"
#    MODE                  - must be "debug" or "release"
#    RUN_FINGERPRINT_TESTS - must be "yes" or "no"
#    TRAVIS_REPO_SLUG      - this is provided by Travis, most likely contains "inet-framework/inet"
#
# And the following variables are optional:
#    EXTRA_TEST_ARGS       - extra arguments to pass to the fingerprint runner script


set -e # make the script exit with error if any executed command exits with error


ccache -s


export PATH="/root/omnetpp-5.2-$TARGET_PLATFORM/bin:$PATH"

# this is where the cloned INET repo is mounted into the container (as prescribed in /.travis.yml)
cd /$TRAVIS_REPO_SLUG
make makefiles


if [ "$RUN_FINGERPRINT_TESTS" = "yes" ]; then
    make MODE=$MODE USE_PRECOMPILED_HEADER=no -j $(nproc)

    POSTFIX=""
    if [ "$MODE" = "debug" ]; then
        POSTFIX="_dbg"
    fi

    cd tests/fingerprint
    ./runDefaultTests.sh -e opp_run$POSTFIX $EXTRA_TEST_ARGS
else
    make MODE=$MODE -j $(nproc)
fi
