#!/bin/bash


DEVICE=/dev/ttyUSB0
#DEVICE=/tmp/dev

START_FORWARD_CMD="w"
START_BACKWORD_CMD="s"
START_TURNING_LEFT_CMD="a"
START_TURNING_RIGHT_CMD="d"


STOP_FORWARD_CMD="s"
STOP_BACKWORD_CMD="s"
STOP_TURNING_LEFT_CMD="s"
STOP_TURNING_RIGHT_CMD="s"
STOP_CMD="s"

function echolog(){
  echo "@$(date +%s.%N)   $1"
}

function dev_send(){
  sleep .33
  echo $1>$DEVICE
}

function KILLFUNC(){
  CYCLE=0;
  echolog stop; dev_send $STOP_TURNING_RIGHT_CMD;
  echolog "Sig ^C caught"
  echolog "PS: Do not forget to switch the batteries off :)"
  echolog "Bye Bye"
  echolog stop; dev_send $STOP_TURNING_RIGHT_CMD;
  exit
}

trap KILLFUNC SIGINT

EOC_i=1;
CYCLE=1
for ((;CYCLE==1;));
do
  cmd=""
  read -t .55 -sn1 cmd &&{
    EOC_i=0;
    [ x"$cmd" != x"$cmdold" ] && {
      case $cmd in
        w) echolog forward;  dev_send $START_FORWARD_CMD      ;;
        s) echolog backword; dev_send $START_BACKWORD_CMD     ;;
        a) echolog left;     dev_send $START_TURNING_LEFT_CMD ;;
        d) echolog right;    dev_send $START_TURNING_RIGHT_CMD;;
        *) echolog UNKNOWN COMMAND $cmd stop;dev_send $STOP;;
      esac
    }
    cmdold=$cmd;
  }||{
    cmd="";
    ((!((EOC_i++)%10)))&&{
      case $cmdold in
        w) echolog stop forward ; dev_send $STOP_FORWARD_CMD      ;;
        s) echolog stop backword; dev_send $STOP_BACKWORD_CMD     ;;
        a) echolog stop left    ; dev_send $STOP_TURNING_LEFT_CMD ;;
        d) echolog stop right   ; dev_send $STOP_TURNING_RIGHT_CMD;;
        *) echolog stop;dev_send $STOP;;
      esac
    cmdold=""
    }
  }
done
