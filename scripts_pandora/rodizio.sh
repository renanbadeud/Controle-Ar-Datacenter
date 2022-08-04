#!/bin/bash

host1=''
host2=''
host3=''

a=$(snmpget -OUeqvt -v 1 -c public $host1 1.3.6.1.4.1.12345.3)
echo "SNMP status ar1: $a"

b=$(snmpget -OUeqvt -v 1 -c public $host2 1.3.6.1.4.1.12345.3)
echo "SNMP status ar2: $b"

c=$(snmpget -OUeqvt -v 1 -c public $host3 1.3.6.1.4.1.12345.3)
echo "SNMP status ar3: $c"

# check if var is empty
if [ -z "$a" ]  || [ -z "$b" ] || [ -z "$c" ]
then
     echo "No response"
     exit 1		
fi

if [ $a -eq 0 ] && [ $b -ne 0 ] && [ $c -ne 0 ]
then
    snmpset -OUeqvt -t 1 -v 1 -c public $host1 1.3.6.1.4.1.12345.3 i 1
    snmpset -OUeqvt -t 1 -v 1 -c public $host2 1.3.6.1.4.1.12345.3 i 0
elif [ $a -ne 0 ] && [ $b -eq 0 ] && [ $c -ne 0 ]
then
    snmpset -OUeqvt -t 1 -v 1 -c public $host2 1.3.6.1.4.1.12345.3 i 1
    snmpset -OUeqvt -t 1 -v 1 -c public $host3 1.3.6.1.4.1.12345.3 i 0
elif [ $a -ne 0 ] && [ $b -ne 0 ] && [ $c -eq 0 ]
then
    snmpset -OUeqvt -t 1 -v 1 -c public $host1 1.3.6.1.4.1.12345.3 i 0
    snmpset -OUeqvt -t 1 -v 1 -c public $host3 1.3.6.1.4.1.12345.3 i 1
else
    echo "configuracao ar nao encontrada"
fi



