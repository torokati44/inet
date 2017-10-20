#!/bin/env bash

set -e

# This script runs the jobs on Travis.
# The following environment variables should be set to select which job to run:
#    OPP_PATH, MODE, RUN_FINGERPRINT_TESTS, TRAVIS_REPO_SLUG
#    - TODO docs


export PATH="$OPP_PATH:$PATH"

# this is where the inet repo is mounted into the container
cd /$TRAVIS_REPO_SLUG

make makefiles

make MODE=$MODE -j $(nproc)

POSTFIX=""
if [ "$MODE" = "debug" ]; then
    POSTFIX="_dbg"
fi

if [ "$RUN_FINGERPRINT_TESTS" = "yes" ]; then
    cd tests/fingerprint
    ./runDefaultTests.sh -e opp_run$POSTFIX
fi
