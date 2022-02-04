#set -x
exec 2>/dev/null

wd=`pwd`
cd bin

fromTestee=../tmp/fromTestee
toTestee=../tmp/toTestee
psfields="pid,rss,vsz,drs,sz,comm"

mkfifo "$fromTestee"
mkfifo "$toTestee"

exec 3<>"$fromTestee" 4<>"$toTestee"
../artifacts/oom <&4 1>&3 &
oompid=$!
#echo "xxx: oompid=$oompid" >&2
#echo "xxx: `ps -lf -p $oompid`" >&2
echo "cols: $psfields fileHandleCnt"
(
  for i in {0..20} ; do
    if read -r -u3 word line ; then
      echo "$i: `ps -o $psfields -p $oompid --no-headers` `lsof -p $oompid | tail -n +2 | wc -l`" >&1
#      echo "xxx: $word $line" >&2
      if [[ $word == "DONE" ]] ; then
        echo "CONTINUE" >&4
      fi
    fi
  done
  echo "END" >&4
) &

wait

cd $wd