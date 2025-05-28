#!/bin/sh
readonly image=ghcr.io/userver-framework/ubuntu-22.04-userver:latest
readonly user=${HOST_UID:-$(id -u)}
readonly cwd=/home/userver
readonly network=dev

opts="-w ${cwd} -v .:${cwd} \
      -v ./../middlewares:${cwd}/middlewares \
      -v ./../jwt-cpp:${cwd}/jwt-cpp \
      --cap-add SYS_PTRACE --security-opt seccomp=unconfined \
      -e CCACHE_DIR=${cwd}/.ccache"

start=f
nobuild=f
preset=debug
port=8080

while [ $# -ne 0 ]; do
	case $1 in
		-r|--release)
			preset=release
			shift;;
		-d|--debug)
			preset=debug
			shift;;
		-s|--start)
			start=t
			shift;;
		-p|--port)
			port=$2
			shift; shift;;
		-n|--no-build)
			nobuild=t
			shift;;
		-*|--*)
			echo "Unknown option $1"
			exit 1;;
		*)
			cd $1
			break
	esac
done

if [ ! -f CMakeLists.txt ]; then
	echo "This script must be executed in a service directory"
	exit 1
fi

[ ! -d middlewares ] && mkdir middlewares
[ ! -d jwt-cpp ] && mkdir jwt-cpp

if ! docker network inspect ${network} >/dev/null 2>&1; then
	docker network create ${network}
fi

cmd="cmake --preset ${preset} && cmake --build build-${preset} --parallel"

if [ ${nobuild} = t ]; then
	cmd="true"
fi

if [ ${start} = t ]; then
	cmd="${cmd} && ./build-${preset}/${PWD##*/} -c config.yaml"
	opts="--network ${network} -p ${port}:8080 --name ${PWD##*/} ${opts}"
fi

exec docker run --rm -it ${opts} ${image} sh -c \
"useradd -u ${user} -s /usr/sbin/nologin userver && chown -R userver /home/userver && sudo -E -u userver sh -c \"${cmd}\""
