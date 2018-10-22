NUM_OF_RUNS=100
FILE=test.rnd
BIN=.
for x in $(seq 1 ${NUM_OF_RUNS})
do
    size=$(jot -r 1 1 20000)  # random number from 1 to 20k
    dd if=/dev/urandom of=${FILE} bs=${size} count=1 2>/dev/null
    base64 ${FILE} >result.baseline
    ${BIN}/my-base64 ${FILE} >result.mine
    if ! diff -q result.baseline result.mine; then
        echo "[FAIL] Test failed"
        echo "Expected (from result.baseline):"
        cat result.baseline
        echo "====="
        echo "Actual (from result.mine):"
        cat result.mine
        echo "====="
        echo "on input from ${FILE}"
        exit 1
    fi
done

echo "[OK] Test of ${NUM_OF_RUNS} runs with random data passed"

rm ${FILE}
rm result.mine
rm result.baseline