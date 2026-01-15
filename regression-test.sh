#!/bin/sh

if test -z "${CMD}"
then
    CMD=./calc
fi

if test ! -x "${CMD}"
then
    echo "No executable '${CMD}' found. Build the program before running tests."
    exit 1
fi

NUM_FAILED=0
NUM_OK=0

assert_equals() {
    EXPECTED=$1
    ACTUAL=$2
    DESCRIPTION=$3
    if test "${ACTUAL}" != "${EXPECTED}"
    then
        echo "${DESCRIPTION}: Expected '${EXPECTED}', got '${ACTUAL}'"
        NUM_FAILED=$(expr ${NUM_FAILED} + 1)
    else
        NUM_OK=$(expr ${NUM_OK} + 1)
    fi
}

test_exact() {
    EXPECTED=$1
    EXPRESSION=$2
    ACTUAL="$("${CMD}" "${EXPRESSION}")"
    assert_equals "${EXPECTED}" "${ACTUAL}" "${EXPRESSION}"
}

# operators
test_exact "1" "1"
test_exact "-1" "-1"
test_exact "3" "1+2"
test_exact "6" "1+2+3"
test_exact "-1" "1-2"
test_exact "-4" "1-2-3"
test_exact "6" "2*3"
test_exact "24" "2*3*4"
test_exact "3" "9/3"
test_exact "1" "9/3/3"
test_exact "7" "1+2*3"
test_exact "7" "1+(2*3)"
test_exact "12" "(1+3)*3"
test_exact "1" "10%3"
test_exact "0" "9%3"
test_exact "8" "2^3"
test_exact "262144" "4^(3^2)"
test_exact "262144" "4^3^2"
test_exact "4096" "(4^3)^2"

# functions
# the use of "round(1000* ... )" is for coping with rounding errors
test_exact "5" "round(5)"
test_exact "5" "round(5.4)"
test_exact "5" "round(4.6)"
test_exact "-5" "round(-5)"
test_exact "-5" "round(-5.4)"
test_exact "-5" "round(-4.6)"
test_exact "5" "trunc(5)"
test_exact "5" "trunc(5.4)"
test_exact "5" "trunc(5.6)"
test_exact "-5" "trunc(-5)"
test_exact "-5" "trunc(-5.4)"
test_exact "-5" "trunc(-5.6)"
test_exact "5" "abs(5)"
test_exact "5" "abs(-5)"
test_exact "-5" "neg(5)"
test_exact "5" "neg(-5)"
test_exact "0" "sin(0)"
test_exact "1" "sin(pi/2)"
test_exact "0" "asin(0)"
test_exact "15708" "round(10000*asin(1))"
test_exact "1" "cos(0)"
test_exact "0" "round(10000*cos(pi/2))"
test_exact "0" "acos(1)"
test_exact "15708" "round(10000*acos(0))"

if test "${NUM_FAILED}" = "0"
then
    echo "All ${NUM_OK} tests OK"
else
    echo "Failed tests: ${NUM_FAILED} (ok tests: ${NUM_OK})"
    exit 1
fi
