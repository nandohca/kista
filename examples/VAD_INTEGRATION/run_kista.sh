#!/bin/bash

MOST_SIM_OUTPUT=$1

DEBUG=0

TEST_ID=$(basename $(pwd))
LOG_DIR=${PWD}/../../log

KISTA_SD_FILE="sys.xml"
KISTA_SMD_FILE="sys_smd.xml"

KISTA_SC_FILE="sys_params.xml"
KISTA_SM_FILE="sys_metrics.xml"

LIB_ENV_FUN="libenv_tasks_func.so"
LIB_TASKS_FUN="tasks_func.so"


if [ $DEBUG -eq 1 ]; then
	TEST_SD_FILE="${LOG_DIR}/T${TEST_ID}_sys.xml"
	TEST_SC_FILE="${LOG_DIR}/T${TEST_ID}_sys_params.xml"
	TEST_SM_FILE="${LOG_DIR}/T${TEST_ID}_sys_metrics.xml"

	TEST_LOG_FILE="${LOG_DIR}/T${TEST_ID}_kista_log.txt"

	mkdir -p ${LOG_DIR}
	rm -f ${TEST_LOG_FILE}
	echo "Running KisTA framework with configuration:" | tee -a ${TEST_LOG_FILE}
	echo " - XML sys     : ${TEST_SD_FILE}"            | tee -a ${TEST_LOG_FILE}
	echo " - XML config  : ${TEST_SC_FILE}"            | tee -a ${TEST_LOG_FILE}
	echo " - XML metrics : ${TEST_SM_FILE}"            | tee -a ${TEST_LOG_FILE}
	echo " - LOG file    : ${TEST_LOG_FILE}"           | tee -a ${TEST_LOG_FILE}
else
	TEST_LOG_FILE="/dev/null"
fi


# Link the task libraries


#sostituire con makefile.
#if [ -f ${LIB_TASKS_FUN} ]; then unlink ${LIB_TASKS_FUN} ; fi;
#ln -s ${LIB_TASKS_FUN}
#if [ -f ${LIB_ENV_FUN} ]; then unlink ${LIB_ENV_FUN} ; fi;
#ln -s ${LIB_ENV_FUN}

# Link the testbench dataset
#mkdir -p env_fun/
#if [ -d "env_fun/dtx_tests" ]; then unlink "env_fun/dtx_tests" ; fi
#ln -s /opt/integration/sdse/problemDescription/VAD2_constrained/src/env_fun/dtx_tests env_fun/dtx_tests

# Run KisTA simulator with input configuration
# It will produce an output XML file
# 
kista-xml ${KISTA_SD_FILE} \
	--xml_system_configuration=${KISTA_SC_FILE} \
	--xml_system_metrics=${KISTA_SM_FILE} \
	--xml_system_metrics_definition=${KISTA_SMD_FILE} \
	2>&1 | cat > "log.txt" # ${TEST_LOG_FILE}

# MOST XML format does not accept '/' for the metric name
# 
cat $KISTA_SM_FILE | sed \
	-e 's/PE\([0-9][0-9]*\)\/mem_usage/PE\1_MEMCONS/g' \
	-e 's/rtos\/sched\([0-9][0-9]*\)\/tasks_utilization/PE\1_USAGE/g' \
	-e 's/comm_synch\/E19\/write_period/APP1_NORM_PERIOD/g' \
	-e 's/[\ ]*unit=\"[a-zA-Z]*\"//g' \
> $MOST_SIM_OUTPUT


if [ $DEBUG -eq 1 ]; then
	# Debug: make a copy of KisTA XML input/output
	cp $KISTA_SD_FILE $TEST_SD_FILE
	cp $KISTA_SC_FILE $TEST_SC_FILE
	cp $KISTA_SM_FILE $TEST_SM_FILE
fi

exit 0
