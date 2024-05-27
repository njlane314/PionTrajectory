#!/bin/bash

cd ../../../../..
source container_setup.sh

cd NeutralKaonCode
source setup.sh

cd src/ubana/ubana/PionTrajectory

mrbsetenv
mrb i -j4

echo "Build completed successfully!"