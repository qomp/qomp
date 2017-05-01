#!/usr/bin/env bash
#Please set qomp path manually
qomp_path=$(pwd)
#
#COLORS
red="\e[0;31m"
green="\e[0;32m"
nocolor="\x1B[0m"
pink="\x1B[01;91m"
yellow="\x1B[01;93m"
blue="\x1B[01;94m"
#
#constans
version="0"
libqomp_defines="${qomp_path}/libqomp/src/defines.h"
macx_file=${qomp_path}/Info.plist
android_file=${qomp_path}/android/AndroidManifest.xml
plugins_dir=${qomp_path}/plugins
#temporary vars
isloop=1

die() { echo "${red}$@${nocolor}"; exit 1; }

check_qomp_dir()
{
	if [ ! -d "${qomp_path}" ]; then
		echo -e "${red}qomp project dir doesn't exists! Please enter path to qomp project:${nocolor}"
		echo ""
		read qomp_dir
		if [ -d "${qomp_dir}" ]; then
			qomp_path=${qomp_dir}
			isloop=0
		fi
	else
		echo -e "${blue}Qomp dir ${qomp_path} exists!${nocolor}"
		isloop=0
	fi
}

check_version()
{
	echo -e "${red}Version not set! Please enter version number:${nocolor}"
	read ver
	if [ ! -z "${ver}" ];then
		version=${ver}
		echo -e "${blue}Version: ${pink}$version${nocolor}"
		isloop=0
	fi
}

do_libqomp()
{
	for deffile in ${libqomp_defines}; do
		if [ -f "${deffile}" ];then
			echo -e "${blue}Patching file ${yellow}${deffile}${blue}...${nocolor}"
			sed -e 's/_VERSION[[:space:]]\"\([0-9]\.\)\+[0-9]\"/_VERSION \"'${version}'\"/' -i ${deffile}
		fi
	done
	#libqomp_cmake_file=${qomp_path}/libqomp/CMakeLists.txt
	#if [ -f ${libqomp_cmake_file} ]; then
	#	maj_ver=${version:0:1}
	#	min_ver=${version:2:1}
	#	patch_ver=${version:4:1}
	#	if [ ! "${patch_ver}" ]; then
	#		patch_ver=0
	#	fi
	#	echo -e "${blue}Patching file ${yellow}${libqomp_cmake_file}${blue}...${nocolor}"
	#	sed -e 's/QOMP_LIB_VERSION_MAJOR[[:space:]][0-9]/QOMP_LIB_VERSION_MAJOR '${maj_ver}'/' -i ${libqomp_cmake_file}
	#	sed -e 's/QOMP_LIB_VERSION_MINOR[[:space:]][0-9]/QOMP_LIB_VERSION_MINOR '${min_ver}'/' -i ${libqomp_cmake_file}
	#	sed -e 's/QOMP_LIB_VERSION_PATCH[[:space:]][0-9]/QOMP_LIB_VERSION_PATCH '${patch_ver}'/' -i ${libqomp_cmake_file}
	#fi
}

do_macx()
{
	if [ -f "${macx_file}" ];then
		echo -e "${blue}Patching file ${yellow}${macx_file}${blue}...${nocolor}"
		sed -e 's/<string>qomp[[:space:]]\([0-9]\.\)\+[0-9]<\/string>/<string>qomp '${version}'<\/string>/' -i ${macx_file}
	fi
}

do_android()
{
	if [ -f "${android_file}" ];then
		echo -e "${blue}Patching file ${yellow}${android_file}${blue}...${nocolor}"
		sed -e 's/android:versionName=\"\([0-9]\.\)\+[0-9]\"/android:versionName=\"'${version}'\"/' -i ${android_file}
		cd ${qomp_path}/android
		old_ver=$(grep 'android:versionCode' *.xml | cut -d '"' -f 8 | cut -d " " -f 1)
		let new_ver=10#$old_ver+1
		cd ${qomp_path}
		sed -e 's/android:versionCode=\"[0-9]\+\"/android:versionCode=\"'${new_ver}'\"/' -i ${android_file}
	fi
}

do_plugin()
{
	if [ ! -z "$1" ];then
		plugin_path=${plugins_dir}/$1
		cd ${plugin_path}
		plugdef_suffix=$(ls|grep defines.h)
		if [ ! -z "${plugdef_suffix}" ];then
			plugdef=${plugin_path}/${plugdef_suffix}
			echo -e "${blue}Patching file ${yellow}${plugdef}${blue}...${nocolor}"
			sed -e 's/\"\([0-9]\.\)\+[0-9]\"/\"'${version}'\"/' -i ${plugdef}
		else
			plugfile=${plugin_path}/$1.h
			if [ -f "${plugfile}" ];then
				echo -e "${blue}Patching file ${yellow}${plugfile}${blue}...${nocolor}"
				sed -e 's/version()[[:space:]]const[[:space:]][\{][[:space:]]return[[:space:]]\"\([0-9]\.\)\+[0-9]/version() const \{ return \"'${version}'/' -i ${plugfile}
			fi
		fi
		metadata=${plugin_path}/metadata.json
		if [ -f "${metadata}" ];then
			echo -e "${blue}Patching file ${yellow}${metadata}${blue}...${nocolor}"
			sed -e 's/version\":[[:space:]]\"\([0-9]\.\)\+[0-9]\"/version\": \"'${version}'\"/' -i ${metadata}
		fi
	fi
}

do_plugins()
{
	pluglist=$(ls ${plugins_dir}/)
	for plugin in ${pluglist};do
		if [ -d "${plugins_dir}/${plugin}" ];then
			echo -e "${blue}Processing plugin: ${pink}${plugin}${nocolor}"
			do_plugin ${plugin}
		fi
	done
}

run_all()
{
	do_libqomp
	do_macx
	do_android
	do_plugins
}

while [ ${isloop} = 1 ];do
	check_qomp_dir
done

if [ ! -z "$1" ]; then
	if [ ! -z "$2" ];then
		if [ "$1" == "-p" ] || [ "$1" == "--plugin" ]; then
			if [ ! -z "$2" ];then
				if [ ! -z "$3" ]; then
					version=$3
					echo -e "${blue}Version: ${pink}$version${nocolor}"
					do_plugin $2
				else
					die "Plugin version not set!!!"
				fi
			else
				die "Plugin name not set!!!"
			fi
		fi
	else
		if [ "$1" == "-h" ] || [ "$1" == "--help" ];then
			echo -e "
${blue}Help information:${nocolor}
${red}./setver.sh [option] version${nocolor}
${blue}available options:${nocolor}
${yellow}-h|--help${nocolor}	${blue}print this help${nocolor}
${yellow}-p|--plugin PLUGINNAME${nocolor} 	${blue}set plugin name${nocolor}
"
		else
			if [ ! -z "$1" ]; then
				version=$1
				echo -e "${blue}Version: ${pink}$version${nocolor}"
				run_all
			else
				die "Version not set!!!"
			fi
		fi
	fi
else
	isloop=1
	while [ ${isloop} = 1 ]; do
		check_version
	done
	run_all
fi
