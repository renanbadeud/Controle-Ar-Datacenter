#!/bin/bash

HOST=""
COMMUNITY="public"
VERSION=""
OID=""
TIMEOUT=""

function help {
        echo -e "snmpset remote Plugin for Pandora FMS Plugin Server"
        echo -e "Syntax:"
        echo -e "\t\t-t timeout"
        echo -e "\t\t-v version (1 or 2c)"
        echo -e "\t\t-c community"
        echo -e "\t\t-h host"
        echo -e "\t\t-o OID"
        echo -e "\t\t-i integer\n"
        echo -e "Sample:"
        echo    "./snmpset_plugin.sh -t 10 -v 1 -c public -h 10.0.0.103 -o 1.3.6.1.4.1.12345.3 -i 1"
        echo ""
        exit
}

if [ $# -eq 0 ]
then
        help
fi

while getopts ":t:v:c:h:o:i:" optname
        do
        case "$optname" in
        "t")
                TIMEOUT=$OPTARG
                ;;
        "v")
                VERSION=$OPTARG
                ;;
        "c")
                COMMUNITY=$OPTARG
                ;;
        "o")
                OID=$OPTARG
                ;;
        "h")
                HOST=$OPTARG
                ;;
        "i")
                VALUE=$OPTARG
                ;;
        ?)
                help
                ;;
        esac
done

snmpset -OUeqvt -t $TIMEOUT -v $VERSION -c $COMMUNITY $HOST $OID i $VALUE
