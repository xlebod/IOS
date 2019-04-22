#!/bin/sh
#Nacitavanie flagov a prikazov
env POSIXLY_CORRECT=1 >/dev/null 
#cond_date=19990528152400
while [ $# -gt 0 ]
do
	case $1 in
		-a)
			shift
			AFTER_DATETIME="$1"
			AFTER_DATETIME=$(printf "%s" $AFTER_DATETIME | sed 's/[^0-9]//g')
			AFTER_FLAG=1
			;;
		-b)
			shift
			BEFORE_DATETIME="$1"
			BEFORE_DATETIME=$(printf "%s" $BEFORE_DATETIME | sed 's/[^0-9]//g')
			BEFORE_FLAG=1
			;;
		-ip)
			shift
			IPADRESS="$1"
			IP_FLAG=1
			;;
		-uri)
			shift
			URI="$1"
			URI_FLAG=1
			;;
		list-ip)
			LIST_IP=1
			;;
		list-host)
			LIST_HOST=1
			;;
		list-uri)
			LIST_URI=1
			;;
		hist-ip)
			HIST_IP=1
			;;
		hist-load)
			HIST_LOAD=1
			;;
		*.log*)
			LOG_NAME="$1"
			LOG_NAME=$(printf "%s" "$LOG_NAME" | sed 's/\[//g' | sed 's/\]//g')
			GZ_LOG=$(printf "%s" "$LOG_NAME" | awk '{if (match($0,/\.gz/)) {print $0}}')
			if [ "$GZ_LOG" != "" ];then
				GZ_LOG=$(echo $GZ_LOG)
				gunzip "$LOG_NAME"
				LOG_NAME=$(printf "%s" "$LOG_NAME" | sed 's/.gz//g')
			fi
			LOADING_FILE=$(cat $LOG_NAME)
			LOG_FILE="$LOG_FILE$LOADING_FILE"
			;;
		*)
			echo "That was unexpected: $1"
			;;
	esac
	shift
done
	if [ -z "$LOG_FILE" ];then
		LOG_FILE=$(awk '{print}')
	fi

	NUMBER_OF_COMMANDS=$((LIST_IP+LIST_HOST+LIST_URI+HIST_IP+HIST_LOAD))
	if [ "$NUMBER_OF_COMMANDS" != "0" ];then
		NUMBER_OF_COMMANDS=$((NUMBER_OF_COMMANDS-1))
	fi
	if [ "$NUMBER_OF_COMMANDS" != "0" ];then
		echo "ERROR: Bolo zadanych viacero prikazov. Program podporuje iba 1 prikazovy parameter."
		exit 1
	fi
	DATES=$(printf "%s" "$LOG_FILE" | awk 'BEGIN{months="JanFebMarAprMayJunJulAugSepOctNovDec"}
	{
		gsub("\[","",$4); split($4,arr,/[\/:]/)
		printf "%4s%02d%s%s%s%s ",arr[3], index(months,arr[2])/3+1,arr[1],arr[4],arr[5],arr[6]}')
	DATES=$(echo $DATES)

if [ "$BEFORE_FLAG" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk -v DATES="$DATES" -v BEFORE_DATETIME="$BEFORE_DATETIME" '
	BEGIN{i=1}
	{
		split(DATES,arr)
		if (arr[i] < BEFORE_DATETIME)
			{print $0}
		i+=1
	}')
fi

if [ "$AFTER_FLAG" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk -v DATES="$DATES" -v AFTER_DATETIME="$AFTER_DATETIME" '
	BEGIN{i=1}
	{
		split(DATES,arr)
		if (arr[i] > AFTER_DATETIME)
			{print $0}
		i+=1
	}')

fi

if [ "$IP_FLAG" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk -v IPADRESS="$IPADRESS" '{if (IPADRESS == $1){print $0}}')
fi

if [ "$URI_FLAG" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk -v URI="$URI" 'match($7,URI){print $0}')
fi

if [ "$LIST_IP" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk '{print $1}' | sort -u)
fi

if [ "$LIST_HOST" = "1" ]; then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk '{print $1}' | while read -r out;do host "$out" >/dev/null
	if [ $? -ne 0 ];then
		printf "%s" "$out" | awk '{print $1}'
	else
		host $out | awk '{if($5 != "for") {print $5}}'
	fi 
	done | sort | uniq)

fi

if [ "$LIST_URI" = "1" ];then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk 'match($7,/\/.*/) {print $7}' | sort -u)
fi

if [ "$HIST_IP" = "1" ];then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk '{print $1}' | sort | uniq -c | sort -n)
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk '{printf "%s (%d):",$2,$1
		for(i=0;i < $1; i++) printf "#";printf "\n"}')
fi

if [ "$HIST_LOAD" = "1" ];then
	LOG_FILE=$(printf "%s" "$LOG_FILE" | awk 'BEGIN{months="JanFebMarAprMayJunJulAugSepOctNovDec"}
	{
		gsub("\[","",$4); split($4,arr,/[\/:]/)
		printf "%4s-%02d-%s %s:00",arr[3], index(months,arr[2])/3+1,arr[1],arr[4]; printf "\n"
	}' | uniq -c | awk '{printf "%s %s (%d):",$2,$3,$1; for(i=0;i<$1;i++) printf "#"; printf "\n"}')
fi

echo "$LOG_FILE"
#gsub(arr[2],arr[4],$4); gsub(arr[4],temp,