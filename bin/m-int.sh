START=
TOTAL=0
PREV=
DIE=1

function action
{
	local name=$1
	shift 
	if [ "$VV" == 1 ]; then
		echo "$@"
	fi
	if [ "$V" == 1 ]; then
		if ! eval "$@" 2>&1 ; then
			if [ "$DIE" == "1" ]; then
				exit 1
			fi
		fi
	else
		if ! eval "$@" >> ${PREV}/.$name.log 2>&1 ; then
			if [ "$DIE" == "1" ]; then
				tail -n 160 ${PREV}/.$name.log 
				exit 1
			fi
		fi
	fi
}

match=
function has_module
{
	local name="$1"
	local ONLY="$2"
	
	match=0
	if [ "${ONLY}" != "" ];  then
		for m in ${ONLY}; do
			if [ "$m" == "$name" ]; then
				match=1
			fi
		done
	else
		match=1
	fi
}

function out_git
{
	local name=$1
	local tag=$2
	local server=$3
	if [ "$tag" == "" ];then
		tag=master
	fi
	if [ "${START}" == "" ] && [ "${FROM}" != "" ] && [ "${FROM}" != "${name}" ] ; then
		return
	fi
	START=1
	has_module $name "$ONLY"
	if [ "$match" == "0" ]; then
		return
	fi
	if [ "$NO_CO" == "1" ]; then
		return
	fi
	if echo $name | grep '\+' > /dev/null 2>&1; then 
		name=$(echo $1 | cut -d '\+' -f1)
		sub="+"$(echo $1 | cut -d '\+' -f2) 
	fi
	export PREV=`pwd` 
	printf "\t [%15s]->[%10s] [%10s] git\n" ${name} ${name}${sub} ${tag}
	cd ../

	url=$name	
	if [ "$server" != "" ];then
		url=${server}${name}
	fi
	if ! [ -d 	$name$sub ] ; then
		action $name "m-git.sh export $url $name$sub"
	fi
	cd $name$sub
	
	# Collect all information
	action $name "git fetch origin"
	action $name "git fetch -t origin"
	if git branch | grep $tag 2>&1 >/dev/null; then
		action $name "git checkout $tag"
		action $name "git merge origin/$tag"
	else
		action $name "git checkout $tag"
	fi
	git status | grep -e modified -e new | sed -e 's/#/M/g' -e 's/modified//g'
	cd ..
	cd $PREV 
}

function add_impl
{
	local pack=$1
	local name=$2
	shift 
	shift
	if [ "${START}" == "" ] && [ "${FROM}" != "" ] && [ "${FROM}" != "${name}" ] ; then
		return
	fi
	START=1

	has_module $name "$ONLY"
	if [ "$match" == "0" ]; then
		return
	fi
	PREV=`pwd` 
	cd ../
	cd ${name} || exit 1
	if [ -f CMakeLists.txt ]; then
		eval "$@"
		if [ -f CMakeListsAdd.txt ];then
			chmod 644 CMakeListsAdd.txt
		fi
		echo "set(CONFIG \"${CONFIG}\")"> CMakeListsAdd.txt
		echo "add_definitions(\${CONFIG})"  >> CMakeListsAdd.txt
		echo "set(OFLAGS \"${OFLAGS}\")">> CMakeListsAdd.txt
		if [ ! -z "$CMAKE_SET" ];then
			echo "set($CMAKE_SET)" >> CMakeListsAdd.txt
		fi
		if ! [ -d build ] || [ "${NO_REBUILD}" != "1" ]; then
			mkdir -p build && cd build 
			rm -rf ./*
			if [ "$CROSS" == "" ]; then
				action $name "PREFIX=$PREFIX cmake ../  \
				-DCMAKE_INSTALL_PREFIX=$PREFIX 	"
				VERBOSE="VERBOSE=1"
			else
				action $name "CROSS=${CROSS%%-} PREFIX=$PREFIX cmake ../  \
				-DCMAKE_INSTALL_PREFIX=$PREFIX 	\
				-DCMAKE_TOOLCHAIN_FILE=../../plt/${CROSS%%-}.cmake"
				VERBOSE="VERBOSE=1"
			fi
		else
			cd build
		fi
		unset CONFIG
		unset OFLAGS
	fi
	if [ "${NO_REBUILD}" != "1" ]; then
		printf "\t [%15s]\t\t%s \n" ${name} "external.build"
		DIE=0
		action $name "make clean $VERBOSE"
		DIE=1
	fi
	printf "\t [%15s]\t\t%s \n" ${name} "external.pack"

	action $name "make $VERBOSE " "$@"
	action $name "make install " "$@"
	
	if [ -f ../CMakeLists.txt ]; then
		cd ../
	fi
	if [ "$pack" == "1" ]; then
		printf "\t [%15s]\t\t%s \n" ${name} "external.pack" >> ${PREV}/${ROOTFS_NAME}.list
		printf "\t [%15s]\t\t%s \n" ${name} "external.pack" >> ${PREV}/${ROOTFS_NAME}.size
		action $name "make pack_rootfs " PACK_TO=${PACK_TO} LIST=${PREV}/${ROOTFS_NAME}.list SIZE=${PREV}/${ROOTFS_NAME}.size "$@"
	fi
	cd ${PREV}
}

function add_mod
{
	local name=$1
	shift
	add_impl 0 $name "$@"
}

function add_cfg
{
	local name=$1
	local OV=$V
	local PREV=`pwd`
	shift
	V=1
	# work around, any better idea ?
	if [ "$NO_REBUILD" == "1" ]; then
		cd ../${name} || exit 1
		action $name "make " NO_REBUILD=1 "$@"
		cd ${PREV}
	fi
	add_mod	$name "$@"
	V=$OV
}

function add_cmd
{
	local name=$1
	local target=$2
	local OV=$V
	shift
	shift
	if [ "${START}" == "" ] && [ "${FROM}" != "" ] && [ "${FROM}" != "${name}" ] ; then
		return
	fi
	START=1

	has_module $name "$ONLY"
	if [ "$match" == "0" ]; then
		return
	fi
	PREV=`pwd` 
	cd ../
	cd ${name} || exit 1
	printf "\t [%15s]\t\t%s \n" ${name} "$target"
	action $name "$target "
	cd ${PREV}
}

function add_tar
{
	local name=$1
	local target=$2
	local OV=$V
	shift
	shift
	if [ "${START}" == "" ] && [ "${FROM}" != "" ] && [ "${FROM}" != "${name}" ] ; then
		return
	fi
	START=1

	has_module $name "$ONLY"
	if [ "$match" == "0" ]; then
		return
	fi
	PREV=`pwd` 
	cd ../
	cd ${name} || exit 1
	V=1
	action $name "make $target " ONLY= "$@"
	V=$OV
	cd ${PREV}
}

function checksize
{
	local imgname=$1
	local maxsize=$2

	imgsize=`ls -l $imgname | awk '{print $5}'`
	echo "$imgname $imgsize"
	if [ $imgsize -gt $maxsize ] ; then
		echo "$imgname is out of size. $maxsize"
        exit 1
	fi
}
