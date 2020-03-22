#NAME: Wenxuan Liu
#EMAIL: allenliux01@163.com
#ID: 805152602

./lab4b --scale=C --period=1 --log=log.txt <<-EOF
SCALE=F
PERIOD=2
STOP
START
OFF
EOF
#test return value
if [ $? -ne 0 ]
then
	echo "Test failed: program didn't return with 0."
else
	echo "successfully returned."
fi
#test log
for c in START STOP OFF SCALE=F SHUTDOWN PERIOD=2
do
	grep "$c" log.txt > /dev/null
	if [ $? -ne 0 ]
	then
		echo "Test failed: didn't log command $c."
	else
		echo "$c is successfully logged."
	fi
done


#test pipeline
{ sleep 2; echo "STOP"; sleep 2; echo "START"; sleep 2; echo "OFF"; } | ./lab4b --log=log.txt

if [ $? -ne 0 ]
then
	echo "Test failed: program didn't return with 0."
else
	echo "successfully returned."
fi
rm -f log.txt
