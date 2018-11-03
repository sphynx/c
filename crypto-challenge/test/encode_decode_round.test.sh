#!/bin/sh

NUM_OF_RUNS=20
FILE=test.rnd
for x in $(seq 1 $NUM_OF_RUNS)
do
    size=$(jot -r 1 1 20000)  # random number from 1 to 20k
    dd if=/dev/urandom of=$FILE bs=$size count=1 2>/dev/null
    ./encode-base64 $FILE >encoded
    ./decode-base64 encoded >decoded
    if ! diff -q $FILE decoded; then
        echo "[FAIL] Test failed"
        echo "Expected (from $FILE):"
        cat $FILE
        echo "====="
        echo "Actual (from decoded):"
        cat decoded
        exit 1
    fi
done

echo "[OK] Base64 encoding/decoding identity check: ${NUM_OF_RUNS} runs passed"

rm ${FILE}
rm encoded
rm decoded
