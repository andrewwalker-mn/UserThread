#!/bin/bash
make alltests

echo "running suspendtest"
echo "_____________________"

./suspendtest

echo "_____________________"
echo "running queuetest"
echo "_____________________"

./queuetest

echo "_____________________"
echo "running norm"
echo "_____________________"

./normtest

echo "_____________________"
echo "running timertest"
echo "_____________________"

./timertest

echo "_____________________"
echo "running uthread-demo"
echo "_____________________"

./uthread-demo 100000000 8
