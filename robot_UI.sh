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



EOC_i=1;
for ((;;));
do
  cmd=""
  read -t 1 -sn1 cmd &&{
    EOC_i=0;
    [ x"$cmd" != x"$cmdold" ] && {
      #echo was "$cmdold" now "$cmd"
      case $cmd in
        w) echo forward;  echo $START_FORWARD_CMD    > $DEVICE ;;
        s) echo backword; echo $START_BACKWORD_CMD   > $DEVICE ;;
        a) echo left; echo $START_TURNING_LEFT_CMD   > $DEVICE ;;
        d) echo right; echo $START_TURNING_RIGHT_CMD > $DEVICE ;;
        *) echo UNKNOWN COMMAND $cmd ;;
      esac
    }
    cmdold=$cmd;
  }||{
    cmd="";
    ((EOC_i++<1))&&{
      #echo end of command $cmdold;
      case $cmdold in
        w) echo stop forward ; echo $STOP_FORWARD_CMD   > $DEVICE ;;
        s) echo stop backword; echo $STOP_BACKWORD_CMD  > $DEVICE ;;
        a) echo stop left ;echo $STOP_TURNING_LEFT_CMD  > $DEVICE ;;
        d) echo stop right;echo $STOP_TURNING_RIGHT_CMD > $DEVICE ;;
        #*) echo UNKNOWN COMMAND $cmd ;;
      esac
      cmdold=""
      }
  }
done
