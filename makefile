IMAGE_TAG=fast_json-schema-docker

WITH_VOLUMES=-v ${PWD}/:/fast_json-schema/ -v fast_json-schema-bundle:/bundle
WITH_DEBUG_FLAGS=--cap-add=SYS_PTRACE --security-opt seccomp=unconfined

build-image:
	docker build . -t ${IMAGE_TAG} --rm

sh:
	docker run -it --rm ${WITH_VOLUMES} ${WITH_DEBUG_FLAGS} ${IMAGE_TAG} sh
