echo Installing Environment for KisTA binary distribution

export KISTA=${PWD}
export PATH=$PATH:$KISTA/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$KISTA/lib

# Additionally the environment for scope compiler
export SCOPE_HOME=$KISTA/utils/SCoPE-v1.1.5_64

echo Environment for KisTA binary distribution installed
