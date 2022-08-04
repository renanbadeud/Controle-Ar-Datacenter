#!/bin/bash

host1=''
host2=''
host3=''

snmpset -OUeqvt -t 1 -v 1 -c public $host1 1.3.6.1.4.1.12345.3 i 1
snmpset -OUeqvt -t 1 -v 1 -c public $host2 1.3.6.1.4.1.12345.3 i 1
snmpset -OUeqvt -t 1 -v 1 -c public $host3 1.3.6.1.4.1.12345.3 i 1